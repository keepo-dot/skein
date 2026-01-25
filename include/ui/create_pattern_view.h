#include "app_state.h"
#include <gtk/gtk.h>

#ifndef CREATE_PATTERN_VIEW
#define CREATE_PATTERN_VIEW
GtkWidget *create_pattern_view(AppState *app_state,
                               GtkDrawingAreaDrawFunc draw_func);
#endif
