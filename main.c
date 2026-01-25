#include "activate.h"
#include "app_state.h"
#include "colors.h"
#include "glib.h"
#include "pattern_data.h"
#include "toolbar_state.h"
#include "ui_state.h"
#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;
  AppState *master_state;
  UiState *ui_state;
  // ToolbarState *toolbar_state;

  // Setup grid.
  PatternData grid;

  grid.width = 1000;
  grid.height = 1000;

  // Allocate memory for AppState.
  master_state = calloc(1, sizeof(AppState));
  if (master_state == NULL) {
    g_print("Error: master state memory allocation failed.");
    return 1;
  }

  // Allocate memory for the grid.
  grid.stitch_data = calloc(grid.width * grid.height, sizeof(StitchData));
  if (grid.stitch_data == NULL) {
    g_print("Error: stitch data memory allocation failed.");
    return 1;
  }
  // Allocate memory for the UiState.
  ui_state = calloc(1, sizeof(UiState));
  if (ui_state == NULL) {
    g_print("Error: ui state memory allocation failed.");
    return 1;
  }
  // Allocate memory for ToolbarState.
  ui_state->toolbar_state = calloc(1, sizeof(ToolbarState));
  if (ui_state->toolbar_state == NULL) {
    g_print("Error: toolbar state memory allocation failed.");
    return 1;
  }

  master_state->pattern = &grid;
  master_state->ui = ui_state;
  master_state->ui->pattern_data = master_state->pattern;

  // paint all stitches to empty white.
  for (int i = 0; i < (grid.width * grid.height); i++) {
    grid.stitch_data[i].stitch_color = COLOR_WHITE;
  }
  /* DEBUG
  g_print("Set stitch color to:\n R: %f\n G: %f\n B: %f\n",
          grid.stitch_data[0].stitch_color.red,
          grid.stitch_data[0].stitch_color.green,
          grid.stitch_data[0].stitch_color.blue);
*/
  // Calculate the total number of stitches
  size_t n_stitches = (size_t)grid.width * grid.height;

  // Calculate Total Bytes:
  // The compiler automatically uses the smaller size (16 bytes) for
  // sizeof(StitchData)
  double total_bytes =
      sizeof(PatternData) + (double)(n_stitches * sizeof(StitchData));

  // Convert to MB
  g_print("New Setup Memory: %.2f MB\n", total_bytes / 1048576.0);
  // Draw window.
  app =
      gtk_application_new("com.github.keepo_dot", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), master_state);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  free(grid.stitch_data);
  free(ui_state->toolbar_state);
  free(ui_state);
  free(master_state);

  g_object_unref(app);

  return status;
}
