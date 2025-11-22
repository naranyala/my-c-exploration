
#include <gtk/gtk.h>

static void on_enter(GtkEntry *entry, gpointer data) {
    const char *cmd = gtk_entry_get_text(entry);
    g_print("Command: %s\n", cmd);
}

int main(int argc, char **argv) {
    gtk_init();

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Command Bar");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter command...");
    gtk_box_append(GTK_BOX(box), entry);

    g_signal_connect(entry, "activate", G_CALLBACK(on_enter), NULL);

    gtk_widget_show(window);
    gtk_main();
    return 0;
}
