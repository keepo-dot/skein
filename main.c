#include "pattern_data/pattern_data.h"
#include <gtk/gtk.h>
#include <stdio.h>

static void draw_grid(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                      gpointer user_data) {
  PatternData *grid = user_data;
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

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  PatternData *grid = (PatternData *)user_data;
  GtkWidget *scrollable_window;
  GtkWidget *area;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "KnitPatC");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  scrollable_window =
      gtk_scrolled_window_new(); // NULL scroll behavior, let GTK handle it.

  area = gtk_drawing_area_new(); // create new drawing area. returns empty area.
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area),
                                     grid->width * STITCH_SIZE);

  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area),
                                      grid->height * STITCH_SIZE);

  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_grid, grid, NULL);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollable_window), area);

  gtk_window_set_child(GTK_WINDOW(window), scrollable_window);

  g_print("Pattern loaded: %d x %d\n", grid->width,
          grid->height); // debug to make sure things work.
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  // Setup grid.
  PatternData grid;

  grid.width = 10;
  grid.height = 15;

  // Allocate memory for the grid.
  grid.grid_data = calloc(grid.width * grid.height, sizeof(Stitch));

  // Draw window.
  app =
      gtk_application_new("com.github.keepo_dot", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), &grid);
  // draw_grid() goes here?
  status = g_application_run(G_APPLICATION(app), argc, argv);
  free(grid.grid_data);
  g_object_unref(app);

  return status;
}
