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
  int texture_id;
  int key;
  int size[2];
  int bearing[2];
  int advance[2];
} Character;

LatoErrorCode character_init(Character *character, FT_Face face, int key,
                             int index);

#endif
