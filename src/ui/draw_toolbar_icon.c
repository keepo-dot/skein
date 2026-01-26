#include "draw_stitch_swatch.h"
#include "gdk/gdk.h"
#include <gtk/gtk.h>

void draw_toolbar_icon(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                       gpointer symbol_color) {
  GdkRGBA theme_color;
  gtk_widget_get_color(GTK_WIDGET(area), &theme_color);
  gdk_cairo_set_source_rgba(cr, &theme_color);
  draw_stitch_swatch(area, cr, width, height, symbol_color);
}
