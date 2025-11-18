// gcc code_counter_gtk3.c -o code_counter_gtk3 `pkg-config --cflags --libs gtk+-3.0` -Wall

#include <gtk/gtk.h>
#include <glib.h>

typedef struct {
    GtkWidget *text_view;
    GtkWidget *label_status;
} Widgets;

static int count_words(const char *text) {
    int words = 0;
    gboolean in_word = FALSE;

    while (*text) {
        gunichar c = g_utf8_get_char(text);
        if (g_unichar_isalpha(c)) {
            if (!in_word) {
                words++;
                in_word = TRUE;
            }
        } else {
            in_word = FALSE;
        }
        text = g_utf8_next_char(text);
    }
    return words;
}

static void update_counter(GtkTextBuffer *buffer, gpointer user_data) {
    Widgets *w = (Widgets *)user_data;

    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Lines
    int lines = gtk_text_buffer_get_line_count(buffer);

    // Characters (including spaces)
    int chars_with_spaces = gtk_text_iter_get_offset(&end);

    // Characters (excluding spaces)
    int chars_no_spaces = 0;
    const char *p = text;
    while (*p) {
        gunichar c = g_utf8_get_char(p);
        if (!g_unichar_isspace(c))
            chars_no_spaces++;
        p = g_utf8_next_char(p);
    }

    // Words
    int words = count_words(text);

    gchar *status = g_strdup_printf(
        "Lines: %d  │  Words: %d  │  Characters: %d  │  Chars (no spaces): %d",
        lines, words, chars_with_spaces, chars_no_spaces);

    gtk_label_set_text(GTK_LABEL(w->label_status), status);

    g_free(text);
    g_free(status);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Code Counter (GTK3) - Live Stats");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Scrolled window + TextView
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled), text_view);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text_view), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(text_view), 10);

    // Status label
    GtkWidget *label_status = gtk_label_new("Lines: 1  │  Words: 0  │  Characters: 0  │  Chars (no spaces): 0");
    gtk_label_set_xalign(GTK_LABEL(label_status), 0.0);
    gtk_box_pack_start(GTK_BOX(vbox), label_status, FALSE, FALSE, 5);

    // Store widgets
    Widgets *w = g_new(Widgets, 1);
    w->text_view = text_view;
    w->label_status = label_status;

    // Connect signal
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(buffer, "changed", G_CALLBACK(update_counter), w);

    // Initial update
    update_counter(buffer, w);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.CodeCounterGTK3", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
