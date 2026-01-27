#include "glib-object.h"
#include "glibconfig.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <stddef.h>
#include <stdio.h>

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

static GtkWidget *button_group_new(ButtonInfo *btn_group,
                                   GCallback callback_func, gpointer state,
                                   const char *data_key, size_t count,
                                   bool add_separator) {
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  if (add_separator) {
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
  return box;
}

GtkWidget *create_toolbar(ToolbarState *state) {
  // create toolbar cointainer.
  GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  // Create tool buttons.
  state->tool_container =
      button_group_new(toolbar_buttons_mode, G_CALLBACK(on_tool_toggled), state,
                       "tool-type", MODE_BUTTON_COUNT, false);

  // create palette buttons.
  state->palette_container =
      button_group_new(toolbar_buttons_color, G_CALLBACK(on_color_toggled),
                       state, "button-color", COLOR_BUTTON_COUNT, true);

  // create stitch buttons.
  state->stitch_type_container = button_group_new(
      toolbar_buttons_stitch_type, G_CALLBACK(on_stitch_toggled), state,
      "stitch-type", STITCH_BUTTON_COUNT, true);

  // add containers to toolbar.
  gtk_box_append(GTK_BOX(toolbar), state->tool_container);
  gtk_box_append(GTK_BOX(toolbar), state->palette_container);
  gtk_box_append(GTK_BOX(toolbar), state->stitch_type_container);
  gtk_widget_set_visible(state->palette_container, false);
  gtk_widget_set_visible(state->stitch_type_container, false);
  return toolbar;
}
