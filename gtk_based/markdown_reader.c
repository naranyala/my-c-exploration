// markdown_reader.c
// Clean, fast Markdown reader using only libcmark + WebKitGTK
// No cmark-gtk required!

#include "gtk_util.h"
#include <cmark.h>
#include <gio/gio.h>
#include <webkit2/webkit2.h>

static GtkWidget *webview = NULL;
static Signal *sig_markdown = NULL;

// ----------------------------------------------------
// Render Markdown → HTML and load into WebKit
// ----------------------------------------------------
static void render_markdown(Signal *sig, gpointer old_val, gpointer new_val,
                            gpointer user_data) {
  const char *md = (const char *)new_val;
  if (!md)
    md = "";

  char *html_body = cmark_markdown_to_html(md, strlen(md), CMARK_OPT_DEFAULT);

  // Embedded minimal but nice CSS (supports dark mode via prefers-color-scheme)
  const char *html =
      "<!DOCTYPE html>"
      "<html><head><meta charset=\"utf-8\">"
      "<style>"
      "body { font-family: system-ui, sans-serif; max-width: 800px; margin: "
      "40px auto; padding: 0 20px; line-height: 1.7; }"
      "h1, h2, h3, h4, h5, h6 { margin-top: 1.8em; color: #2c3e50; }"
      "code { background: rgba(0,0,0,0.07); padding: 2px 6px; border-radius: "
      "4px; font-size: 90%; }"
      "pre { background: #282c34; color: #abb2bf; padding: 16px; "
      "border-radius: 8px; overflow-x: auto; }"
      "pre code { background: none; padding: 0; }"
      "blockquote { border-left: 4px solid #3498db; padding-left: 16px; "
      "margin: 16px 0; color: #555; }"
      "table { border-collapse: collapse; width: 100%; margin: 20px 0; }"
      "th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }"
      "th { background: #f8f9fa; }"
      "a { color: #3498db; }"
      "@media (prefers-color-scheme: dark) {"
      "  body { background: #1e1e1e; color: #d4d4d4; }"
      "  h1,h2,h3,h4,h5,h6 { color: #569cd6; }"
      "  code { background: #3e3e3e; }"
      "  pre { background: #252526; }"
      "  blockquote { border-color: #569cd6; }"
      "  th { background: #2d2d30; }"
      "  a { color: #4fc1ff; }"
      "}"
      "</style></head>"
      "<body>%s</body></html>";

  char *full_html =
      g_strdup_printf(html, html_body ? html_body : "<p><i>No content</i></p>");
  webkit_web_view_load_html(WEBKIT_WEB_VIEW(webview), full_html, NULL);

  g_free(html_body);
  g_free(full_html);
}

// ----------------------------------------------------
// Load file and update signal
// ----------------------------------------------------
static void load_file(const char *path) {
  char *content = NULL;
  gsize len = 0;
  GError *err = NULL;

  if (g_file_get_contents(path, &content, &len, &err)) {
    signal_set(sig_markdown, content); // takes ownership

    char *basename = g_path_get_basename(path);
    char *title = g_strdup_printf("Markdown Reader — %s", basename);
    gtk_window_set_title(GTK_WINDOW(gtk_widget_get_toplevel(webview)), title);
    g_free(basename);
    g_free(title);
  } else {
    dialog_error(NULL, "Error", err->message);
    g_error_free(err);
  }
}

// ----------------------------------------------------
// File → Open
// ----------------------------------------------------
static void on_open_clicked(GtkMenuItem *item, GtkApp *app) {
  GtkWidget *dialog = gtk_file_chooser_dialog_new(
      "Open Markdown File", GTK_WINDOW(app->window),
      GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open",
      GTK_RESPONSE_ACCEPT, NULL);

  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, "Markdown Files");
  gtk_file_filter_add_pattern(filter, "*.md");
  gtk_file_filter_add_pattern(filter, "*.markdown");
  gtk_file_filter_add_mime_type(filter, "text/markdown");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    load_file(filename);
    g_free(filename);
  }
  gtk_widget_destroy(dialog);
}

// ----------------------------------------------------
// Drag & Drop
// ----------------------------------------------------
static void on_drag_data_received(GtkWidget *widget, GdkDragContext *context,
                                  int x, int y, GtkSelectionData *data,
                                  guint info, guint time, gpointer user_data) {
  gchar **uris = gtk_selection_data_get_uris(data);
  if (uris && uris[0]) {
    gchar *filename = g_filename_from_uri(uris[0], NULL, NULL);
    if (filename) {
      load_file(filename);
      g_free(filename);
    }
  }
  g_strfreev(uris);
  gtk_drag_finish(context, TRUE, FALSE, time);
}

// ----------------------------------------------------
// App activate
// ----------------------------------------------------
static void on_activate(GtkApp *self, gpointer user_data) {
  sig_markdown = signal_new(
      g_strdup("# Markdown Reader\n\n"
               "Welcome! Drop a `.md` file here or use **File → Open**\n\n"
               "- Supports **bold**, *italic*, `code`, links, lists, tables\n"
               "- Syntax highlighted code blocks\n"
               "- Automatic dark mode\n"
               "- Fast, native, beautiful\n\n"
               "Happy reading! ✨"));

  // Layout
  Layout main = layout_vbox(0, 0);

  // Menu bar
  GtkWidget *menubar = gtk_menu_bar_new();
  GtkWidget *file_menu = gtk_menu_new();
  GtkWidget *file_item = gtk_menu_item_new_with_label("File");
  GtkWidget *open_item = gtk_menu_item_new_with_label("Open…");
  GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");

  g_signal_connect(open_item, "activate", G_CALLBACK(on_open_clicked), self);
  g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu),
                        gtk_separator_menu_item_new());
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

  // WebKit view in scrolled window
  Layout scroll = layout_scrolled(GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  webview = webkit_web_view_new();
  gtk_container_add(GTK_CONTAINER(scroll.root), webview);

  // Assemble
  layout_append(main, menubar);
  layout_append_expand(main, scroll.root);

  layout_apply_to_window(GTK_WINDOW(self->window), main);

  // Drag & drop
  gtk_drag_dest_set(self->window, GTK_DEST_DEFAULT_ALL, NULL, 0,
                    GDK_ACTION_COPY);
  gtk_drag_dest_add_uri_targets(self->window);
  g_signal_connect(self->window, "drag-data-received",
                   G_CALLBACK(on_drag_data_received), NULL);

  // Connect rendering
  signal_connect(sig_markdown, render_markdown, NULL);
  render_markdown(sig_markdown, NULL, signal_get(sig_markdown), NULL);
}

// ----------------------------------------------------
// Main
// ----------------------------------------------------
int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  GtkApp app =
      gtk_app_new("com.example.markdownreader", "Markdown Reader", 1100, 750);

  gtk_app_set_activate_cb(&app, on_activate, NULL);
  return gtk_app_run(&app, argc, argv);
}
