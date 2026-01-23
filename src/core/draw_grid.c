#include "gdk/gdk.h"
#include "pattern_data.h"
#include <gtk/gtk.h>
void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
               gpointer user_data) {
  PatternData *grid = user_data;

  GdkRGBA fg_color;

  gtk_widget_get_color(GTK_WIDGET(area), &fg_color);

  gdk_cairo_set_source_rgba(cr, &fg_color);

  for (int i = 0; i < grid->height; i++) {
    for (int j = 0; j < grid->width; j++) {
      double pixel_x = j * STITCH_SIZE; // width
      double pixel_y = i * STITCH_SIZE; // height

      cairo_rectangle(cr, pixel_x, pixel_y, STITCH_SIZE,
                      STITCH_SIZE); // rect to draw
      cairo_stroke(cr);             // draw the thing.
    }
  }
}
