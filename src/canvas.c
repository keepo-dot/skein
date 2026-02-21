#include "cairo.h"
#include "gdk/gdk.h"
#include "glib-object.h"
#include "resources.h"
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

static void apply_tool_to_cell(AppState *app_state, int index) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;

  switch (toolbar_state->active_mode) {
  case MODE_PAINT: {
    GdkRGBA initial_color = toolbar_state->active_color;

    if (initial_color.alpha != 0) {
      grid_data->stitch_data[index].stitch_color = toolbar_state->active_color;

      if (toolbar_state->active_stitch != STITCH_EMPTY) {
        grid_data->stitch_data[index].stitch_type =
            app_state->ui->toolbar_state->active_stitch;
      } else if (grid_data->stitch_data[index].stitch_type == STITCH_EMPTY) {
        grid_data->stitch_data[index].stitch_type = STITCH_KNIT;
      }
    }

    grid_data->redraw = true;
    break;
  }
  case MODE_STITCH:
    grid_data->stitch_data[index].stitch_type = toolbar_state->active_stitch;
    if (toolbar_state->active_color.alpha != 0.0) {
      grid_data->stitch_data[index].stitch_color = toolbar_state->active_color;
    }
    grid_data->redraw = true;
    break;
  case MODE_ERASE:
    grid_data->stitch_data[index].stitch_type = STITCH_EMPTY;
    grid_data->stitch_data[index].stitch_color = COLOR_EMPTY;
    grid_data->redraw = true;
    break;
  case MODE_PICKER:
    if (grid_data->stitch_data[index].stitch_color.alpha > 0.0) {
      toolbar_state->active_color = grid_data->stitch_data[index].stitch_color;
    }
    break;
  default:
    break;
  }
}

// handles drag events like painting strokes and moving.
static void on_drag_update(GtkGestureDrag *gesture, double offset_x,
                           double offset_y, AppState *app_state) {
  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));
  double current_mouse_x = grid_data->mouse_start_x + offset_x;
  double current_mouse_y = grid_data->mouse_start_y + offset_y;
  int column = (int)((current_mouse_x + grid_data->camera_x) /
                     app_state->pattern->stitch_size);
  int row = (int)((current_mouse_y + grid_data->camera_y) /
                  app_state->pattern->stitch_size);

  if (toolbar_state->active_mode == MODE_MOVE) {
    double new_camera_pos_x = grid_data->drag_start_x - offset_x;
    double new_camera_pos_y = grid_data->drag_start_y - offset_y;
    int width = gtk_widget_get_width(area);
    int height = gtk_widget_get_height(area);
    int max_x = (grid_data->width * grid_data->stitch_size) - width;
    int max_y = (grid_data->height * grid_data->stitch_size) - height;
    grid_data->camera_x = MAX(0, MIN(new_camera_pos_x, MAX(0, max_x)));
    grid_data->camera_y = MAX(0, MIN(new_camera_pos_y, MAX(0, max_y)));
    grid_data->redraw = true;
  } else if ((column >= 0 && column < grid_data->width) &&
             (row >= 0 && row < grid_data->height)) {
    int index = (row * grid_data->width) + column;
    apply_tool_to_cell(app_state, index);
  }
}
// handles single click events like drawing a color,or drawing a stitch.
static void on_drag_begin(GtkGestureDrag *gesture, double start_x,
                          double start_y, AppState *app_state) {

  ToolbarState *toolbar_state = app_state->ui->toolbar_state;
  PatternData *grid_data = app_state->pattern;
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture));

  grid_data->mouse_start_x = start_x;
  grid_data->mouse_start_y = start_y;

  if (toolbar_state && toolbar_state->active_mode == MODE_MOVE) {
    grid_data->drag_start_x = grid_data->camera_x;
    grid_data->drag_start_y = grid_data->camera_y;
  }

  int column =
      (int)((start_x + grid_data->camera_x) / app_state->pattern->stitch_size);
  int row =
      (int)((start_y + grid_data->camera_y) / app_state->pattern->stitch_size);
  if ((column >= 0 && column < grid_data->width) &&
      (row >= 0 && row < grid_data->height)) {
    int index = (row * grid_data->width) + column;
    apply_tool_to_cell(app_state, index);
  }
}

static gboolean on_scroll(GtkEventControllerScroll *controller, double dx,
                          double dy, AppState *app_state) {
  GdkModifierType state = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(controller));
  GtkWidget *area =
      gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(controller));
  int width = gtk_widget_get_width(area);
  int height = gtk_widget_get_height(area);
  int old_size = app_state->pattern->stitch_size;
  double center_x = (width / 2.0) + app_state->pattern->camera_x;
  double center_y = (height / 2.0) + app_state->pattern->camera_y;
  double grid_x = center_x / old_size;
  double grid_y = center_y / old_size;
  if (!(state & GDK_CONTROL_MASK)) {
    return FALSE;
  }
  app_state->pattern->stitch_size -= (dy * 5);
  if (app_state->pattern->stitch_size < 5) {
    app_state->pattern->stitch_size = 5;
  } else if (app_state->pattern->stitch_size > 100) {
    app_state->pattern->stitch_size = 100;
  }

  app_state->pattern->camera_x =
      (grid_x * app_state->pattern->stitch_size) - (width / 2.0);
  app_state->pattern->camera_y =
      (grid_y * app_state->pattern->stitch_size) - (height / 2.0);
  int max_x =
      (app_state->pattern->width * app_state->pattern->stitch_size) - width;
  int max_y =
      (app_state->pattern->height * app_state->pattern->stitch_size) - height;

  app_state->pattern->camera_x =
      MAX(0, MIN(app_state->pattern->camera_x, MAX(0, max_x)));
  app_state->pattern->camera_y =
      MAX(0, MIN(app_state->pattern->camera_y, MAX(0, max_y)));
  app_state->pattern->redraw = true;
  return TRUE;
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

  int start_column =
      (int)((clip_x1 + grid->camera_x) / app_state->pattern->stitch_size);
  int end_column =
      (int)((clip_x2 + grid->camera_x) / app_state->pattern->stitch_size) + 1;
  int start_row =
      (int)((clip_y1 + grid->camera_y) / app_state->pattern->stitch_size);
  int end_row =
      (int)((clip_y2 + grid->camera_y) / app_state->pattern->stitch_size) + 1;

  start_column = MAX(0, start_column);
  start_row = MAX(0, start_row);
  end_column = MIN(grid->width, end_column);
  end_row = MIN(grid->height, end_row);
  for (int i = start_row; i < end_row; i++) {
    for (int j = start_column; j < end_column; j++) {
      double pixel_x =
          (j * app_state->pattern->stitch_size) - grid->camera_x; // width
      double pixel_y =
          (i * app_state->pattern->stitch_size) - grid->camera_y; // height
      int index = (i * grid->width) + j;
      GdkRGBA cell_color = grid->stitch_data[index].stitch_color;
      if (grid->stitch_data[index].stitch_type != STITCH_EMPTY &&
          grid->stitch_data[index].stitch_color.alpha == 0.0) {
        cell_color = fg_color;
      }
      if (app_state->pattern->stitch_data[index].stitch_type == STITCH_EMPTY) {
        gdk_cairo_set_source_rgba(cr, &fg_color);
      } else {
        gdk_cairo_set_source_rgba(cr, &cell_color);
      }
      cairo_rectangle(cr, pixel_x, pixel_y, app_state->pattern->stitch_size,
                      app_state->pattern->stitch_size); // rect to draw
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
            area, cr, app_state->pattern->stitch_size,
            app_state->pattern->stitch_size,
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
  GtkEventController *mouse_scroll_event =
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
  g_signal_connect(mouse_drag_event, "drag-begin", G_CALLBACK(on_drag_begin),
                   app_state);
  g_signal_connect(mouse_drag_event, "drag-update", G_CALLBACK(on_drag_update),
                   app_state);
  g_signal_connect(mouse_scroll_event, "scroll", G_CALLBACK(on_scroll),
                   app_state);
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_drag_event));
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_scroll_event));
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_grid, app_state,
                                 NULL);

  return area;
}
