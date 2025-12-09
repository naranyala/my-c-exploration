// gtk_util.h
// Enhanced single-header GTK utility library
// Features:
// - App shell with window management
// - Signal<T> reactivity system
// - Event bus for pub/sub
// - Layout helpers (vbox/hbox/grid/overlay)
// - Widget builders with fluent API
// - Common dialogs (modal, alert, confirm, input)
// - Style/CSS helpers
// - Timer and idle callbacks

#ifndef GTK_UTIL_H
#define GTK_UTIL_H

#include <gtk/gtk.h>
#include <stdarg.h>
#include <string.h>

// ===========================
// 1. REACTIVE SIGNAL SYSTEM
// ===========================

typedef struct Signal Signal;
typedef void (*SignalCallback)(Signal *sig, gpointer old_val, gpointer new_val,
                               gpointer user_data);

struct Signal {
  gpointer value;
  GList *callbacks;
  void (*free_fn)(gpointer); // Optional cleanup function
};

typedef struct {
  SignalCallback cb;
  gpointer user_data;
} _SignalHandler;

static inline Signal *signal_new(gpointer initial_value) {
  Signal *sig = g_malloc(sizeof(Signal));
  sig->value = initial_value;
  sig->callbacks = NULL;
  sig->free_fn = NULL;
  return sig;
}

static inline Signal *signal_new_with_free(gpointer initial_value,
                                           void (*free_fn)(gpointer)) {
  Signal *sig = signal_new(initial_value);
  sig->free_fn = free_fn;
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
  if (sig->free_fn && sig->value) {
    sig->free_fn(sig->value);
  }
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
// 3. ENHANCED LAYOUT SYSTEM
// ===========================

typedef struct {
  GtkWidget *root;
} Layout;

static inline void _set_margin(GtkWidget *w, int margin) {
  if (margin > 0) {
    gtk_widget_set_margin_start(w, margin);
    gtk_widget_set_margin_end(w, margin);
    gtk_widget_set_margin_top(w, margin);
    gtk_widget_set_margin_bottom(w, margin);
  }
}

static inline void _set_margin_detailed(GtkWidget *w, int top, int right,
                                        int bottom, int left) {
  gtk_widget_set_margin_top(w, top);
  gtk_widget_set_margin_end(w, right);
  gtk_widget_set_margin_bottom(w, bottom);
  gtk_widget_set_margin_start(w, left);
}

// VBox
static inline Layout layout_vbox(int spacing, int margin) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, spacing);
  _set_margin(box, margin);
  return (Layout){.root = box};
}

// HBox
static inline Layout layout_hbox(int spacing, int margin) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, spacing);
  _set_margin(box, margin);
  return (Layout){.root = box};
}

// Grid
static inline Layout layout_grid(int row_spacing, int col_spacing, int margin) {
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), row_spacing);
  gtk_grid_set_column_spacing(GTK_GRID(grid), col_spacing);
  _set_margin(grid, margin);
  return (Layout){.root = grid};
}

// Scrolled Window
static inline Layout layout_scrolled(GtkPolicyType hpolicy,
                                     GtkPolicyType vpolicy) {
  GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), hpolicy,
                                 vpolicy);
  gtk_widget_set_vexpand(scrolled, TRUE);
  gtk_widget_set_hexpand(scrolled, TRUE);
  return (Layout){.root = scrolled};
}

// Frame with label
static inline Layout layout_frame(const char *label, int margin) {
  GtkWidget *frame = gtk_frame_new(label);
  _set_margin(frame, margin);
  return (Layout){.root = frame};
}

// Append to box
static inline Layout layout_append(Layout layout, GtkWidget *child) {
  gtk_box_pack_start(GTK_BOX(layout.root), child, FALSE, FALSE, 0);
  gtk_widget_show(child);
  return layout;
}

// Append with expand
static inline Layout layout_append_expand(Layout layout, GtkWidget *child) {
  gtk_box_pack_start(GTK_BOX(layout.root), child, TRUE, TRUE, 0);
  gtk_widget_show(child);
  return layout;
}

// Append multiple (NULL-terminated)
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

// Attach to grid
static inline Layout layout_attach(Layout layout, GtkWidget *child, int left,
                                   int top, int width, int height) {
  gtk_grid_attach(GTK_GRID(layout.root), child, left, top, width, height);
  gtk_widget_show(child);
  return layout;
}

// Add to scrolled window
static inline Layout layout_add_scrolled_child(Layout layout,
                                               GtkWidget *child) {
  gtk_container_add(GTK_CONTAINER(layout.root), child);
  gtk_widget_show(child);
  return layout;
}

// Add to frame
static inline Layout layout_add_frame_child(Layout layout, GtkWidget *child) {
  gtk_container_add(GTK_CONTAINER(layout.root), child);
  gtk_widget_show(child);
  return layout;
}

// Apply to window
static inline void layout_apply_to_window(GtkWindow *window, Layout layout) {
  gtk_container_add(GTK_CONTAINER(window), layout.root);
  gtk_widget_show_all(GTK_WIDGET(window));
}

// ===========================
// 4. WIDGET BUILDERS
// ===========================

// Button builder
typedef struct {
  GtkWidget *widget;
} WidgetBuilder;

static inline WidgetBuilder widget_button(const char *label) {
  return (WidgetBuilder){.widget = gtk_button_new_with_label(label)};
}

static inline WidgetBuilder widget_button_with_icon(const char *icon_name) {
  GtkWidget *btn = gtk_button_new();
  GtkWidget *icon =
      gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image(GTK_BUTTON(btn), icon);
  return (WidgetBuilder){.widget = btn};
}

static inline WidgetBuilder widget_label(const char *text) {
  return (WidgetBuilder){.widget = gtk_label_new(text)};
}

static inline WidgetBuilder widget_label_markup(const char *markup) {
  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), markup);
  return (WidgetBuilder){.widget = label};
}

static inline WidgetBuilder widget_entry(const char *placeholder) {
  GtkWidget *entry = gtk_entry_new();
  if (placeholder) {
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
  }
  return (WidgetBuilder){.widget = entry};
}

static inline WidgetBuilder widget_text_view(gboolean editable) {
  GtkWidget *text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), editable);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
  return (WidgetBuilder){.widget = text_view};
}

static inline WidgetBuilder widget_checkbox(const char *label) {
  return (WidgetBuilder){.widget = gtk_check_button_new_with_label(label)};
}

static inline WidgetBuilder widget_switch(void) {
  return (WidgetBuilder){.widget = gtk_switch_new()};
}

static inline WidgetBuilder widget_spinner(void) {
  GtkWidget *spinner = gtk_spinner_new();
  gtk_spinner_start(GTK_SPINNER(spinner));
  return (WidgetBuilder){.widget = spinner};
}

// Fluent API methods
static inline WidgetBuilder wb_margin(WidgetBuilder wb, int margin) {
  _set_margin(wb.widget, margin);
  return wb;
}

static inline WidgetBuilder
wb_margin_detailed(WidgetBuilder wb, int top, int right, int bottom, int left) {
  _set_margin_detailed(wb.widget, top, right, bottom, left);
  return wb;
}

static inline WidgetBuilder wb_expand(WidgetBuilder wb, gboolean hexpand,
                                      gboolean vexpand) {
  gtk_widget_set_hexpand(wb.widget, hexpand);
  gtk_widget_set_vexpand(wb.widget, vexpand);
  return wb;
}

static inline WidgetBuilder wb_align(WidgetBuilder wb, GtkAlign halign,
                                     GtkAlign valign) {
  gtk_widget_set_halign(wb.widget, halign);
  gtk_widget_set_valign(wb.widget, valign);
  return wb;
}

static inline WidgetBuilder wb_size(WidgetBuilder wb, int width, int height) {
  gtk_widget_set_size_request(wb.widget, width, height);
  return wb;
}

static inline WidgetBuilder wb_tooltip(WidgetBuilder wb, const char *text) {
  gtk_widget_set_tooltip_text(wb.widget, text);
  return wb;
}

static inline WidgetBuilder wb_css_class(WidgetBuilder wb,
                                         const char *class_name) {
  GtkStyleContext *context = gtk_widget_get_style_context(wb.widget);
  gtk_style_context_add_class(context, class_name);
  return wb;
}

static inline WidgetBuilder wb_connect(WidgetBuilder wb, const char *signal,
                                       GCallback callback, gpointer data) {
  g_signal_connect(wb.widget, signal, callback, data);
  return wb;
}

static inline GtkWidget *wb_build(WidgetBuilder wb) { return wb.widget; }

// ===========================
// 5. COMMON DIALOGS
// ===========================

// Simple alert dialog
static inline void dialog_alert(GtkWindow *parent, const char *title,
                                const char *message) {
  GtkWidget *dialog = gtk_message_dialog_new(
      parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

// Confirmation dialog
static inline gboolean dialog_confirm(GtkWindow *parent, const char *title,
                                      const char *message) {
  GtkWidget *dialog = gtk_message_dialog_new(
      parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  return (result == GTK_RESPONSE_YES);
}

// Error dialog
static inline void dialog_error(GtkWindow *parent, const char *title,
                                const char *message) {
  GtkWidget *dialog = gtk_message_dialog_new(
      parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

// Input dialog (returns NULL if cancelled, must be freed by caller)
static inline char *dialog_input(GtkWindow *parent, const char *title,
                                 const char *prompt, const char *default_text) {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      title, parent, GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      "_Cancel", GTK_RESPONSE_CANCEL, "_OK", GTK_RESPONSE_OK, NULL);

  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *label = gtk_label_new(prompt);
  GtkWidget *entry = gtk_entry_new();

  if (default_text) {
    gtk_entry_set_text(GTK_ENTRY(entry), default_text);
  }

  gtk_box_pack_start(GTK_BOX(content), label, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(content), entry, FALSE, FALSE, 5);
  gtk_widget_show_all(content);

  gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
  gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  char *text = NULL;

  if (result == GTK_RESPONSE_OK) {
    text = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  }

  gtk_widget_destroy(dialog);
  return text;
}

// Custom modal dialog builder
typedef struct {
  GtkWidget *dialog;
  GtkWidget *content_area;
} DialogBuilder;

static inline DialogBuilder dialog_new(GtkWindow *parent, const char *title) {
  GtkWidget *dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
  gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

  DialogBuilder db = {.dialog = dialog,
                      .content_area =
                          gtk_dialog_get_content_area(GTK_DIALOG(dialog))};

  return db;
}

static inline DialogBuilder
dialog_add_button(DialogBuilder db, const char *label, gint response_id) {
  gtk_dialog_add_button(GTK_DIALOG(db.dialog), label, response_id);
  return db;
}

static inline DialogBuilder dialog_set_size(DialogBuilder db, int width,
                                            int height) {
  gtk_window_set_default_size(GTK_WINDOW(db.dialog), width, height);
  return db;
}

static inline DialogBuilder dialog_add_content(DialogBuilder db,
                                               GtkWidget *widget) {
  gtk_box_pack_start(GTK_BOX(db.content_area), widget, TRUE, TRUE, 0);
  return db;
}

static inline gint dialog_run_and_destroy(DialogBuilder db) {
  gtk_widget_show_all(db.dialog);
  gint result = gtk_dialog_run(GTK_DIALOG(db.dialog));
  gtk_widget_destroy(db.dialog);
  return result;
}

// ===========================
// 6. STYLE & CSS HELPERS
// ===========================

static inline void style_load_css(const char *css_data) {
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider, css_data, -1, NULL);
  gtk_style_context_add_provider_for_screen(
      gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_object_unref(provider);
}

static inline void style_add_class(GtkWidget *widget, const char *class_name) {
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_class(context, class_name);
}

static inline void style_remove_class(GtkWidget *widget,
                                      const char *class_name) {
  GtkStyleContext *context = gtk_widget_get_style_context(widget);
  gtk_style_context_remove_class(context, class_name);
}

// ===========================
// 7. TIMER & IDLE CALLBACKS
// ===========================

typedef struct {
  guint id;
  GSourceFunc callback;
  gpointer user_data;
} TimerHandle;

static inline TimerHandle *timer_add(guint interval_ms, GSourceFunc callback,
                                     gpointer user_data) {
  TimerHandle *handle = g_malloc(sizeof(TimerHandle));
  handle->callback = callback;
  handle->user_data = user_data;
  handle->id = g_timeout_add(interval_ms, callback, user_data);
  return handle;
}

static inline void timer_remove(TimerHandle *handle) {
  if (handle) {
    g_source_remove(handle->id);
    g_free(handle);
  }
}

static inline guint idle_add(GSourceFunc callback, gpointer user_data) {
  return g_idle_add(callback, user_data);
}

// ===========================
// 8. GTK APP WRAPPER
// ===========================

typedef struct GtkApp {
  GtkApplication *app;
  const char *app_id;
  const char *title;
  int width;
  int height;
  GtkWidget *window;
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

// ===========================
// 9. UTILITY HELPERS
// ===========================

// Run on main thread (thread-safe)
static inline void main_thread_invoke(GSourceFunc func, gpointer data) {
  g_idle_add(func, data);
}

// Get text from entry
static inline const char *entry_get_text(GtkWidget *entry) {
  return gtk_entry_get_text(GTK_ENTRY(entry));
}

// Set text to entry
static inline void entry_set_text(GtkWidget *entry, const char *text) {
  gtk_entry_set_text(GTK_ENTRY(entry), text);
}

// Get text from text view
static inline char *text_view_get_text(GtkWidget *text_view) {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(buffer, &start, &end);
  return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

// Set text to text view
static inline void text_view_set_text(GtkWidget *text_view, const char *text) {
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  gtk_text_buffer_set_text(buffer, text, -1);
}

#endif // GTK_UTIL_H
