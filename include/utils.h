#ifndef UTILS_H
#define UTILS_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>

GdkPixbuf *fetch_icon(const char *icon_name, int width, int height, int scale,
                      gboolean preserve_aspect_ratio, GError **error);

#endif // !UTILS_H
