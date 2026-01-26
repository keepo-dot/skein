#include "app_state.h"
#include <gtk/gtk.h>

#ifndef ON_DRAG_BEGIN
#define ON_DRAG_BEGIN

#endif /* ifndef ON_DRAG_BEGIN                                                 \
#define ON_DRAG_BEGIN                                                          \
 */

void on_drag_begin(GtkGestureDrag *gesture, double start_x, double start_y,
                   AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

  if (toolbar_state && toolbar_state->active_mode == MODE_MOVE) {
    grid_data->drag_start_x = grid_data->camera_x;
    grid_data->drag_start_y = grid_data->camera_y;
  } else if (toolbar_state && toolbar_state->active_mode == MODE_PAINT) {
    grid_data->mouse_start_x = start_x;
    grid_data->mouse_start_y = start_y;

    int column = (int)((start_x + grid_data->camera_x) / STITCH_SIZE);
    int row = (int)((start_y + grid_data->camera_y) / STITCH_SIZE);

    if ((column >= 0 && column < grid_data->width) &&
        (row >= 0 && row < grid_data->height)) {
      int index = (row * grid_data->width) + column;
      grid_data->stitch_data[index].stitch_color = toolbar_state->active_color;
      gtk_widget_queue_draw(area);
    }
  } else if (toolbar_state && toolbar_state->active_mode == MODE_STITCH) {
    grid_data->mouse_start_x = start_x;
    grid_data->mouse_start_y = start_y;

    int column = (int)((start_x + grid_data->camera_x) / STITCH_SIZE);
    int row = (int)((start_y + grid_data->camera_y) / STITCH_SIZE);

    if ((column >= 0 && column < grid_data->width) &&
        (row >= 0 && row < grid_data->height)) {
      int index = (row * grid_data->width) + column;
      grid_data->stitch_data[index].stitch_type = toolbar_state->active_stitch;
      gtk_widget_queue_draw(area);
    }
  }
}
