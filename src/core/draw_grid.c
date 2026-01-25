#include "app_state.h"
#include "cairo.h"
#include "glib.h"
#include "pattern_data.h"
#include <gtk/gtk.h>
void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
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

  if (grid->redraw) {
    g_print("Drawing Viewport: Cols %d-%d, Rows %d-%d\n", start_column,
            end_column, start_row, end_row);
  }

  for (int i = start_row; i < end_row; i++) {
    for (int j = start_column; j < end_column; j++) {
      double pixel_x = (j * STITCH_SIZE) - grid->camera_x; // width
      double pixel_y = (i * STITCH_SIZE) - grid->camera_y; // height
      int index = (i * grid->width) + j;

      gdk_cairo_set_source_rgba(cr, &grid->stitch_data[index].stitch_color);

      cairo_rectangle(cr, pixel_x, pixel_y, STITCH_SIZE,
                      STITCH_SIZE); // rect to draw
      cairo_fill_preserve(cr);
      gdk_cairo_set_source_rgba(cr, &fg_color);
      cairo_stroke(cr);
    }
  }
}
