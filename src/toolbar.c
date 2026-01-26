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

GtkWidget *create_toolbar(ToolbarState *state) {
  GtkWidget *toolbar;
  GtkWidget *tool_container;
  GtkWidget *palette_container;
  GtkWidget *stitch_type_container;
  toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  tool_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  palette_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  stitch_type_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  ButtonInfo toolbar_buttons_mode[] = {
      {"Move", "object-move", STITCH_EMPTY, MODE_MOVE, NULL},
      {"Paint Stitch", "draw-brush-symbolic", STITCH_EMPTY, MODE_PAINT, NULL},
      {"Erase Stitch", "draw-eraser-symbolic", STITCH_EMPTY, MODE_ERASE, NULL},
      {"Color Picker", "color-picker-symbolic", STITCH_EMPTY, MODE_PICKER,
       NULL},
      {"Stitch Type", "view-grid-symbolic", STITCH_EMPTY, MODE_STITCH, NULL}};

  ButtonInfo toolbar_buttons_color[] = {
      {"Red", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_RED},
      {"Orange", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_ORANGE},
      {"Yellow", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_YELLOW},
      {"Green", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_GREEN},
      {"Blue", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_BLUE},
      {"Indigo", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_INDIGO},
      {"Violet", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_VIOLET}};

  ButtonInfo toolbar_buttons_stitch_type[] = {
      {"Knit", NULL, 1, MODE_STITCH, NULL},
      {"Purl", NULL, 2, MODE_STITCH, NULL},
      {"Yarn Over", NULL, 3, MODE_STITCH, NULL},
      {"Knit 2 Together", NULL, 4, MODE_STITCH, NULL},
      {"Slip, Slip, Knit", NULL, 5, MODE_STITCH, NULL}};

  GtkWidget *leader_button_mode = NULL;
  // Create tool buttons.
  for (size_t i = 0; i < G_N_ELEMENTS(toolbar_buttons_mode); i++) {
    // g_print("DEBUG: Creating button %d (Type: %d)\n", i,
    //         toolbar_buttons[i].button_type);

    GtkWidget *button = create_button(&toolbar_buttons_mode[i]);
    if (i == 0) {
      leader_button_mode = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button_mode));
    }

    if (button == NULL) {
      g_print("Error: mode_button %d is NULL\n", (int)i);
    }
    // Attach the specific tool mode (Move, Paint, etc.) to the button
    // so the callback knows which mode this button represents.
    g_object_set_data(G_OBJECT(button), "tool-type",
                      GINT_TO_POINTER(toolbar_buttons_mode[i].button_type));
    if (state->active_mode == toolbar_buttons_mode[i].button_type) {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), true);
    }
    // Add callback function on button toggle.
    g_signal_connect(button, "toggled", G_CALLBACK(on_tool_toggled), state);
    state->tool_buttons[toolbar_buttons_mode[i].button_type] = button;
    // g_print("DEBUG: Button %d saved to state\n", i);
    gtk_box_append(GTK_BOX(tool_container), button);
  }
  gtk_box_append(GTK_BOX(toolbar), tool_container);
  gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
  GtkWidget *leader_button_color = NULL;
  // Creates color buttons.
  for (size_t i = 0; i < G_N_ELEMENTS(toolbar_buttons_color); i++) {
    GtkWidget *button = create_button(&toolbar_buttons_color[i]);
    if (i == 0) {
      leader_button_color = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button_color));
    }

    if (button == NULL) {
      g_print("Error: color_button %d is NULL\n", (int)i);
    }
    // Attach the specific color to the button
    // so the callback knows which mode this button represents.
    g_object_set_data(G_OBJECT(button), "button-color",
                      (gpointer)toolbar_buttons_color[i].button_color);
    // Add callback function on button toggle.
    g_signal_connect(button, "toggled", G_CALLBACK(on_color_toggled), state);
    gtk_box_append(GTK_BOX(palette_container), button);
  }
  state->palette_container = palette_container;
  gtk_widget_set_visible(palette_container,
                         false); // makes sure the palette starts hidden.
  gtk_box_append(GTK_BOX(toolbar), palette_container);
  // create stitch type buttons.
  GtkWidget *leader_button_stitch = NULL;
  for (size_t i = 0; i < G_N_ELEMENTS(toolbar_buttons_stitch_type); i++) {
    GtkWidget *button = create_button(&toolbar_buttons_stitch_type[i]);
    if (i == 0) {
      leader_button_stitch = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button_stitch));
    }
    if (button == NULL) {
      g_print("Error: stitch_button %d is NULL\n", (int)i);
    }
    g_object_set_data(G_OBJECT(button), "stitch-type",
                      (gpointer)toolbar_buttons_stitch_type[i].stitch_type);
    g_signal_connect(button, "toggled", G_CALLBACK(on_stitch_toggled), state);
    gtk_box_append(GTK_BOX(stitch_type_container), button);
  }
  state->stitch_type_container = stitch_type_container;
  gtk_box_append(GTK_BOX(toolbar), stitch_type_container);
  gtk_widget_set_visible(stitch_type_container, false);
  return toolbar;
}
