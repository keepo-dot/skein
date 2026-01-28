#include "resources.h"
#include "glibconfig.h"
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
     GINT_TO_POINTER(MODE_MOVE), 1},
    {"Paint Stitch", "draw-brush-symbolic", STITCH_EMPTY, MODE_PAINT, NULL,
     GINT_TO_POINTER(MODE_PAINT), 1},
    {"Erase Stitch", "draw-eraser-symbolic", STITCH_EMPTY, MODE_ERASE, NULL,
     GINT_TO_POINTER(MODE_ERASE), 1},
    {"Color Picker", "color-picker-symbolic", STITCH_EMPTY, MODE_PICKER, NULL,
     GINT_TO_POINTER(MODE_PICKER), 1},
    {"Stitch Type", "view-grid-symbolic", STITCH_EMPTY, MODE_STITCH, NULL,
     GINT_TO_POINTER(MODE_STITCH), 1}};

ButtonInfo toolbar_buttons_color[] = {
    {"Red", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_RED, (gpointer)&COLOR_RED,
     1},
    {"Orange", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_ORANGE,
     (gpointer)&COLOR_ORANGE, 1},
    {"Yellow", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_YELLOW,
     (gpointer)&COLOR_YELLOW, 1},
    {"Green", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_GREEN,
     (gpointer)&COLOR_GREEN, 1},
    {"Blue", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_BLUE, (gpointer)&COLOR_BLUE,
     1},
    {"Indigo", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_INDIGO,
     (gpointer)&COLOR_INDIGO, 1},
    {"Violet", NULL, STITCH_EMPTY, MODE_COLOR, &COLOR_VIOLET,
     (gpointer)&COLOR_VIOLET, 1}};

ButtonInfo toolbar_buttons_stitch_type[] = {
    {"Knit", NULL, STITCH_KNIT, MODE_STITCH, NULL, GINT_TO_POINTER(STITCH_KNIT),
     1},
    {"Purl", NULL, STITCH_PURL, MODE_STITCH, NULL, GINT_TO_POINTER(STITCH_PURL),
     1},
    {"Yarn Over", NULL, STITCH_YO, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_YO), 1},
    {"Knit 2 Together", NULL, STITCH_K2TOG, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_K2TOG), 1},
    {"Slip, Slip, Knit", NULL, STITCH_SSK, MODE_STITCH, NULL,
     GINT_TO_POINTER(STITCH_SSK), 1}};

ButtonInfo toolbar_buttons_fileio[] = {
    {"New Pattern", "document-new-symbolic", STITCH_EMPTY, MODE_NEWFILE, NULL,
     GINT_TO_POINTER(MODE_NEWFILE), 0},
    {"Save Pattern", "document-save-symbolic", STITCH_EMPTY, MODE_SAVEFILE,
     NULL, GINT_TO_POINTER(MODE_SAVEFILE), 0},
    {"Load Pattern", "document-open-symbolic", STITCH_EMPTY, MODE_LOADFILE,
     NULL, GINT_TO_POINTER(MODE_LOADFILE), 0}};
