#include "./gtk_related_packages.c"
#include "./gtk_util.h"

// Create a styled label with custom formatting
static GtkWidget *create_feature_label(const char *text) {
  GtkWidget *label = gtk_label_new(text);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_widget_set_margin_start(label, 25);
  gtk_widget_set_margin_top(label, 3);
  gtk_widget_set_margin_bottom(label, 3);

  // Add some styling with markup
  char markup[512];
  snprintf(markup, sizeof(markup), "<span foreground='#555555'>• %s</span>",
           text);
  gtk_label_set_markup(GTK_LABEL(label), markup);

  return label;
}

// Create an expander with package information
static GtkWidget *create_package_expander(PackageItem *item) {
  // Create expander with package name as label
  char header[256];
  snprintf(header, sizeof(header), "<b>%s</b>", item->name);
  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), header);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);

  item->expander = gtk_expander_new(NULL);
  gtk_expander_set_label_widget(GTK_EXPANDER(item->expander), label);
  gtk_widget_set_margin_top(item->expander, 5);
  gtk_widget_set_margin_bottom(item->expander, 5);
  gtk_widget_set_margin_start(item->expander, 10);
  gtk_widget_set_margin_end(item->expander, 10);

  // Create content box
  GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_widget_set_margin_start(content_box, 10);
  gtk_widget_set_margin_end(content_box, 10);
  gtk_widget_set_margin_top(content_box, 5);
  gtk_widget_set_margin_bottom(content_box, 10);

  // Add description
  GtkWidget *desc_label = gtk_label_new(item->description);
  gtk_label_set_xalign(GTK_LABEL(desc_label), 0.0);
  gtk_label_set_line_wrap(GTK_LABEL(desc_label), TRUE);
  gtk_widget_set_margin_bottom(desc_label, 10);
  gtk_box_pack_start(GTK_BOX(content_box), desc_label, FALSE, FALSE, 0);

  // Add separator
  GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_bottom(separator, 5);
  gtk_box_pack_start(GTK_BOX(content_box), separator, FALSE, FALSE, 0);

  // Add features header
  GtkWidget *features_header = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(features_header), "<b>Key Features:</b>");
  gtk_label_set_xalign(GTK_LABEL(features_header), 0.0);
  gtk_widget_set_margin_bottom(features_header, 5);
  gtk_box_pack_start(GTK_BOX(content_box), features_header, FALSE, FALSE, 0);

  // Add features
  item->feature_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  for (int i = 0; i < item->feature_count; i++) {
    GtkWidget *feature_label = create_feature_label(item->features[i]);
    gtk_box_pack_start(GTK_BOX(item->feature_box), feature_label, FALSE, FALSE,
                       0);
  }
  gtk_box_pack_start(GTK_BOX(content_box), item->feature_box, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(item->expander), content_box);
  gtk_widget_show_all(content_box);

  return item->expander;
}

static void app_activate(GtkApp *app, gpointer user_data) {
  // Initialize package data

  int package_count = sizeof(packages) / sizeof(packages[0]);

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
  GtkWidget *subtitle =
      gtk_label_new("Essential C libraries for GTK-based development");
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
  GtkWidget *list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Add all packages to the list
  for (int i = 0; i < package_count; i++) {
    GtkWidget *expander = create_package_expander(&packages[i]);
    gtk_box_pack_start(GTK_BOX(list_box), expander, FALSE, FALSE, 0);
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
