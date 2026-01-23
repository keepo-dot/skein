#include "create_pattern_view.h"
#include "pattern_data.h"
#include <gtk/gtk.h>

// create window etc.
GtkWidget *create_pattern_view(PatternData *grid,
                               GtkDrawingAreaDrawFunc draw_func) {
  GtkWidget *area;
  GtkWidget *scrollable_window;

  scrollable_window = gtk_scrolled_window_new();
  area = gtk_drawing_area_new();

  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area),
                                     grid->width * STITCH_SIZE);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area),
                                      grid->height * STITCH_SIZE);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_func, grid, NULL);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable_window), area);
  gtk_widget_set_vexpand(scrollable_window, TRUE);

  return scrollable_window;
}
