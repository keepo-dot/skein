#include "button.h"
#include "create_button.h"
#include "glib-object.h"
#include "glibconfig.h"
#include "gtk/gtkshortcut.h"
#include "on_tool_toggled.h"
#include "toolbar_state.h"
#include <gtk/gtk.h>
#include <stdalign.h>

GtkWidget *create_toolbar(ToolbarState *state) {
  GtkWidget *toolbar;
  toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

  ButtonInfo toolbar_buttons[] = {
      {"Move", "object-move", NULL, MODE_MOVE},
      {"Paint Stitch", "draw-brush-symbolic", NULL, MODE_PAINT},
      {"Erase Stitch", "draw-eraser-symbolic", NULL, MODE_ERASE},
      {"Color Picker", "color-picker-symbolic", NULL, MODE_PICKER}};

  GtkWidget *leader_button = NULL;

  for (int i = 0; i < NUM_TOOLS; i++) {
    // g_print("DEBUG: Creating button %d (Type: %d)\n", i,
    //         toolbar_buttons[i].button_type);

    GtkWidget *button = create_button(&toolbar_buttons[i]);
    if (i == 0) {
      leader_button = button;
    } else {
      gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(button),
                                  GTK_TOGGLE_BUTTON(leader_button));
    }

    if (button == NULL) {
      g_print("Error: button %d is NULL\n", i);
    }
    // Attach the specific tool mode (Move, Paint, etc.) to the button
    // so the callback knows which mode this button represents.
    g_object_set_data(G_OBJECT(button), "tool-type",
                      GINT_TO_POINTER(toolbar_buttons[i].button_type));
    // Add callback function on button toggle.
    g_signal_connect(button, "toggled", G_CALLBACK(on_tool_toggled), state);
    state->tool_buttons[toolbar_buttons[i].button_type] = button;
    // g_print("DEBUG: Button %d saved to state\n", i);
    gtk_box_append(GTK_BOX(toolbar), button);
  }
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(leader_button), TRUE);
  return toolbar;
}
