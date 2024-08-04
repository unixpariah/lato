#ifndef CHARACTER_H
#define CHARACTER_H

#include "Error.h"
#include <GL/gl.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>
#include <freetype/ftsynth.h>
#include <ft2build.h>

typedef struct {
  GLuint texture_id;
  int size[2];
  int bearing[2];
  int advance[2];
} Character;

LatoErrorCode character_init(Character *character, FT_Face face,
                             unsigned int key, GLuint index);

void character_destroy(Character *character);

#endif
