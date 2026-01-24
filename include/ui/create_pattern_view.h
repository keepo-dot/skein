#include "pattern_data.h"
#include <gtk/gtk.h>

#ifndef CREATE_PATTERN_VIEW
#define CREATE_PATTERN_VIEW
GtkWidget *create_pattern_view(PatternData *grid,
                               GtkDrawingAreaDrawFunc draw_func);
#endif
