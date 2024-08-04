#ifndef LIBLATO_H
#define LIBLATO_H

#define LENGTH 400

#include "Character.h"
#include "Context.h"
#include "Error.h"
#include "GL/glext.h"
#include "math.h"
#include <GL/gl.h>
#include <fontconfig/fontconfig.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>
#include <freetype/ftsynth.h>
#include <ft2build.h>
#include <string.h>

typedef struct {
  GLuint texture_array;
  float letter_map[LENGTH];
  Mat4 transform[LENGTH];
  int index;
  Character *char_info;
} Lato;

LatoErrorCode lato_init(Lato *lato, LatoContext *lato_context);

void lato_destroy(Lato *lato, LatoContext *lato_context);

LatoErrorCode get_font_path(char **buffer, const char *font_name);

#endif
