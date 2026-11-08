#ifndef SKEIN_DIALOGS_H
#define SKEIN_DIALOGS_H

#include "ui_types.h"
#include <gtk/gtk.h>

// New pattern dimension prompt
void show_new_pattern_dialog(GtkWidget *main_window, AppState *app_state);
void on_new_pattern_confirm(GtkWidget *button, gpointer app_state);

// Repeat section naming prompt
void show_repeat_dialog(GtkWidget *main_window, AppState *app_state);
void on_repeat_confirm(GtkWidget *button, AppState *app_state);

#endif // !SKEIN_DIALOGS_H
