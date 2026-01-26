#include "toolbar_state.h"
#include <gtk/gtk.h>
#ifndef BUTTON_H
#define BUTTON_H

typedef struct {
  const char *label;
  const char *icon_name;
  StitchType stitch_type;
  ToolbarMode button_type;
  const GdkRGBA *button_color;
} ButtonInfo;
#endif
