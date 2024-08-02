#ifndef LIBLATO_H
#define LIBLATO_H

#define LENGTH 400

#include "Character.h"
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
  float letter_map[LENGTH];
  Mat4 transform[LENGTH];
  float start_color[LENGTH][4];
  float end_color[LENGTH][4];
  float deg[LENGTH];
  int index;
  Character char_info[128];
} Lato;

LatoErrorCode lato_init(Lato *lato);

void lato_text_place();

void lato_text_render();

#endif
