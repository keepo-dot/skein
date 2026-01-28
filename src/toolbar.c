#include "glib-object.h"
#include "glib.h"
#include "glibconfig.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <stddef.h>
#include <stdio.h>

void pattern_reset_size(PatternData *pattern, int new_width, int new_height) {
  free(pattern->stitch_data);
  pattern->stitch_data =
      calloc((size_t)(new_width * new_height), sizeof(StitchData));
  pattern->width = new_width;
  pattern->height = new_height;
}

void on_new_pattern_confirm(GtkWidget *button, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  GtkWidget *w_spin = g_object_get_data(G_OBJECT(button), "w-spin");
  GtkWidget *h_spin = g_object_get_data(G_OBJECT(button), "h-spin");
  GtkWidget *dialog_window =
      g_object_get_data(G_OBJECT(button), "dialog-window");
  int new_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w_spin));
  int new_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(h_spin));
  pattern_reset_size(state->pattern, new_width, new_height);
  state->pattern->redraw = true;
  gtk_window_destroy(GTK_WINDOW(dialog_window));
}

void show_new_pattern_dialog(GtkWidget *main_window, AppState *app_state) {
  GtkWidget *new_pattern_dialog = gtk_window_new();
  gtk_window_set_modal(GTK_WINDOW(new_pattern_dialog), true);
  gtk_window_set_transient_for(GTK_WINDOW(new_pattern_dialog),
                               GTK_WINDOW(main_window));
  GtkWidget *input_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(new_pattern_dialog), input_container);

  GtkWidget *width_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *width_label = gtk_label_new("Width:");
  GtkWidget *width_spinner = gtk_spin_button_new_with_range(1, 1000, 1);
  gtk_box_append(GTK_BOX(width_container), width_label);
  gtk_box_append(GTK_BOX(width_container), width_spinner);

  GtkWidget *height_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *height_label = gtk_label_new("Height:");
  GtkWidget *height_spinner = gtk_spin_button_new_with_range(1, 1000, 1);
  gtk_box_append(GTK_BOX(height_container), height_label);
  gtk_box_append(GTK_BOX(height_container), height_spinner);

  GtkWidget *button_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");
  GtkWidget *create_button = gtk_button_new_with_label("Create");
  gtk_box_append(GTK_BOX(button_container), cancel_button);
  gtk_box_append(GTK_BOX(button_container), create_button);

  gtk_box_append(GTK_BOX(input_container), width_container);
  gtk_box_append(GTK_BOX(input_container), height_container);
  gtk_box_append(GTK_BOX(input_container), button_container);
  g_signal_connect_swapped(cancel_button, "clicked",
                           G_CALLBACK(gtk_window_destroy), new_pattern_dialog);
  g_object_set_data(G_OBJECT(create_button), "w-spin", width_spinner);
  g_object_set_data(G_OBJECT(create_button), "h-spin", height_spinner);
  g_object_set_data(G_OBJECT(create_button), "dialog-window",
                    new_pattern_dialog);
  g_signal_connect(create_button, "clicked", G_CALLBACK(on_new_pattern_confirm),
                   app_state);
  gtk_window_present(GTK_WINDOW(new_pattern_dialog));
}

void on_action_clicked(GtkButton *button, gpointer app_state) {
  AppState *state = (AppState *)app_state;
  GtkWidget *main_window = GTK_WIDGET(gtk_widget_get_root(GTK_WIDGET(button)));
  int action =
      GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "action-type"));
  switch (action) {
  case MODE_NEWFILE:
    show_new_pattern_dialog(main_window, state);
    break;
  case MODE_SAVEFILE:
    break;
  case MODE_LOADFILE:
    break;
  }
}

void on_stitch_toggled(GtkToggleButton *button, gpointer toolbar_state) {
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
      "action-type", FILEIO_BUTTON_COUNT, true, false, 1);

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
  return toolbar;
}
