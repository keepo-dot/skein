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
  grid.stitch_data = calloc(grid.width * grid.height, sizeof(StitchData));
  // paint all stitches to empty white.
  for (int i = 0; i < (grid.width * grid.height); i++) {
    grid.stitch_data[i].stitch_color.red = 1.0;
    grid.stitch_data[i].stitch_color.green = 1.0;
    grid.stitch_data[i].stitch_color.blue = 1.0;
    grid.stitch_data[i].stitch_color.alpha = 1.0;
  }
  g_print("Set stitch color to:\n R: %f\n G: %f\n B: %f\n",
          grid.stitch_data[0].stitch_color.red,
          grid.stitch_data[0].stitch_color.green,
          grid.stitch_data[0].stitch_color.blue);

  // Draw window.
  app =
      gtk_application_new("com.github.keepo_dot", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), &grid);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  free(grid.stitch_data);
  g_object_unref(app);

  return status;
}
