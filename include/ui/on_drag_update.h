#ifndef ON_DRAG_UPDATE
#define ON_DRAG_UPDATE
#include "app_state.h"
#include <gtk/gtk.h>
void on_drag_update(GtkGestureDrag *gesture, double offset_x, double offset_y,
                    AppState *app_state);

#endif // !ON_DRAG_UPDATE
#define ON_DRAG_UPDATE
