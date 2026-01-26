#include "pattern_data.h" // For your enums
#include <gtk/gtk.h>

void draw_stitch_swatch(GtkDrawingArea *area, cairo_t *cr, int w, int h,
                        gpointer user_data) {
  int stitch_type = GPOINTER_TO_INT(user_data);

  cairo_set_line_width(cr, 2.0);
  cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

  double center_x = w / 2.0;
  double center_y = h / 2.0;
  double size = w * 0.6; // Scale relative to button size

  switch (stitch_type) {
  case STITCH_KNIT: // Vertical Line
    cairo_move_to(cr, center_x, center_y - (size / 2));
    cairo_line_to(cr, center_x, center_y + (size / 2));
    cairo_stroke(cr);
    break;

  case STITCH_PURL: // Horizontal Dash (or Dot)
    cairo_move_to(cr, center_x - (size / 2), center_y);
    cairo_line_to(cr, center_x + (size / 2), center_y);
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
