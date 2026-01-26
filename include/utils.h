#ifndef UTILS_H
#include "types.h"
#include <gtk/gtk.h>

extern const GdkRGBA COLOR_RED;
extern const GdkRGBA COLOR_ORANGE;
extern const GdkRGBA COLOR_YELLOW;
extern const GdkRGBA COLOR_GREEN;
extern const GdkRGBA COLOR_BLUE;
extern const GdkRGBA COLOR_INDIGO;
extern const GdkRGBA COLOR_VIOLET;
extern const GdkRGBA COLOR_WHITE;
extern const GdkRGBA COLOR_BLACK;

void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data);
GtkWidget *create_button(ButtonInfo *info);

#endif // !UTILS_H
