#include "button.h"
#include "colors.h"
#include "draw_color_swatch.h"
#include "draw_stitch_swatch.h"
#include <gtk/gtk.h>
#include <stdalign.h>
#include <stdio.h>

GtkWidget *create_button(ButtonInfo *info) {

  GtkWidget *button = gtk_toggle_button_new();

  // This creates tool buttons, ie with a NULL button_color.
  if (info->icon_name) {
    gtk_button_set_icon_name(GTK_BUTTON(button), info->icon_name);
    // This handles the color buttons. ie with a NULL icon_name.
  } else if (info->button_color != NULL) {
    GtkWidget *button_draw_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(button_draw_area),
                                   draw_color_swatch,
                                   (gpointer)info->button_color, NULL);
    gtk_button_set_child(GTK_BUTTON(button), button_draw_area);
    gtk_widget_set_size_request(button_draw_area, 16, 16);
  } else if (info->stitch_type != 0) {
    GtkWidget *button_draw_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(button_draw_area),
                                   draw_stitch_swatch,
                                   (gpointer)info->stitch_type, NULL);
    gtk_button_set_child(GTK_BUTTON(button), button_draw_area);
    gtk_widget_set_size_request(button_draw_area, 16, 16);

  } else if (info->label != NULL) {
    gtk_button_set_label(GTK_BUTTON(button), info->label);
  }
  gtk_widget_set_tooltip_text(
      button, info->label); // Set toolbar tip regardless of button type.

  return button;
}
