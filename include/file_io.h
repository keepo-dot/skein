#ifndef FILE_IO_H
#define FILE_IO_H //

#include "types.h"
#include <gtk/gtk.h>
#include <json-glib-1.0/json-glib/json-glib.h>

JsonBuilder *pattern_json_builder(PatternData *pattern);
void pattern_json_load(char *filename, PatternData *current_canvas);
void pattern_json_save(JsonBuilder *builder, char *file_path);

#endif // !FILE_IO_H
