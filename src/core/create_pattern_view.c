#include "create_pattern_view.h"
#include "activate.h"
#include "pattern_data.h"
#include <gtk/gtk.h>

static gboolean needs_redraw(GtkWidget *widget, GdkFrameClock *frame_clock,
                             gpointer user_data) {
  PatternData *grid = (PatternData *)user_data;

  if (grid->redraw == true) {
    gtk_widget_queue_draw(widget);
    grid->redraw = false; // reset draw flag to avoid runaway.
  }

  return G_SOURCE_CONTINUE; // tells gtk to run the function again.
}

// create window etc.
GtkWidget *create_pattern_view(PatternData *grid,
                               GtkDrawingAreaDrawFunc draw_func) {
  GtkWidget *area;
  GtkWidget *scrollable_window;

  scrollable_window = gtk_scrolled_window_new();
  area = gtk_drawing_area_new();
  grid->redraw = true;
  gtk_widget_add_tick_callback(
      area, needs_redraw, grid,
      NULL); // adds callback to check if the pattern has changed.
             // makes sure the fps doesn't go zoooooom.
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area),
                                     grid->width * STITCH_SIZE);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area),
                                      grid->height * STITCH_SIZE);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_func, grid, NULL);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable_window), area);
  gtk_widget_set_vexpand(scrollable_window, TRUE);

  return scrollable_window;
}
