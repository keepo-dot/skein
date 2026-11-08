#include "canvas_events.h"
#include "resources.h"
#include "ui_types.h"
// checks if grid needs redrawn, if so, calls a redraw.
static gboolean needs_redraw(GtkWidget *widget, GdkFrameClock *frame_clock,
                             gpointer ui_p) {
  (void)frame_clock;
  UiState *ui = (UiState *)ui_p;
  if (ui->redraw == true) {
    gtk_widget_queue_draw(widget);
    ui->redraw = false; // reset draw flag to avoid runaway.
  }

  return G_SOURCE_CONTINUE; // tells gtk to run the function again.
}

void draw_repeat_outlines(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                          gpointer user_data) {
  (void)area;
  (void)w;
  (void)h;
  AppState *app_state = (AppState *)user_data;
  RepeatTable *repeat_table = app_state->pattern->repeat_table;

  if (repeat_table->num_repeats == 0)
    return;
  // draw repeat outline
  for (size_t i = 0; i < repeat_table->num_repeats; i++) {
    if (i == app_state->ui->hovered_repeat_index) {
      cairo_set_line_width(cr, 4.0);
      GdkRGBA hover_color = {0.85, 0.55, 0.20, 1.0};
      gdk_cairo_set_source_rgba(cr, &hover_color);
    } else {
      cairo_set_line_width(cr, 3.0);
      GdkRGBA repeat_color = {0.96, 0.84, 0.15, 1.0};
      gdk_cairo_set_source_rgba(cr, &repeat_color);
    }
    double stitch_size = app_state->pattern->stitch_size;
    int s_row = repeat_table->repeat_section[i].start_row;
    int e_row = repeat_table->repeat_section[i].end_row;
    int top_row = (s_row < e_row) ? s_row : e_row;
    int bottom_row = (s_row < e_row) ? e_row : s_row;
    int num_rows = (bottom_row - top_row) + 1;
    double x_pos = 0.0 - app_state->ui->camera_x;
    double y_pos = (top_row * stitch_size) - app_state->ui->camera_y;
    double rect_w = stitch_size * app_state->pattern->width;
    double rect_h = stitch_size * num_rows;
    cairo_rectangle(cr, x_pos, y_pos, rect_w, rect_h);
    cairo_stroke(cr);
    // draw icon in top right corner.
    double r_edge =
        (app_state->pattern->width * app_state->pattern->stitch_size) -
        app_state->ui->camera_x;
    double icon_w = r_edge - gdk_pixbuf_get_width(app_state->ui->repeat_icon);
    G_GNUC_BEGIN_IGNORE_DEPRECATIONS
    gdk_cairo_set_source_pixbuf(cr, GDK_PIXBUF(app_state->ui->repeat_icon),
                                icon_w, y_pos);
    G_GNUC_END_IGNORE_DEPRECATIONS
    cairo_paint(cr);
  }
}

void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                       gpointer button_color) {
  (void)area;
  (void)w;
  (void)h;
  GdkRGBA *paint_color = (GdkRGBA *)button_color;
  gdk_cairo_set_source_rgba(cr, paint_color);
  cairo_paint(cr);
}

void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data) {
  (void)area;
  (void)w;
  (void)h;
  int stitch_type = GPOINTER_TO_INT(user_data);

  cairo_set_line_width(cr, 2.0);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

  double center_x = w / 2.0;
  double center_y = h / 2.0;
  double size = w * 0.6; // Scale relative to button size

  switch (stitch_type) {
  case STITCH_KNIT: // No line, knit is default.
    break;

  case STITCH_PURL: // Horizontal Dash (or Dot)
    cairo_move_to(cr, center_x - (size / 2), center_y);
    cairo_line_to(cr, center_x + (size / 2), center_y);
    cairo_stroke(cr);
    break;

  case STITCH_M1: // New: A twisted loop "e"
    // Top loop
    cairo_arc(cr, center_x, center_y - (size / 4), size / 4, 0, 2 * G_PI);
    // Legs crossing at bottom
    cairo_move_to(cr, center_x - (size / 4), center_y + (size / 2));
    cairo_line_to(cr, center_x + (size / 4), center_y); // cross up
    cairo_stroke(cr);
    break;

  case STITCH_SLIP: // New: A "V" shape
    cairo_move_to(cr, center_x - (size / 3), center_y - (size / 3));
    cairo_line_to(cr, center_x, center_y + (size / 3)); // Down to point
    cairo_line_to(cr, center_x + (size / 3),
                  center_y - (size / 3)); // Up to right
    cairo_stroke(cr);
    break;

  case STITCH_YO: // Circle
    cairo_arc(cr, center_x, center_y, size / 3, 0, 2 * G_PI);
    cairo_stroke(cr);
    break;

  case STITCH_K2TOG: // Right Lean /
    cairo_move_to(cr, center_x + (size / 2),
                  center_y - (size / 2)); // Top Right
    cairo_line_to(cr, center_x - (size / 2), center_y + (size / 2)); // Bot Left
    cairo_stroke(cr);
    break;

  case STITCH_SSK: // Left Lean \ (Backslash)
    cairo_move_to(cr, center_x - (size / 2), center_y - (size / 2)); // Top Left
    cairo_line_to(cr, center_x + (size / 2),
                  center_y + (size / 2)); // Bot Right
    cairo_stroke(cr);
    break;
  }
}

// draws the grid lines and fills the square colors and handles grid
// transaltion. uses cairo.
static void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                      gpointer user_data) {
  AppState *app_state = (AppState *)user_data;
  PatternData *grid = app_state->pattern;
  UiState *ui = app_state->ui;

  GdkRGBA fg_color;

  gtk_widget_get_color(GTK_WIDGET(area), &fg_color);

  double clip_x1, clip_y1, clip_x2, clip_y2;
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2,
                     &clip_y2); // grab the visual bounds.visual

  int start_column =
      (int)((clip_x1 + ui->camera_x) / app_state->pattern->stitch_size);
  int end_column =
      (int)((clip_x2 + ui->camera_x) / app_state->pattern->stitch_size) + 1;
  int start_row =
      (int)((clip_y1 + ui->camera_y) / app_state->pattern->stitch_size);
  int end_row =
      (int)((clip_y2 + ui->camera_y) / app_state->pattern->stitch_size) + 1;

  start_column = MAX(0, start_column);
  start_row = MAX(0, start_row);
  end_column = MIN(grid->width, end_column);
  end_row = MIN(grid->height, end_row);
  for (int i = start_row; i < end_row; i++) {
    for (int j = start_column; j < end_column; j++) {
      double pixel_x =
          (j * app_state->pattern->stitch_size) - ui->camera_x; // width
      double pixel_y =
          (i * app_state->pattern->stitch_size) - ui->camera_y; // height
      int index = (i * grid->width) + j;
      GdkRGBA cell_color = TO_GDK_RGBA(grid->stitch_data[index].stitch_color);
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

        GdkRGBA *bg = &TO_GDK_RGBA(grid->stitch_data[index].stitch_color);

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
  draw_repeat_outlines(area, cr, width, height, app_state);
}

// general GTK Widget setup. creates a new pattern view and connects click
// callbacks for movement updates.
GtkWidget *create_pattern_view(AppState *app_state) {
  UiState *ui = app_state->ui;
  GtkWidget *area;
  area = gtk_drawing_area_new();
  gtk_widget_set_hexpand(area, true);
  gtk_widget_set_vexpand(area, true);
  ui->redraw = true;
  gtk_widget_add_tick_callback(
      area, needs_redraw, app_state->ui,
      NULL); // adds callback to check if the pattern has changed.
             // makes sure the fps doesn't go zoooooom.
  GtkGesture *mouse_drag_event = gtk_gesture_drag_new();
  GtkEventController *mouse_scroll_event =
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
  GtkEventController *mouse_motion_event = gtk_event_controller_motion_new();
  g_signal_connect(mouse_drag_event, "drag-begin", G_CALLBACK(on_drag_begin),
                   app_state);
  g_signal_connect(mouse_drag_event, "drag-update", G_CALLBACK(on_drag_update),
                   app_state);
  g_signal_connect(mouse_scroll_event, "scroll", G_CALLBACK(on_scroll),
                   app_state);
  g_signal_connect(mouse_drag_event, "drag-end", G_CALLBACK(on_drag_end),
                   app_state);
  g_signal_connect(mouse_motion_event, "motion", G_CALLBACK(on_motion),
                   app_state);
  g_signal_connect(area, "query-tooltip", G_CALLBACK(on_repeat_hover),
                   app_state);
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_drag_event));
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_scroll_event));
  gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(mouse_motion_event));
  gtk_widget_set_has_tooltip(area, TRUE);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_grid, app_state,
                                 NULL);

  return area;
}
