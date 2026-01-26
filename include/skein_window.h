#ifndef SKEIN_WINDOW_H
#define SKEIN_WINDOW_H
#include <gtk/gtk.h>
GtkWidget *create_main_window(GtkApplication *app, GtkWidget *container);
GtkWidget *create_app_container(GtkWidget *pattern_view, GtkWidget *toolbar);
#endif
