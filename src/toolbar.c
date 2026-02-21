#include "file_io.h"
#include "glib-object.h"
#include "glib.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>
#include <stddef.h>
#include <stdio.h>

static void on_save_dialog_finish(GObject *file_dialog, GAsyncResult *save_file,
                                  gpointer app_state_p) {
  AppState *app_state = (AppState *)app_state_p;
  GtkWidget *main_window = app_state->main_window;
  PatternData *pattern = app_state->pattern;
  GFile *file_to_save = gtk_file_dialog_save_finish(
      GTK_FILE_DIALOG(file_dialog), save_file, NULL);
  if (file_to_save == NULL) {
    return;
  }
  char *path = g_file_get_path(file_to_save);
  if (g_str_has_suffix(path, ".skn")) {
    pattern_json_save(pattern_json_builder(pattern), path);
    g_free(path);
  } else {
    char *fixed_path = g_strdup_printf("%s.skn", path);
    pattern_json_save(pattern_json_builder(pattern), fixed_path);
    g_free(fixed_path);
  }
  GtkAlertDialog *alert = gtk_alert_dialog_new("Pattern saved successfully!");
  gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
  g_object_unref(alert);

  g_object_unref(file_to_save);
}
static void on_load_dialog_finish(GObject *file_dialog,
                                  GAsyncResult *file_to_open,
                                  gpointer app_state_p) {
  AppState *app_state = (AppState *)app_state_p;
  GtkWidget *main_window = app_state->main_window;
  GError *error = NULL;
  GFile *file_to_load = gtk_file_dialog_open_finish(
      GTK_FILE_DIALOG(file_dialog), file_to_open, NULL);
  if (file_to_load == NULL) {
    if (error && error->code != G_IO_ERROR_CANCELLED) {
      GtkAlertDialog *null_alert =
          gtk_alert_dialog_new("File load failed: %s", error->message);
      gtk_alert_dialog_show(null_alert, GTK_WINDOW(main_window));
      g_object_unref(null_alert);
    }
    g_clear_error(&error);
    return;
  }
  char *path = g_file_get_path(file_to_load);
  if (path == NULL) {
    GtkAlertDialog *null_path =
        gtk_alert_dialog_new("File load failed: filepath is null or corrupt.");
    gtk_alert_dialog_show(null_path, GTK_WINDOW(main_window));
    g_object_unref(null_path);
    return;
  }
  pattern_json_load(path, app_state->pattern);
  GtkAlertDialog *alert = gtk_alert_dialog_new("Pattern loaded successfully!");
  gtk_alert_dialog_show(alert, GTK_WINDOW(main_window));
  g_object_unref(alert);
  g_free(path);
  g_object_unref(file_to_load);
}

static void on_empty_save_response(GObject *source_object, GAsyncResult *res,
                                   gpointer user_data) {
  AppState *app_state = (AppState *)user_data;
  GtkAlertDialog *alert = GTK_ALERT_DIALOG(source_object);
  GError *error = NULL;

  int response = gtk_alert_dialog_choose_finish(alert, res, &error);

  if (error) {
    g_print("Error: %s\n", error->message);
    g_error_free(error);
    return;
  }

  if (response == 1) {
    GtkWidget *main_window = app_state->main_window;
    GtkFileDialog *save_dialog = setup_file_dialog("Save Pattern");
    gtk_file_dialog_save(save_dialog, GTK_WINDOW(main_window), NULL,
                         on_save_dialog_finish, app_state);
  }
}

static void on_action_clicked(GtkButton *button, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  GtkWidget *main_window = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(button)));
  int action =
      GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "action-type"));
  switch (action) {
  case MODE_NEWFILE:
    show_new_pattern_dialog(main_window, state);
    break;
  case MODE_SAVEFILE: {
    if (!is_pattern_empty(state->pattern)) {
      // Normal save flow if pattern has data
      GtkFileDialog *save_dialog = setup_file_dialog("Save Pattern");
      gtk_file_dialog_save(save_dialog, GTK_WINDOW(main_window), NULL,
                           on_save_dialog_finish, app_state);
    } else {
      // Warning flow if pattern is empty
      GtkAlertDialog *empty_alert = gtk_alert_dialog_new(
          "The pattern is empty. Do you still want to save?");

      // Define buttons: Cancel (0) and Save Anyway (1)
      const char *buttons[] = {"Cancel", "Save Anyway", NULL};
      gtk_alert_dialog_set_buttons(empty_alert, buttons);
      gtk_alert_dialog_set_cancel_button(empty_alert, 0);
      gtk_alert_dialog_set_default_button(empty_alert, 1);

      gtk_alert_dialog_choose(empty_alert, GTK_WINDOW(main_window), NULL,
                              on_empty_save_response, app_state);
      g_object_unref(empty_alert);
    }
    break;
  }
  case MODE_LOADFILE: {
    GtkFileDialog *load_dialog = setup_file_dialog("Load Pattern");
    gtk_file_dialog_open(load_dialog, GTK_WINDOW(main_window), NULL,
                         on_load_dialog_finish, app_state);

    break;
  }
  }
}

static void on_stitch_toggled(GtkToggleButton *button, gpointer toolbar_state) {
  ToolbarState *state = (ToolbarState *)toolbar_state;
  if (gtk_toggle_button_get_active(button)) {
    StitchType *stitch = g_object_get_data(G_OBJECT(button), "stitch-type");
    state->active_stitch = GPOINTER_TO_INT(stitch);
  }
}

static void on_color_toggled(GtkToggleButton *button, gpointer toolbar_state) {
  ToolbarState *state = (ToolbarState *)toolbar_state;
  if (gtk_toggle_button_get_active(button)) {
    GdkRGBA *color = g_object_get_data(G_OBJECT(button), "button-color");
    state->active_color = *color;
  }
}

static void on_tool_toggled(GtkToggleButton *button, gpointer toolbar_state) {
  ToolbarState *state = (ToolbarState *)toolbar_state;
  if (gtk_toggle_button_get_active(button)) {
    ToolbarMode mode =
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "tool-type"));
    state->active_mode = mode;
    switch (state->active_mode) {
    case MODE_PAINT:
      gtk_widget_set_visible(state->palette_container, true);
      gtk_widget_set_visible(state->stitch_type_container, false);
      break;

    case MODE_STITCH:
      gtk_widget_set_visible(state->stitch_type_container, true);
      gtk_widget_set_visible(state->palette_container, false);
      break;
    case MODE_ERASE:
    case MODE_PICKER:
    default:
      gtk_widget_set_visible(state->palette_container, false);
      gtk_widget_set_visible(state->stitch_type_container, false);
    }
  }
}

static GtkWidget *
action_button_group_new(ButtonInfo *btn_group, GCallback callback_func,
                        gpointer app_state, const char *data_key, size_t count,
                        bool add_separator_left, bool add_seperator_right,
                        int alignment) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  if (alignment == 1) {
    gtk_widget_set_halign(box, GTK_ALIGN_END);
  } else if (alignment == 0) {
    gtk_widget_set_halign(box, GTK_ALIGN_START);
  }
  if (add_separator_left) {
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
  }
  for (size_t i = 0; i < count; i++) {
    ButtonInfo *current_button = &btn_group[i];
    GtkWidget *button = create_button(current_button);
    g_object_set_data(G_OBJECT(button), data_key, current_button->type_ptr);
    g_signal_connect(button, "clicked", callback_func, app_state);
    gtk_box_append(GTK_BOX(box), button);
  }
  if (add_seperator_right) {
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
  }
  return box;
}

static GtkWidget *toggle_button_group_new(ButtonInfo *btn_group,
                                          GCallback callback_func,
                                          gpointer state, const char *data_key,
                                          size_t count, bool add_separator_left,
                                          bool add_seperator_right,
                                          int alignment) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  if (alignment == 1) {
    gtk_widget_set_halign(box, GTK_ALIGN_END);
  } else if (alignment == 0) {
    gtk_widget_set_halign(box, GTK_ALIGN_START);
  }
  if (add_separator_left) {
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
  }
  GtkWidget *leader_button = NULL;
  for (size_t i = 0; i < count; i++) {
    ButtonInfo *current_button = &btn_group[i];
    GtkWidget *button = create_button(current_button);
    if (button == NULL) {
      g_print("Error: %s button is NULL\n", current_button->label);

    } else if (i == 0) {
      leader_button = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button));
    }
    g_object_set_data(G_OBJECT(button), data_key, current_button->type_ptr);
    g_signal_connect(button, "toggled", callback_func, state);
    gtk_box_append(GTK_BOX(box), button);
  }
  if (add_seperator_right) {
    gtk_box_append(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
  }
  return box;
}

GtkWidget *create_toolbar(AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  // create toolbar cointainer.
  GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  // Create tool buttons.
  toolbar_state->tool_container = toggle_button_group_new(
      toolbar_buttons_mode, G_CALLBACK(on_tool_toggled), toolbar_state,
      "tool-type", MODE_BUTTON_COUNT, false, true, 0);

  // create palette buttons.
  toolbar_state->palette_container = toggle_button_group_new(
      toolbar_buttons_color, G_CALLBACK(on_color_toggled), toolbar_state,
      "button-color", COLOR_BUTTON_COUNT, true, false, 0);

  // create stitch buttons.
  toolbar_state->stitch_type_container = toggle_button_group_new(
      toolbar_buttons_stitch_type, G_CALLBACK(on_stitch_toggled), toolbar_state,
      "stitch-type", STITCH_BUTTON_COUNT, true, false, 0);
  // create file io buttons.
  toolbar_state->fileio_container = action_button_group_new(
      toolbar_buttons_fileio, G_CALLBACK(on_action_clicked), app_state,
      "action-type", FILEIO_BUTTON_COUNT, false, true, 1);
  gtk_widget_set_hexpand(toolbar_state->fileio_container, true);
  // add containers to toolbar.
  gtk_box_append(GTK_BOX(toolbar), toolbar_state->tool_container);
  gtk_box_append(GTK_BOX(toolbar), toolbar_state->palette_container);
  gtk_box_append(GTK_BOX(toolbar), toolbar_state->stitch_type_container);
  gtk_box_append(GTK_BOX(toolbar), toolbar_state->fileio_container);
  // sets the move tool to active on startup.
  GtkWidget *first_tool =
      gtk_widget_get_first_child(toolbar_state->tool_container);
  if (first_tool && GTK_IS_TOGGLE_BUTTON(first_tool)) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(first_tool), true);
  }
  // load startup screen, will put in its own place later.
  pattern_json_load("startup.skn", app_state->pattern);
  return toolbar;
}
