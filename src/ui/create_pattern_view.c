#include "create_pattern_view.h"
#include "activate.h"
#include "app_state.h"
#include "glib-object.h"
#include "on_drag_begin.h"
#include "on_drag_update.h"
#include "pattern_data.h"
#include "toolbar_state.h"
#include <gtk/gtk.h>

static gboolean needs_redraw(GtkWidget *widget, GdkFrameClock *frame_clock,
                             gpointer grid_data) {
  PatternData *grid = (PatternData *)grid_data;

  if (grid->redraw == true) {
    gtk_widget_queue_draw(widget);
    grid->redraw = false; // reset draw flag to avoid runaway.
  }

  return G_SOURCE_CONTINUE; // tells gtk to run the function again.
}

// create window etc.
GtkWidget *create_pattern_view(AppState *app_state,
                               GtkDrawingAreaDrawFunc draw_func) {
  GtkWidget *area;
  area = gtk_drawing_area_new();
  gtk_widget_set_hexpand(area, true);
  gtk_widget_set_vexpand(area, true);
  app_state->pattern->redraw = true;
  gtk_widget_add_tick_callback(
      area, needs_redraw, app_state->pattern,
      NULL); // adds callback to check if the pattern has changed.
             // makes sure the fps doesn't go zoooooom.
  GtkGesture *mouse_drag_event = gtk_gesture_drag_new();
  g_signal_connect(mouse_drag_event, "drag-begin", G_CALLBACK(on_drag_begin),
                   app_state);
  g_signal_connect(mouse_drag_event, "drag-update", G_CALLBACK(on_drag_update),
                   app_state);
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_drag_event));
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_func, app_state,
                                 NULL);

  return area;
}
