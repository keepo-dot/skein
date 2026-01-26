#include "app_state.h"
#include <gtk/gtk.h>

#ifndef ON_DRAG_BEGIN
#define ON_DRAG_BEGIN

void on_drag_begin(GtkGestureDrag *gesture, double start_x, double start_y,
                   AppState *app_state);
#endif /* ifndef ON_DRAG_BEGIN                                                 \
#define ON_DRAG_BEGIN                                                          \
 */
