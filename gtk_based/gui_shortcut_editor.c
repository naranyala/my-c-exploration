
#include <dirent.h>
#include <gtk/gtk.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FILES 1000
#define MAX_PATH 4096
#define CENTRAL_DIR "/usr/share/applications"

// Global variables
GtkWidget *window, *listbox, *scrolled_window;
char *files[MAX_FILES];
int file_count = 0;
GtkWidget *current_edit_window = NULL;

// Desktop file structure
typedef struct {
  char *type;          // Application
  char *version;       // 1.0
  char *name;          // Display name
  char *generic_name;  // Generic name
  char *comment;       // Tooltip
  char *icon;          // Icon path
  char *exec;          // Command
  char *path;          // Working directory
  char *terminal;      // Run in terminal
  char *categories;    // Categories
  char *keywords;      // Keywords
  char *mime_types;    // MIME types
  char *original_path; // Original file path
} DesktopFile;

// Function prototypes
DesktopFile *parse_desktop_file(const char *path);
void save_desktop_file(DesktopFile *df);
void free_desktop_file(DesktopFile *df);
void scan_directories();
void populate_listbox();
void on_item_clicked(GtkListBox *box, GtkListBoxRow *row, gpointer data);
void create_editor_window(DesktopFile *df);
void on_save_clicked(GtkWidget *widget, gpointer data);
void on_cancel_clicked(GtkWidget *widget, gpointer data);

// Main window
int main(int argc, char **argv) {
  gtk_init(&argc, &argv);

  // Scan for desktop files
  scan_directories();

  // Create main window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Desktop File Editor");
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Create vertical box
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // Create header
  GtkWidget *header = gtk_header_bar_new();
  gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Desktop Files");
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);

  // Refresh button
  GtkWidget *refresh_btn = gtk_button_new_with_label("Refresh");
  g_signal_connect_swapped(refresh_btn, "clicked", G_CALLBACK(populate_listbox),
                           NULL);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(header), refresh_btn);

  gtk_window_set_titlebar(GTK_WINDOW(window), header);

  // Status label
  char status[100];
  snprintf(status, sizeof(status), "Found %d .desktop files", file_count);
  GtkWidget *status_label = gtk_label_new(status);
  gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

  // Create scrolled window for list
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

  // Create listbox
  listbox = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(listbox), GTK_SELECTION_SINGLE);
  g_signal_connect(listbox, "row-activated", G_CALLBACK(on_item_clicked), NULL);
  gtk_container_add(GTK_CONTAINER(scrolled_window), listbox);

  // Populate list
  populate_listbox();

  // Show window
  gtk_widget_show_all(window);
  gtk_main();

  // Cleanup
  for (int i = 0; i < file_count; i++)
    free(files[i]);

  return 0;
}

// Parse desktop file
DesktopFile *parse_desktop_file(const char *path) {
  DesktopFile *df = malloc(sizeof(DesktopFile));
  memset(df, 0, sizeof(DesktopFile));
  df->original_path = strdup(path);

  // Set defaults
  df->type = strdup("Application");
  df->version = strdup("1.0");
  df->terminal = strdup("false");

  FILE *fp = fopen(path, "r");
  if (!fp)
    return df;

  char line[512];
  while (fgets(line, sizeof(line), fp)) {
    line[strcspn(line, "\n")] = 0;

    // Skip comments and empty lines
    if (line[0] == '#' || line[0] == '\0')
      continue;

    // Parse key=value pairs
    char *eq = strchr(line, '=');
    if (!eq)
      continue;

    *eq = '\0';
    char *key = line;
    char *value = eq + 1;

    // Trim whitespace
    char *end = key + strlen(key) - 1;
    while (end > key && (*end == ' ' || *end == '\t'))
      end--;
    *(end + 1) = '\0';

    end = value + strlen(value) - 1;
    while (end > value && (*end == ' ' || *end == '\t'))
      end--;
    *(end + 1) = '\0';

    // Assign values
    if (strcmp(key, "Type") == 0) {
      free(df->type);
      df->type = strdup(value);
    } else if (strcmp(key, "Version") == 0) {
      free(df->version);
      df->version = strdup(value);
    } else if (strcmp(key, "Name") == 0) {
      free(df->name);
      df->name = strdup(value);
    } else if (strcmp(key, "GenericName") == 0) {
      free(df->generic_name);
      df->generic_name = strdup(value);
    } else if (strcmp(key, "Comment") == 0) {
      free(df->comment);
      df->comment = strdup(value);
    } else if (strcmp(key, "Icon") == 0) {
      free(df->icon);
      df->icon = strdup(value);
    } else if (strcmp(key, "Exec") == 0) {
      free(df->exec);
      df->exec = strdup(value);
    } else if (strcmp(key, "Path") == 0) {
      free(df->path);
      df->path = strdup(value);
    } else if (strcmp(key, "Terminal") == 0) {
      free(df->terminal);
      df->terminal = strdup(value);
    } else if (strcmp(key, "Categories") == 0) {
      free(df->categories);
      df->categories = strdup(value);
    } else if (strcmp(key, "Keywords") == 0) {
      free(df->keywords);
      df->keywords = strdup(value);
    } else if (strcmp(key, "MimeType") == 0) {
      free(df->mime_types);
      df->mime_types = strdup(value);
    }
  }

  fclose(fp);
  return df;
}

// Save desktop file
void save_desktop_file(DesktopFile *df) {
  FILE *fp = fopen(df->original_path, "w");
  if (!fp)
    return;

  fprintf(fp, "[Desktop Entry]\n");
  fprintf(fp, "Type=%s\n", df->type ? df->type : "Application");
  fprintf(fp, "Version=%s\n", df->version ? df->version : "1.0");
  if (df->name)
    fprintf(fp, "Name=%s\n", df->name);
  if (df->generic_name)
    fprintf(fp, "GenericName=%s\n", df->generic_name);
  if (df->comment)
    fprintf(fp, "Comment=%s\n", df->comment);
  if (df->icon)
    fprintf(fp, "Icon=%s\n", df->icon);
  if (df->exec)
    fprintf(fp, "Exec=%s\n", df->exec);
  if (df->path)
    fprintf(fp, "Path=%s\n", df->path);
  fprintf(fp, "Terminal=%s\n", df->terminal ? df->terminal : "false");
  if (df->categories)
    fprintf(fp, "Categories=%s\n", df->categories);
  if (df->keywords)
    fprintf(fp, "Keywords=%s\n", df->keywords);
  if (df->mime_types)
    fprintf(fp, "MimeType=%s\n", df->mime_types);

  fclose(fp);
}

// Free desktop file
void free_desktop_file(DesktopFile *df) {
  if (!df)
    return;
  free(df->type);
  free(df->version);
  free(df->name);
  free(df->generic_name);
  free(df->comment);
  free(df->icon);
  free(df->exec);
  free(df->path);
  free(df->terminal);
  free(df->categories);
  free(df->keywords);
  free(df->mime_types);
  free(df->original_path);
  free(df);
}

// Scan directories for desktop files
void scan_directories() {
  // Clear existing files
  for (int i = 0; i < file_count; i++)
    free(files[i]);
  file_count = 0;

  char *home = getenv("HOME");
  if (!home)
    return;

  char *dirs[] = {"Desktop", ".local/share/applications", CENTRAL_DIR};

  for (int i = 0; i < 3; i++) {
    char path[MAX_PATH];
    if (strcmp(dirs[i], CENTRAL_DIR) == 0) {
      strcpy(path, dirs[i]);
    } else {
      snprintf(path, sizeof(path), "%s/%s", home, dirs[i]);
    }

    DIR *d = opendir(path);
    if (!d)
      continue;

    struct dirent *ent;
    while ((ent = readdir(d))) {
      if (ent->d_type == DT_REG) {
        const char *ext = strrchr(ent->d_name, '.');
        if (ext && strcmp(ext, ".desktop") == 0) {
          char full_path[MAX_PATH];
          snprintf(full_path, sizeof(full_path), "%s/%s", path, ent->d_name);
          files[file_count++] = strdup(full_path);
          if (file_count >= MAX_FILES)
            break;
        }
      }
    }
    closedir(d);
    if (file_count >= MAX_FILES)
      break;
  }
}

// Populate listbox with found files
void populate_listbox() {
  // Clear existing items
  GList *children = gtk_container_get_children(GTK_CONTAINER(listbox));
  g_list_foreach(children, (GFunc)gtk_widget_destroy, NULL);
  g_list_free(children);

  // Rescan files
  scan_directories();

  // Add files to listbox
  for (int i = 0; i < file_count; i++) {
    char *base = strrchr(files[i], '/');
    if (base)
      base++;
    else
      base = files[i];

    GtkWidget *row = gtk_list_box_row_new();
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(row), hbox);

    // File name
    GtkWidget *label = gtk_label_new(base);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 5);

    // Path (smaller label)
    GtkWidget *path_label = gtk_label_new(files[i]);
    gtk_widget_set_halign(path_label, GTK_ALIGN_END);
    gtk_label_set_ellipsize(GTK_LABEL(path_label), PANGO_ELLIPSIZE_START);
    gtk_widget_set_size_request(path_label, 200, -1);
    gtk_box_pack_end(GTK_BOX(hbox), path_label, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(listbox), row);
  }

  gtk_widget_show_all(listbox);
}

// Handle list item click
void on_item_clicked(GtkListBox *box, GtkListBoxRow *row, gpointer data) {
  if (!row)
    return;

  int index = gtk_list_box_row_get_index(row);
  if (index < 0 || index >= file_count)
    return;

  DesktopFile *df = parse_desktop_file(files[index]);
  create_editor_window(df);
}

// Create editor window
void create_editor_window(DesktopFile *df) {
  if (current_edit_window) {
    gtk_widget_destroy(current_edit_window);
  }

  current_edit_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(current_edit_window), "Edit Desktop File");
  gtk_window_set_default_size(GTK_WINDOW(current_edit_window), 500, 600);
  g_signal_connect(current_edit_window, "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &current_edit_window);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
  gtk_container_add(GTK_CONTAINER(current_edit_window), vbox);

  // Create scrolled window for form
  GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

  // Form container
  GtkWidget *form = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(form), 5);
  gtk_grid_set_column_spacing(GTK_GRID(form), 10);
  gtk_container_add(GTK_CONTAINER(scrolled), form);

// Helper function to add form field
#define ADD_FIELD(label_text, key, value)                                      \
  do {                                                                         \
    GtkWidget *label = gtk_label_new(label_text);                              \
    gtk_widget_set_halign(label, GTK_ALIGN_END);                               \
    gtk_grid_attach(GTK_GRID(form), label, 0, row, 1, 1);                      \
                                                                               \
    GtkWidget *entry = gtk_entry_new();                                        \
    if (value)                                                                 \
      gtk_entry_set_text(GTK_ENTRY(entry), value);                             \
    gtk_grid_attach(GTK_GRID(form), entry, 1, row, 1, 1);                      \
    g_object_set_data(G_OBJECT(current_edit_window), key, entry);              \
    row++;                                                                     \
  } while (0)

  int row = 0;
  ADD_FIELD("Type:", "type", df->type);
  ADD_FIELD("Version:", "version", df->version);
  ADD_FIELD("Name:", "name", df->name);
  ADD_FIELD("Generic Name:", "generic_name", df->generic_name);
  ADD_FIELD("Comment:", "comment", df->comment);
  ADD_FIELD("Icon:", "icon", df->icon);
  ADD_FIELD("Exec Command:", "exec", df->exec);
  ADD_FIELD("Working Path:", "path", df->path);

  // Terminal checkbox
  GtkWidget *term_label = gtk_label_new("Terminal:");
  gtk_widget_set_halign(term_label, GTK_ALIGN_END);
  gtk_grid_attach(GTK_GRID(form), term_label, 0, row, 1, 1);

  GtkWidget *term_check = gtk_check_button_new_with_label("Run in terminal");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(term_check),
                               df->terminal &&
                                   strcmp(df->terminal, "true") == 0);
  gtk_grid_attach(GTK_GRID(form), term_check, 1, row, 1, 1);
  g_object_set_data(G_OBJECT(current_edit_window), "terminal", term_check);
  row++;

  ADD_FIELD("Categories:", "categories", df->categories);
  ADD_FIELD("Keywords:", "keywords", df->keywords);
  ADD_FIELD("MIME Types:", "mime_types", df->mime_types);

  // Buttons
  GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
  gtk_box_set_spacing(GTK_BOX(button_box), 10);
  gtk_box_pack_end(GTK_BOX(vbox), button_box, FALSE, FALSE, 0);

  GtkWidget *cancel_btn = gtk_button_new_with_label("Cancel");
  g_signal_connect(cancel_btn, "clicked", G_CALLBACK(on_cancel_clicked),
                   current_edit_window);
  gtk_container_add(GTK_CONTAINER(button_box), cancel_btn);

  GtkWidget *save_btn = gtk_button_new_with_label("Save");
  g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_clicked), df);
  gtk_container_add(GTK_CONTAINER(button_box), save_btn);
  gtk_widget_set_can_default(save_btn, TRUE);
  gtk_widget_grab_default(save_btn);

  gtk_widget_show_all(current_edit_window);
}

// Save button handler
void on_save_clicked(GtkWidget *widget, gpointer data) {
  DesktopFile *df = (DesktopFile *)data;

// Update values from form
#define GET_TEXT(key)                                                          \
  gtk_entry_get_text(                                                          \
      GTK_ENTRY(g_object_get_data(G_OBJECT(current_edit_window), key)))

#define UPDATE_FIELD(field, key)                                               \
  do {                                                                         \
    free(df->field);                                                           \
    const char *val = GET_TEXT(key);                                           \
    df->field = val && val[0] ? strdup(val) : NULL;                            \
  } while (0)

  UPDATE_FIELD(type, "type");
  UPDATE_FIELD(version, "version");
  UPDATE_FIELD(name, "name");
  UPDATE_FIELD(generic_name, "generic_name");
  UPDATE_FIELD(comment, "comment");
  UPDATE_FIELD(icon, "icon");
  UPDATE_FIELD(exec, "exec");
  UPDATE_FIELD(path, "path");
  UPDATE_FIELD(categories, "categories");
  UPDATE_FIELD(keywords, "keywords");
  UPDATE_FIELD(mime_types, "mime_types");

  // Terminal checkbox
  GtkWidget *term_check =
      g_object_get_data(G_OBJECT(current_edit_window), "terminal");
  free(df->terminal);
  df->terminal = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(term_check))
                     ? strdup("true")
                     : strdup("false");

  // Save file
  save_desktop_file(df);

  // Close window
  gtk_widget_destroy(current_edit_window);
  current_edit_window = NULL;

  // Free desktop file
  free_desktop_file(df);
}

// Cancel button handler
void on_cancel_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *window = GTK_WIDGET(data);
  gtk_widget_destroy(window);
}
