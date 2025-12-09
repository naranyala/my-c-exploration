#include "./gtk_related_packages.c"
#include "./gtk_util.h"

static int package_count = sizeof(packages) / sizeof(packages[0]);

// Create a styled feature label
static GtkWidget *create_feature_label(const char *text) {
  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_margin_start(label, 25);
  gtk_widget_set_margin_top(label, 3);
  gtk_widget_set_margin_bottom(label, 3);
  gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(label), 60);

  char markup[512];
  snprintf(markup, sizeof(markup), "<span foreground='#555555'>• %s</span>",
           text);
  gtk_label_set_markup(GTK_LABEL(label), markup);

  return label;
}

// Show modal dialog with package details
static void show_package_modal(GtkWidget *parent_window, PackageItem *package) {
  // Create modal dialog
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      package->name, GTK_WINDOW(parent_window),
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Close",
      GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 400);

  // Get content area
  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_widget_set_margin_start(content_area, 20);
  gtk_widget_set_margin_end(content_area, 20);
  gtk_widget_set_margin_top(content_area, 20);
  gtk_widget_set_margin_bottom(content_area, 20);

  // Create scrolled window for content
  GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_vexpand(scrolled, TRUE);
  gtk_widget_set_hexpand(scrolled, TRUE);

  // Create content box
  GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
  gtk_widget_set_margin_start(content_box, 10);
  gtk_widget_set_margin_end(content_box, 10);
  gtk_widget_set_margin_top(content_box, 10);
  gtk_widget_set_margin_bottom(content_box, 10);

  // Add package name header
  GtkWidget *name_label = gtk_label_new(NULL);
  char name_markup[256];
  snprintf(name_markup, sizeof(name_markup),
           "<span size='x-large' weight='bold'>%s</span>", package->name);
  gtk_label_set_markup(GTK_LABEL(name_label), name_markup);
  gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
  gtk_widget_set_margin_bottom(name_label, 10);
  gtk_box_pack_start(GTK_BOX(content_box), name_label, FALSE, FALSE, 0);

  // Add separator
  GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_bottom(sep1, 10);
  gtk_box_pack_start(GTK_BOX(content_box), sep1, FALSE, FALSE, 0);

  // Add description
  GtkWidget *desc_header = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(desc_header), "<b>Description</b>");
  gtk_label_set_xalign(GTK_LABEL(desc_header), 0.0);
  gtk_widget_set_margin_bottom(desc_header, 5);
  gtk_box_pack_start(GTK_BOX(content_box), desc_header, FALSE, FALSE, 0);

  GtkWidget *desc_label = gtk_label_new(package->description);
  gtk_label_set_xalign(GTK_LABEL(desc_label), 0.0);
  gtk_label_set_line_wrap(GTK_LABEL(desc_label), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(desc_label), 60);
  gtk_widget_set_margin_start(desc_label, 10);
  gtk_widget_set_margin_bottom(desc_label, 15);
  gtk_box_pack_start(GTK_BOX(content_box), desc_label, FALSE, FALSE, 0);

  // Add separator
  GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_bottom(sep2, 10);
  gtk_box_pack_start(GTK_BOX(content_box), sep2, FALSE, FALSE, 0);

  // Add features header
  GtkWidget *features_header = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(features_header), "<b>Key Features</b>");
  gtk_label_set_xalign(GTK_LABEL(features_header), 0.0);
  gtk_widget_set_margin_bottom(features_header, 10);
  gtk_box_pack_start(GTK_BOX(content_box), features_header, FALSE, FALSE, 0);

  // Add features
  for (int i = 0; i < package->feature_count; i++) {
    GtkWidget *feature_label = create_feature_label(package->features[i]);
    gtk_box_pack_start(GTK_BOX(content_box), feature_label, FALSE, FALSE, 0);
  }

  gtk_container_add(GTK_CONTAINER(scrolled), content_box);
  gtk_box_pack_start(GTK_BOX(content_area), scrolled, TRUE, TRUE, 0);

  gtk_widget_show_all(dialog);

  // Run dialog and destroy when closed
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

// Callback when list item is clicked
static void on_package_clicked(GtkListBox *list_box, GtkListBoxRow *row,
                               gpointer user_data) {
  GtkWidget *parent_window = GTK_WIDGET(user_data);
  int index = gtk_list_box_row_get_index(row);

  if (index >= 0 && index < package_count) {
    show_package_modal(parent_window, &packages[index]);
  }
}

// Create a list row for a package
static GtkWidget *create_package_row(PackageItem *package) {
  GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_start(row_box, 15);
  gtk_widget_set_margin_end(row_box, 15);
  gtk_widget_set_margin_top(row_box, 12);
  gtk_widget_set_margin_bottom(row_box, 12);

  // Package name
  GtkWidget *name_label = gtk_label_new(NULL);
  char markup[256];
  snprintf(markup, sizeof(markup), "<b>%s</b>", package->name);
  gtk_label_set_markup(GTK_LABEL(name_label), markup);
  gtk_label_set_xalign(GTK_LABEL(name_label), 0.0);
  gtk_box_pack_start(GTK_BOX(row_box), name_label, FALSE, FALSE, 0);

  // Short description
  GtkWidget *desc_label = gtk_label_new(package->description);
  gtk_label_set_xalign(GTK_LABEL(desc_label), 0.0);
  gtk_label_set_line_wrap(GTK_LABEL(desc_label), TRUE);
  gtk_label_set_max_width_chars(GTK_LABEL(desc_label), 70);

  // Make description smaller and gray
  char desc_markup[512];
  snprintf(desc_markup, sizeof(desc_markup),
           "<span foreground='#666666' size='small'>%s</span>",
           package->description);
  gtk_label_set_markup(GTK_LABEL(desc_label), desc_markup);
  gtk_box_pack_start(GTK_BOX(row_box), desc_label, FALSE, FALSE, 0);

  return row_box;
}

static void app_activate(GtkApp *app, gpointer user_data) {
  // Create main layout
  Layout main_layout = layout_vbox(0, 0);

  // Add title header
  GtkWidget *title = gtk_label_new(NULL);
  gtk_label_set_markup(
      GTK_LABEL(title),
      "<span size='x-large' weight='bold'>GTK Development Libraries</span>");
  gtk_widget_set_margin_top(title, 15);
  gtk_widget_set_margin_bottom(title, 10);
  main_layout = layout_append(main_layout, title);

  // Add subtitle
  GtkWidget *subtitle = gtk_label_new("Click on any library to view details");
  gtk_widget_set_margin_bottom(subtitle, 15);
  main_layout = layout_append(main_layout, subtitle);

  // Add separator
  GtkWidget *top_separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_start(top_separator, 10);
  gtk_widget_set_margin_end(top_separator, 10);
  gtk_widget_set_margin_bottom(top_separator, 10);
  main_layout = layout_append(main_layout, top_separator);

  // Create scrolled window for the list
  GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                 GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled),
                                             300);
  gtk_widget_set_vexpand(scrolled, TRUE);
  gtk_widget_set_hexpand(scrolled, TRUE);

  // Create list box for packages
  GtkWidget *list_box = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_NONE);

  // Add visual styling to list box
  g_signal_connect(list_box, "row-activated", G_CALLBACK(on_package_clicked),
                   app->window);

  // Add all packages to the list
  for (int i = 0; i < package_count; i++) {
    GtkWidget *row_content = create_package_row(&packages[i]);
    gtk_list_box_insert(GTK_LIST_BOX(list_box), row_content, -1);
  }

  gtk_container_add(GTK_CONTAINER(scrolled), list_box);
  gtk_widget_show_all(list_box);

  main_layout = layout_append(main_layout, scrolled);

  // Make sure the scrolled window expands properly
  gtk_box_set_child_packing(GTK_BOX(main_layout.root), scrolled, TRUE, TRUE, 0,
                            GTK_PACK_START);

  // Apply layout to window
  layout_apply_to_window(GTK_WINDOW(app->window), main_layout);
}

int main(int argc, char *argv[]) {
  GtkApp app = gtk_app_new("com.example.gtk.packages",
                           "GTK Development Libraries", 600, 500);
  gtk_app_set_activate_cb(&app, app_activate, NULL);

  return gtk_app_run(&app, argc, argv);
}
