#include <gtk/gtk.h>
#include "grammar_ui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppData app;
    create_main_window(&app);

    gtk_widget_show_all(app.window);

    gtk_main();
    return 0;
}
