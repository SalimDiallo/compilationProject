#include "grammar_ui.h"
#include "../config/utils.h"
#include <string.h>

void display_grammar(AppData *app) {
    if (app->grammar_list == NULL) return;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->grammar_text));
    gtk_text_buffer_set_text(buffer, "", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);

    char line[256];
    node *current = app->grammar_list;
    while (current != NULL) {
        snprintf(line, sizeof(line), "%c %s: ",
                current->value,
                current->is_final ? "*" : " ");
        gtk_text_buffer_insert(buffer, &iter, line, -1);

        children *trans = current->transitions;
        int first = 1;
        while (trans != NULL) {
            if (!first) {
                gtk_text_buffer_insert(buffer, &iter, " | ", -1);
            }
            snprintf(line, sizeof(line), "->%c%c", trans->value, trans->transition);
            gtk_text_buffer_insert(buffer, &iter, line, -1);
            trans = trans->next;
            first = 0;
        }

        gtk_text_buffer_insert(buffer, &iter, "\n", -1);
        current = current->next;
    }
}

void create_main_window(AppData *app) {
    // Create main window
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "Grammar Checker");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 700, 600);
    gtk_container_set_border_width(GTK_CONTAINER(app->window), 10);
    g_signal_connect(app->window, "destroy", G_CALLBACK(on_window_destroy), app);

    // Create main vertical box
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(app->window), vbox);

    // Create notebook with tabs
    app->notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), app->notebook, FALSE, FALSE, 0);

    // ===== TAB 1: Load from File =====
    GtkWidget *file_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(file_tab), 10);

    GtkWidget *file_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *file_label = gtk_label_new("Grammar File:");
    app->file_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(app->file_entry), "input/grammaire.txt");
    GtkWidget *load_button = gtk_button_new_with_label("Load Grammar");
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_grammar_file), app);

    gtk_box_pack_start(GTK_BOX(file_hbox), file_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(file_hbox), app->file_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(file_hbox), load_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(file_tab), file_hbox, FALSE, FALSE, 0);

    GtkWidget *tab1_label = gtk_label_new("Load from File");
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), file_tab, tab1_label);

    GtkWidget *input_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(input_tab), 10);

    GtkWidget *input_label = gtk_label_new("Enter Grammar (format: A -> bB):");
    gtk_box_pack_start(GTK_BOX(input_tab), input_label, FALSE, FALSE, 0);

    GtkWidget *scrolled_input = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_input),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_input, -1, 100);

    app->grammar_input = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->grammar_input), GTK_WRAP_WORD);
    GtkTextBuffer *input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->grammar_input));
    gtk_text_buffer_set_text(input_buffer, "A -> bB\nB -> cB\nB -> \\t", -1);
    gtk_container_add(GTK_CONTAINER(scrolled_input), app->grammar_input);
    gtk_box_pack_start(GTK_BOX(input_tab), scrolled_input, TRUE, TRUE, 0);

    GtkWidget *parse_button = gtk_button_new_with_label("Parse Grammar");
    g_signal_connect(parse_button, "clicked", G_CALLBACK(on_parse_grammar_input), app);
    gtk_box_pack_start(GTK_BOX(input_tab), parse_button, FALSE, FALSE, 0);

    GtkWidget *tab2_label = gtk_label_new("Manual Input");
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), input_tab, tab2_label);

    GtkWidget *grammar_label = gtk_label_new("Loaded Grammar:");
    gtk_box_pack_start(GTK_BOX(vbox), grammar_label, FALSE, FALSE, 0);

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 150);

    app->grammar_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->grammar_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->grammar_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), app->grammar_text);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), separator, FALSE, FALSE, 5);

    GtkWidget *string_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *string_label = gtk_label_new("Test String:");
    app->string_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->string_entry), "Enter string to check...");
    GtkWidget *check_button = gtk_button_new_with_label("Check String");
    g_signal_connect(check_button, "clicked", G_CALLBACK(on_check_string), app);

    gtk_box_pack_start(GTK_BOX(string_hbox), string_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(string_hbox), app->string_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(string_hbox), check_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), string_hbox, FALSE, FALSE, 0);

    app->result_label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(app->result_label), "<span font='14'>Result will appear here</span>");
    gtk_box_pack_start(GTK_BOX(vbox), app->result_label, FALSE, FALSE, 10);

    app->grammar_list = NULL;
}

void on_load_grammar_file(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppData *app = (AppData *)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(app->file_entry));

    app->grammar_list = get_grammaire_list((char *)filename);

    if (app->grammar_list == NULL) {
        gtk_label_set_markup(GTK_LABEL(app->result_label),
                            "<span foreground='red' font='14'>Error: Could not load grammar file</span>");
        return;
    }

    display_grammar(app);
    gtk_label_set_markup(GTK_LABEL(app->result_label),
                        "<span foreground='green' font='14'>Grammar loaded successfully from file!</span>");
}

void on_parse_grammar_input(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppData *app = (AppData *)data;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->grammar_input));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (text == NULL || strlen(text) == 0) {
        gtk_label_set_markup(GTK_LABEL(app->result_label),
                            "<span foreground='red' font='14'>Error: No grammar text entered</span>");
        g_free(text);
        return;
    }

    app->grammar_list = parse_grammaire_text(text);
    g_free(text);

    if (app->grammar_list == NULL) {
        gtk_label_set_markup(GTK_LABEL(app->result_label),
                            "<span foreground='red' font='14'>Error: Could not parse grammar</span>");
        return;
    }

    display_grammar(app);
    gtk_label_set_markup(GTK_LABEL(app->result_label),
                        "<span foreground='green' font='14'>Grammar parsed successfully!</span>");
}

void on_check_string(GtkWidget *widget, gpointer data) {
    (void)widget;
    AppData *app = (AppData *)data;

    if (app->grammar_list == NULL) {
        gtk_label_set_markup(GTK_LABEL(app->result_label),
                            "<span foreground='red' font='14'>Please load a grammar first!</span>");
        return;
    }

    const char *test_string = gtk_entry_get_text(GTK_ENTRY(app->string_entry));

    if (strlen(test_string) == 0) {
        gtk_label_set_markup(GTK_LABEL(app->result_label),
                            "<span foreground='orange' font='14'>Please enter a string to check</span>");
        return;
    }

    int result = check_string((char *)test_string, app->grammar_list);

    char result_text[256];
    if (result) {
        snprintf(result_text, sizeof(result_text),
                "<span foreground='green' font='16' weight='bold'>✓ String \"%s\" is ACCEPTED</span>",
                test_string);
    } else {
        snprintf(result_text, sizeof(result_text),
                "<span foreground='red' font='16' weight='bold'>✗ String \"%s\" is REJECTED</span>",
                test_string);
    }

    gtk_label_set_markup(GTK_LABEL(app->result_label), result_text);
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    (void)widget;
    (void)data;
    gtk_main_quit();
}
