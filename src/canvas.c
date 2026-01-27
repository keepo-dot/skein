#include "types.h"
#include "utils.h"
#include <gtk/gtk.h>

// checks if grid needs redrawn, if so, calls a redraw.
static gboolean needs_redraw(GtkWidget *widget, GdkFrameClock *frame_clock,
                             gpointer grid_data) {
  PatternData *grid = (PatternData *)grid_data;

  if (grid->redraw == true) {
    gtk_widget_queue_draw(widget);
    grid->redraw = false; // reset draw flag to avoid runaway.
  }

  return G_SOURCE_CONTINUE; // tells gtk to run the function again.
}

// handles drag events like painting strokes and moving.
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
  } else if (toolbar_state->active_mode == MODE_PAINT) {
    double current_mouse_x = grid_data->mouse_start_x + offset_x;
    double current_mouse_y = grid_data->mouse_start_y + offset_y;
    int column = (int)((current_mouse_x + grid_data->camera_x) / STITCH_SIZE);
    int row = (int)((current_mouse_y + grid_data->camera_y) / STITCH_SIZE);

    if ((column >= 0 && column < grid_data->width) &&
        (row >= 0 && row < grid_data->height)) {
      int index = (row * grid_data->width) + column;
      GdkRGBA initial_color = toolbar_state->active_color;
      if (initial_color.alpha != 0) {
        grid_data->stitch_data[index].stitch_color =
            toolbar_state->active_color;
        gtk_widget_queue_draw(area);
      }
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

// handles single click events like drawing a color,or drawing a stitch.
static void on_drag_begin(GtkGestureDrag *gesture, double start_x,
                          double start_y, AppState *app_state) {
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
      GdkRGBA initial_color = toolbar_state->active_color;
      if (initial_color.alpha != 0) {
        grid_data->stitch_data[index].stitch_color =
            toolbar_state->active_color;
        gtk_widget_queue_draw(area);
      }
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

// draws the grid lines and fills the square colors and handles grid
// transaltion. uses cairo.
static void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                      gpointer user_data) {
  AppState *app_state = (AppState *)user_data;
  PatternData *grid = app_state->pattern;

  GdkRGBA fg_color;

  gtk_widget_get_color(GTK_WIDGET(area), &fg_color);

  double clip_x1, clip_y1, clip_x2, clip_y2;
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2,
                     &clip_y2); // grab the visual bounds.visual

  int start_column = (int)((clip_x1 + grid->camera_x) / STITCH_SIZE);
  int end_column = (int)((clip_x2 + grid->camera_x) / STITCH_SIZE) + 1;
  int start_row = (int)((clip_y1 + grid->camera_y) / STITCH_SIZE);
  int end_row = (int)((clip_y2 + grid->camera_y) / STITCH_SIZE) + 1;

  start_column = MAX(0, start_column);
  start_row = MAX(0, start_row);
  end_column = MIN(grid->width, end_column);
  end_row = MIN(grid->height, end_row);

  for (int i = start_row; i < end_row; i++) {
    for (int j = start_column; j < end_column; j++) {
      double pixel_x = (j * STITCH_SIZE) - grid->camera_x; // width
      double pixel_y = (i * STITCH_SIZE) - grid->camera_y; // height
      int index = (i * grid->width) + j;

      gdk_cairo_set_source_rgba(cr, &grid->stitch_data[index].stitch_color);

      cairo_rectangle(cr, pixel_x, pixel_y, STITCH_SIZE,
                      STITCH_SIZE); // rect to draw
      cairo_fill_preserve(cr);
      gdk_cairo_set_source_rgba(cr, &COLOR_BLACK);
      cairo_stroke(cr);

      if (grid->stitch_data[index].stitch_type) {

        GdkRGBA *bg = &grid->stitch_data[index].stitch_color;

        // If the stitch is transparent (unpainted), assume the background is
        // White
        double red = (bg->alpha < 0.5) ? 1.0 : bg->red;
        double green = (bg->alpha < 0.5) ? 1.0 : bg->green;
        double blue = (bg->alpha < 0.5) ? 1.0 : bg->blue;

        double brightness = (red * 0.299) + (green * 0.587) + (blue * 0.114);

        if (brightness > 0.5) {
          gdk_cairo_set_source_rgba(
              cr, &COLOR_BLACK); // Light background -> Black lines
        } else {
          gdk_cairo_set_source_rgba(
              cr, &COLOR_WHITE); // Dark background -> White lines
        }

        cairo_save(cr);
        cairo_translate(cr, pixel_x, pixel_y);
        draw_stitch_swatch(
            area, cr, STITCH_SIZE, STITCH_SIZE,
            GINT_TO_POINTER(grid->stitch_data[index].stitch_type));
        cairo_restore(cr);
      }
    }
  }
}

// general GTK Widget setup. creates a new pattern view and connects click
// callbacks for movement updates.
GtkWidget *create_pattern_view(AppState *app_state) {
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
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_grid, app_state,
                                 NULL);

  return area;
}
