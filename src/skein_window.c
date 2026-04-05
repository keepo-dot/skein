#include "canvas.h"
#include "glib-object.h"
#include "gtk/gtkshortcut.h"
#include <gtk/gtk.h>

GtkWidget *create_app_container(GtkWidget *pattern_view, GtkWidget *toolbar) {
  GtkWidget *container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(container), pattern_view);
  gtk_box_append(GTK_BOX(container), toolbar);
  return container;
}

GtkWidget *create_main_window(GtkApplication *app, GtkWidget *container,
                              AppState *app_state) {
  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Skein - Knitting Pattern Maker");
  gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);
  GtkShortcutAction *cb_undo =
      gtk_callback_action_new(undo_action, app_state, NULL);
  GtkShortcutAction *cb_redo =
      gtk_callback_action_new(redo_action, app_state, NULL);
  GtkShortcutTrigger *shortcut_keybind_undo =
      gtk_shortcut_trigger_parse_string("<Control>z");
  GtkShortcutTrigger *shortcut_keybind_redo =
      gtk_shortcut_trigger_parse_string("<Control>y");
  GtkShortcut *undo_shortcut = gtk_shortcut_new(shortcut_keybind_undo, cb_undo);
  GtkShortcut *redo_shortcut = gtk_shortcut_new(shortcut_keybind_redo, cb_redo);
  GtkEventController *controller = gtk_shortcut_controller_new();
  gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(controller),
                                       undo_shortcut);
  gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(controller),
                                       redo_shortcut);

  gtk_widget_add_controller(window, controller);
  gtk_window_set_child(GTK_WINDOW(window), container);
  return window;
}
