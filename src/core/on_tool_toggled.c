#include "toolbar_state.h"
#include <gtk/gtk.h>

void on_tool_toggled(GtkToggleButton *button, gpointer toolbar_state) {
  ToolbarState *state = (ToolbarState *)toolbar_state;
  if (gtk_toggle_button_get_active(button)) {
    ToolbarMode mode =
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "tool-type"));
    state->active_mode = mode;
    /* DEBUG
    g_print("Current Mode: %d\n", state->active_mode);
    */
  }
}
