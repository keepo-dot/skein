#include "create_pattern_view.h"
#include "activate.h"
#include "app_state.h"
#include "glib-object.h"
#include "pattern_data.h"
#include "toolbar_state.h"
#include <gtk/gtk.h>

static void on_drag_begin(GtkGestureDrag *gesture, double start_x,
                          double start_y, AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;

  if (toolbar_state && toolbar_state->active_mode == MODE_MOVE) {
    grid_data->drag_start_x = grid_data->camera_x;
    grid_data->drag_start_y = grid_data->camera_y;
  }
}

static void on_drag_update(GtkGestureDrag *gesture, double offset_x,
                           double offset_y, AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
  if (toolbar_state->active_mode == MODE_MOVE) {
    double new_camera_pos_x = grid_data->drag_start_x - offset_x;
    double new_camera_pos_y = grid_data->drag_start_y - offset_y;
    grid_data->camera_x = new_camera_pos_x;
    grid_data->camera_y = new_camera_pos_y;
    gtk_widget_queue_draw(area);
  }
}

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
