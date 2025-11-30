#ifndef GRAMMAR_UI_H
#define GRAMMAR_UI_H

#include <gtk/gtk.h>
#include "../config/graph.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *file_entry;
    GtkWidget *string_entry;
    GtkWidget *result_label;
    GtkWidget *grammar_text;
    GtkWidget *grammar_input;
    GtkWidget *notebook;
    list grammar_list;
} AppData;

void create_main_window(AppData *app);
void on_load_grammar_file(GtkWidget *widget, gpointer data);
void on_parse_grammar_input(GtkWidget *widget, gpointer data);
void on_check_string(GtkWidget *widget, gpointer data);
void on_window_destroy(GtkWidget *widget, gpointer data);

#endif // GRAMMAR_UI_H
