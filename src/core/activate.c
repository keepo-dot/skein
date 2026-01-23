#include "create_app_container.h"
#include "create_main_window.h"
#include "create_pattern_view.h"
#include "create_toolbar.h"
#include "draw_grid.h"
#include <gtk/gtk.h>

void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *main_window;
  GtkWidget *pattern_view;
  PatternData *grid = (PatternData *)user_data;
  GtkWidget *container;
  GtkWidget *toolbar;

  // create the toolbar.
  toolbar = create_toolbar();
  // create pattern view.
  pattern_view = create_pattern_view(grid, draw_grid);
  // create container.
  container = create_app_container(pattern_view, toolbar);

  // create main window.
  main_window = create_main_window(app, container);

  g_print("Pattern loaded: %d x %d\n", grid->width, grid->height);
  gtk_window_present(GTK_WINDOW(main_window));
}
