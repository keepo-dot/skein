#ifndef SKEIN_TOOLBAR_H
#define SKEIN_TOOLBAR_H

#include "ui_types.h"
#include <gtk/gtk.h>

GtkWidget *create_toolbar(AppState *state);
GtkWidget *create_button(ButtonInfo *info);
GtkFileDialog *setup_file_dialog(char *window_title);

#endif // !SKEIN_TOOLBAR_H
