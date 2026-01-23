#include "activate.h"
#include "pattern_data.h"
#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  // Setup grid.
  PatternData grid;

  grid.width = 50;
  grid.height = 50;

  // Allocate memory for the grid.
  grid.grid_data = calloc(grid.width * grid.height, sizeof(Stitch));

  // Draw window.
  app =
      gtk_application_new("com.github.keepo_dot", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), &grid);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  free(grid.grid_data);
  g_object_unref(app);

  return status;
}
