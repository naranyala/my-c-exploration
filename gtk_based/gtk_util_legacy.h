// gtk_util.h
// Single-header GTK utility with:
// - App shell
// - Signal<T> reactivity
// - Event bus
// - Layout helpers (vbox/hbox/grid)
// Compatible with GTK 3

#ifndef GTK_UTIL_H
#define GTK_UTIL_H

#include <gtk/gtk.h>
#include <stdarg.h>
#include <string.h>

// ===========================
// 1. REACTIVE SIGNAL
// ===========================

typedef struct Signal Signal;
typedef void (*SignalCallback)(Signal *sig, gpointer old_val, gpointer new_val,
                               gpointer user_data);

struct Signal {
  gpointer value;
  GList *callbacks;
};

typedef struct {
  SignalCallback cb;
  gpointer user_data;
} _SignalHandler;

static inline Signal *signal_new(gpointer initial_value) {
  Signal *sig = g_malloc(sizeof(Signal));
  sig->value = initial_value;
  sig->callbacks = NULL;
  return sig;
}

static inline void signal_set(Signal *sig, gpointer new_value) {
  gpointer old = sig->value;
  if (old == new_value)
    return;
  sig->value = new_value;
  for (GList *l = sig->callbacks; l; l = l->next) {
    _SignalHandler *h = (_SignalHandler *)l->data;
    h->cb(sig, old, new_value, h->user_data);
  }
}

static inline gpointer signal_get(Signal *sig) { return sig->value; }

static inline guint signal_connect(Signal *sig, SignalCallback callback,
                                   gpointer user_data) {
  _SignalHandler *h = g_malloc(sizeof(_SignalHandler));
  h->cb = callback;
  h->user_data = user_data;
  sig->callbacks = g_list_append(sig->callbacks, h);
  return (guint)g_list_length(sig->callbacks);
}

static inline void signal_free(Signal *sig) {
  g_list_free_full(sig->callbacks, g_free);
  g_free(sig);
}

// ===========================
// 2. EVENT BUS
// ===========================

typedef struct {
  char *topic;
  GCallback callback;
  gpointer user_data;
} _EventSubscription;

static GList *_event_subscribers = NULL;

static inline void event_publish(const char *topic, gpointer data) {
  GList *subs = _event_subscribers;
  while (subs) {
    _EventSubscription *sub = (_EventSubscription *)subs->data;
    if (strcmp(sub->topic, topic) == 0) {
      void (*fn)(const char *, gpointer, gpointer) =
          (void (*)(const char *, gpointer, gpointer))sub->callback;
      fn(topic, data, sub->user_data);
    }
    subs = subs->next;
  }
}

static inline gpointer event_subscribe(const char *topic, GCallback callback,
                                       gpointer user_data) {
  _EventSubscription *sub = g_malloc(sizeof(_EventSubscription));
  sub->topic = g_strdup(topic);
  sub->callback = callback;
  sub->user_data = user_data;
  _event_subscribers = g_list_prepend(_event_subscribers, sub);
  return sub;
}

static inline void event_unsubscribe(gpointer handle) {
  _EventSubscription *sub = (_EventSubscription *)handle;
  _event_subscribers = g_list_remove(_event_subscribers, sub);
  g_free(sub->topic);
  g_free(sub);
}

// ===========================
// 3. LAYOUT UTILITY
// ===========================

typedef struct {
  GtkWidget *root;
} Layout;

// Internal helper: apply uniform margin
static inline void _set_margin(GtkWidget *w, int margin) {
  if (margin > 0) {
    gtk_widget_set_margin_start(w, margin);
    gtk_widget_set_margin_end(w, margin);
    gtk_widget_set_margin_top(w, margin);
    gtk_widget_set_margin_bottom(w, margin);
  }
}

// VBox (vertical box)
static inline Layout layout_vbox(int spacing, int margin) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
  _set_margin(box, margin);
  return (Layout){.root = box};
}

// HBox (horizontal box)
static inline Layout layout_hbox(int spacing, int margin) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
  _set_margin(box, margin);
  return (Layout){.root = box};
}

// Grid layout
static inline Layout layout_grid(int row_spacing, int col_spacing, int margin) {
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), row_spacing);
  gtk_grid_set_column_spacing(GTK_GRID(grid), col_spacing);
  _set_margin(grid, margin);
  return (Layout){.root = grid};
}

// Append to box (vbox/hbox) - GTK 3 compatible
static inline Layout layout_append(Layout layout, GtkWidget *child) {
  gtk_box_pack_start(GTK_BOX(layout.root), child, FALSE, FALSE, 0);
  gtk_widget_show(child); // auto-show
  return layout;
}

// Add multiple children (variadic, NULL-terminated)
static inline Layout layout_append_many(Layout layout, ...) {
  va_list args;
  va_start(args, layout);
  GtkWidget *child;
  while ((child = va_arg(args, GtkWidget *)) != NULL) {
    layout = layout_append(layout, child);
  }
  va_end(args);
  return layout;
}

// Attach to grid: (child, left, top, width, height)
static inline Layout layout_attach(Layout layout, GtkWidget *child, int left,
                                   int top, int width, int height) {
  gtk_grid_attach(GTK_GRID(layout.root), child, left, top, width, height);
  gtk_widget_show(child);
  return layout;
}

// Finalize: set as window child - GTK 3 compatible
static inline void layout_apply_to_window(GtkWindow *window, Layout layout) {
  gtk_container_add(GTK_CONTAINER(window), layout.root);
  gtk_widget_show_all(GTK_WIDGET(window));
}

// ===========================
// 4. GTK APP WRAPPER
// ===========================

typedef struct GtkApp {
  GtkApplication *app;
  const char *app_id;
  const char *title;
  int width;
  int height;
  GtkWidget *window; // Store window reference
  void (*activate_cb)(struct GtkApp *self, gpointer user_data);
  gpointer user_data;
} GtkApp;

static void _gtk_app_on_activate(GApplication *gapp, gpointer user_data) {
  GtkApp *self = (GtkApp *)user_data;
  self->window = gtk_application_window_new(GTK_APPLICATION(gapp));
  gtk_window_set_title(GTK_WINDOW(self->window), self->title);
  gtk_window_set_default_size(GTK_WINDOW(self->window), self->width,
                              self->height);

  if (self->activate_cb) {
    self->activate_cb(self, self->user_data);
  }

  gtk_widget_show_all(self->window);
}

static inline GtkApp gtk_app_new(const char *app_id, const char *title,
                                 int width, int height) {
  GtkApp app = {0};
  app.app_id = app_id ? app_id : "com.example.app";
  app.title = title ? title : "Application";
  app.width = width > 0 ? width : 640;
  app.height = height > 0 ? height : 480;
  app.window = NULL;
  return app;
}

static inline void gtk_app_set_activate_cb(GtkApp *app,
                                           void (*cb)(GtkApp *, gpointer),
                                           gpointer user_data) {
  app->activate_cb = cb;
  app->user_data = user_data;
}

static inline int gtk_app_run(GtkApp *app, int argc, char *argv[]) {
  app->app = gtk_application_new(app->app_id, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app->app, "activate", G_CALLBACK(_gtk_app_on_activate), app);
  int status = g_application_run(G_APPLICATION(app->app), argc, argv);
  g_object_unref(app->app);
  return status;
}

#endif // GTK_UTIL_H
