#include "gdk-pixbuf/gdk-pixbuf.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>
#include <stddef.h>

GdkPixbuf *fetch_icon(const char *icon_name, int width, int height, int scale,
                      gboolean preserve_aspect_ratio, GError **error) {
  (void)preserve_aspect_ratio;
  GdkDisplay *display = gdk_display_get_default();
  if (!display) {
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                "Failed to get default GdkDisplay");
    return NULL;
  }
  GtkIconTheme *icon_theme = gtk_icon_theme_get_for_display(display);
  GtkIconPaintable *icon_paintable = gtk_icon_theme_lookup_icon(
      icon_theme, icon_name, NULL, width, scale, GTK_TEXT_DIR_NONE,
      GTK_ICON_LOOKUP_FORCE_SYMBOLIC);
  if (!GTK_IS_ICON_PAINTABLE(icon_paintable)) {
    g_object_unref(icon_paintable);
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                "Failed to find icon: %s", icon_name);
    return NULL;
  }
  GFile *icon_file = gtk_icon_paintable_get_file(icon_paintable);
  if (!icon_file) {
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                "Failed to get file for icon: %s", icon_name);
    g_object_unref(icon_paintable);
    return NULL;
  }
  char *icon_filepath = g_file_get_path(icon_file);
  if (!icon_filepath) {
    g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                "Failed to get path for icon: %s", icon_name);
    g_object_unref(icon_file);
    g_object_unref(icon_paintable);
    return NULL;
  }
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(icon_filepath, width,
                                                        height, FALSE, error);
  if (!pixbuf) {
    g_set_error(error, G_CONVERT_ERROR, G_CONVERT_ERROR_FAILED,
                "failed to convert file to pixbuf");
    g_object_unref(pixbuf);
    return NULL;
  }
  g_error_free(*error);
  g_free(icon_filepath);
  g_object_unref(icon_paintable);
  g_object_unref(icon_file);
  return pixbuf;
}
