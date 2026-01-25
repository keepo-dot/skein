#include "button.h"
#include "gdk/gdk.h"
#include <gtk/gtk.h>
void draw_color_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                       gpointer button_color) {
  GdkRGBA *paint_color = (GdkRGBA *)button_color;
  gdk_cairo_set_source_rgba(cr, paint_color);
  cairo_paint(cr);
}
