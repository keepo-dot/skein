#include "cairo.h"
#include "pattern_data.h"
#include <gtk/gtk.h>
void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
               gpointer user_data) {
  PatternData *grid = user_data;

  GdkRGBA fg_color;

  gtk_widget_get_color(GTK_WIDGET(area), &fg_color);

  double clip_x1, clip_y1, clip_x2, clip_y2;
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2,
                     &clip_y2); // grab the visual bounds.visual

  int start_column = (int)(clip_x1 / STITCH_SIZE);
  int end_column = (int)(clip_x2 / STITCH_SIZE) + 1;
  int start_row = (int)(clip_y1 / STITCH_SIZE);
  int end_row = (int)(clip_y2 / STITCH_SIZE) + 1;

  if (grid->redraw) {
    g_print("Drawing Viewport: Cols %d-%d, Rows %d-%d\n", start_column,
            end_column, start_row, end_row);
  }

  for (int i = start_row; i < end_row; i++) {
    for (int j = start_column; j < end_column; j++) {
      double pixel_x = j * STITCH_SIZE; // width
      double pixel_y = i * STITCH_SIZE; // height
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
