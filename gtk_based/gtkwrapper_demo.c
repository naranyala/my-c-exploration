#include "./gtk_util.h"

static Signal *counter;

void on_inc_clicked(GtkWidget *btn, gpointer _) {
  int v = GPOINTER_TO_INT(signal_get(counter));
  signal_set(counter, GINT_TO_POINTER(v + 1));
}

void update_label(Signal *sig, gpointer old, gpointer new, gpointer user_data) {
  char buf[32];
  snprintf(buf, sizeof(buf), "Count: %d", GPOINTER_TO_INT(new));
  gtk_label_set_text(GTK_LABEL(user_data), buf);
}

static void app_activate(GtkApp *app, gpointer _) {
  GtkWidget *label = gtk_label_new("Count: 0");
  GtkWidget *btn = gtk_button_new_with_label("Increment");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_inc_clicked), NULL);

  signal_connect(counter, update_label, label);

  Layout layout = layout_vbox(10, 15);
  layout = layout_append_many(layout, label, btn, NULL);

  layout_apply_to_window(GTK_WINDOW(app->window), layout);
}

int main(int argc, char *argv[]) {
  counter = signal_new(GINT_TO_POINTER(0));

  GtkApp app = gtk_app_new("com.example.layout", "Layout Demo", 250, 150);
  gtk_app_set_activate_cb(&app, app_activate, NULL);

  return gtk_app_run(&app, argc, argv);
}
