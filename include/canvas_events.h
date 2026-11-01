#ifndef CANVAS_EVENTS_H
#define CANVAS_EVENTS_H

#include "ui_types.h"

gboolean undo_action(GtkWidget *widget, GVariant *args, gpointer app_state);

gboolean redo_action(GtkWidget *widget, GVariant *args, gpointer app_state);

void on_drag_end(GtkGestureDrag *gesture, double offset_x, double offset_y,
                 AppState *app_state);
void on_drag_update(GtkGestureDrag *gesture, double offset_x, double offset_y,
                    AppState *app_state);
void on_drag_begin(GtkGestureDrag *gesture, double start_x, double start_y,
                   AppState *app_state);
gboolean on_scroll(GtkEventControllerScroll *controller, double dx, double dy,
                   AppState *app_state);
gboolean on_motion(GtkEventControllerMotion *controller, double mx, double my,
                   AppState *app_state);

#endif // CANVAS_EVENTS_H
