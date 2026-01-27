#include "resources.h"
#include <gtk/gtk.h>

// global color constants.
const GdkRGBA COLOR_RED = {1.0, 0.0, 0.0, 1.0};
const GdkRGBA COLOR_ORANGE = {1.0, 0.65, 0.0, 1.0};
const GdkRGBA COLOR_YELLOW = {1.0, 1.0, 0.0, 1.0};
const GdkRGBA COLOR_GREEN = {0.0, 1.0, 0.0, 1.0};
const GdkRGBA COLOR_BLUE = {0.0, 0.0, 1.0, 1.0};
const GdkRGBA COLOR_INDIGO = {0.29, 0.0, 0.51, 1.0};
const GdkRGBA COLOR_VIOLET = {0.93, 0.51, 0.93, 1.0};
const GdkRGBA COLOR_WHITE = {1.0, 1.0, 1.0, 1.0};
const GdkRGBA COLOR_BLACK = {0.0, 0.0, 0.0, 1.0};

// Button arrays for building toolbar.
ButtonInfo toolbar_buttons_mode[] = {
    {"Move", "object-move", STITCH_EMPTY, MODE_MOVE, NULL,
     GINT_TO_POINTER(MODE_MOVE)},
    {"Paint Stitch", "draw-brush-symbolic", STITCH_EMPTY, MODE_PAINT, NULL,
     GINT_TO_POINTER(MODE_PAINT)},
    {"Erase Stitch", "draw-eraser-symbolic", STITCH_EMPTY, MODE_ERASE, NULL,
     GINT_TO_POINTER(MODE_ERASE)},
    {"Color Picker", "color-picker-symbolic", STITCH_EMPTY, MODE_PICKER, NULL,
     GINT_TO_POINTER(MODE_PICKER)},
    {"Stitch Type", "view-grid-symbolic", STITCH_EMPTY, MODE_STITCH, NULL,
     GINT_TO_POINTER(MODE_STITCH)}};

ButtonInfo toolbar_buttons_color[] = {
    {"Red", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_RED, (gpointer)&COLOR_RED},
    {"Orange", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_ORANGE,
     (gpointer)&COLOR_ORANGE},
    {"Yellow", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_YELLOW,
     (gpointer)&COLOR_YELLOW},
    {"Green", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_GREEN,
     (gpointer)&COLOR_GREEN},
    {"Blue", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_BLUE,
     (gpointer)&COLOR_BLUE},
    {"Indigo", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_INDIGO,
     (gpointer)&COLOR_INDIGO},
    {"Violet", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_VIOLET,
     (gpointer)&COLOR_VIOLET}};

ButtonInfo toolbar_buttons_stitch_type[] = {
    {"Knit", NULL, STITCH_KNIT, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_KNIT)},
    {"Purl", NULL, STITCH_PURL, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_PURL)},
    {"Yarn Over", NULL, STITCH_YO, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_YO)},
    {"Knit 2 Together", NULL, STITCH_K2TOG, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_K2TOG)},
    {"Slip, Slip, Knit", NULL, STITCH_SSK, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_SSK)}};
