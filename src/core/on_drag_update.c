#include "app_state.h"
#include <gtk/gtk.h>

void on_drag_update(GtkGestureDrag *gesture, double offset_x, double offset_y,
                    AppState *app_state) {
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
  } else if (toolbar_state->active_mode == MODE_PAINT) {
    double current_mouse_x = grid_data->mouse_start_x + offset_x;
    double current_mouse_y = grid_data->mouse_start_y + offset_y;
    int column = (int)((current_mouse_x + grid_data->camera_x) / STITCH_SIZE);
    int row = (int)((current_mouse_y + grid_data->camera_y) / STITCH_SIZE);

    if ((column >= 0 && column < grid_data->width) &&
        (row >= 0 && row < grid_data->height)) {
      int index = (row * grid_data->width) + column;
      grid_data->stitch_data[index].stitch_color = toolbar_state->active_color;
      gtk_widget_queue_draw(area);
    }
  } else if (toolbar_state->active_mode == MODE_STITCH) {
    double current_mouse_x = grid_data->mouse_start_x + offset_x;
    double current_mouse_y = grid_data->mouse_start_y + offset_y;
    int column = (int)((current_mouse_x + grid_data->camera_x) / STITCH_SIZE);
    int row = (int)((current_mouse_y + grid_data->camera_y) / STITCH_SIZE);

    if ((column >= 0 && column < grid_data->width) &&
        (row >= 0 && row < grid_data->height)) {
      int index = (row * grid_data->width) + column;
      grid_data->stitch_data[index].stitch_type = toolbar_state->active_stitch;
      gtk_widget_queue_draw(area);
    }
  }
}
