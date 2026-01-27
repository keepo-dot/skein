#ifndef RESOURCES_H
#define RESOURCES_H

#include "types.h"
#include <gtk/gtk.h>

// color constants.
extern const GdkRGBA COLOR_RED;
extern const GdkRGBA COLOR_ORANGE;
extern const GdkRGBA COLOR_YELLOW;
extern const GdkRGBA COLOR_GREEN;
extern const GdkRGBA COLOR_BLUE;
extern const GdkRGBA COLOR_INDIGO;
extern const GdkRGBA COLOR_VIOLET;
extern const GdkRGBA COLOR_WHITE;
extern const GdkRGBA COLOR_BLACK;

// toolbar data arrays.
extern ButtonInfo toolbar_buttons_mode[];
#define MODE_BUTTON_COUNT 5
extern ButtonInfo toolbar_buttons_color[];
#define COLOR_BUTTON_COUNT 7
extern ButtonInfo toolbar_buttons_stitch_type[];
#define STITCH_BUTTON_COUNT 5
#endif // !RESOURCES_H
