#ifndef SKEIN_WINDOW_H
#define SKEIN_WINDOW_H
#include "ui_types.h"
GtkWidget *create_main_window(GtkApplication *app, GtkWidget *container,
                              AppState *app_state);
GtkWidget *create_app_container(GtkWidget *pattern_view, GtkWidget *toolbar);
#endif
