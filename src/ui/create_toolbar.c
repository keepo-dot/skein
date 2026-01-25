#include "button.h"
#include "colors.h"
#include "create_button.h"
#include "glib-object.h"
#include "glibconfig.h"
#include "gtk/gtkshortcut.h"
#include "on_color_toggled.h"
#include "on_tool_toggled.h"
#include "toolbar_state.h"
#include <gtk/gtk.h>
#include <stdalign.h>

GtkWidget *create_toolbar(ToolbarState *state) {
  GtkWidget *toolbar;
  toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  ButtonInfo toolbar_buttons_mode[] = {
      {"Move", "object-move", NULL, MODE_MOVE},
      {"Paint Stitch", "draw-brush-symbolic", NULL, MODE_PAINT},
      {"Erase Stitch", "draw-eraser-symbolic", NULL, MODE_ERASE},
      {"Color Picker", "color-picker-symbolic", NULL, MODE_PICKER}};

  ButtonInfo toolbar_buttons_color[] = {
      {"Red", NULL, NULL, MODE_COLOR, &COLOR_RED},
      {"Orange", NULL, NULL, MODE_COLOR, &COLOR_ORANGE},
      {"Yellow", NULL, NULL, MODE_COLOR, &COLOR_YELLOW},
      {"Green", NULL, NULL, MODE_COLOR, &COLOR_GREEN},
      {"Blue", NULL, NULL, MODE_COLOR, &COLOR_BLUE},
      {"Indigo", NULL, NULL, MODE_COLOR, &COLOR_INDIGO},
      {"Violet", NULL, NULL, MODE_COLOR, &COLOR_VIOLET}};

  GtkWidget *leader_button_mode = NULL;
  // Create tool buttons.
  for (int i = 0; i < G_N_ELEMENTS(toolbar_buttons_mode); i++) {
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
      g_print("Error: mode_button %d is NULL\n", i);
    }
    // Attach the specific tool mode (Move, Paint, etc.) to the button
    // so the callback knows which mode this button represents.
    g_object_set_data(G_OBJECT(button), "tool-type",
                      GINT_TO_POINTER(toolbar_buttons_mode[i].button_type));
    // Add callback function on button toggle.
    g_signal_connect(button, "toggled", G_CALLBACK(on_tool_toggled), state);
    state->tool_buttons[toolbar_buttons_mode[i].button_type] = button;
    // g_print("DEBUG: Button %d saved to state\n", i);
    gtk_box_append(GTK_BOX(toolbar), button);
  }

  gtk_box_append(GTK_BOX(toolbar), gtk_separator_new(GTK_ORIENTATION_VERTICAL));

  GtkWidget *leader_button_color = NULL;
  // Creates color buttons.
  for (int i = 0; i < G_N_ELEMENTS(toolbar_buttons_color); i++) {
    GtkWidget *button = create_button(&toolbar_buttons_color[i]);
    if (i == 0) {
      leader_button_color = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button_color));
    }

    if (button == NULL) {
      g_print("Error: mode_button %d is NULL\n", i);
    }
    // Attach the specific tool mode (Move, Paint, etc.) to the button
    // so the callback knows which mode this button represents.
    g_object_set_data(G_OBJECT(button), "button-color",
                      (gpointer)toolbar_buttons_color[i].button_color);
    // Add callback function on button toggle.
    g_signal_connect(button, "toggled", G_CALLBACK(on_color_toggled), state);
    // state->color_buttons[toolbar_buttons_color[i].button_color] = button;
    // g_print("DEBUG: Button %d saved to state\n", i);
    gtk_box_append(GTK_BOX(toolbar), button);
  }

  // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(leader_button_color), TRUE);
  return toolbar;
}
