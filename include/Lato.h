#define LENGTH 400

#include "Character.h"
#include <GL/gl.h>
#include <fontconfig/fontconfig.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>
#include <freetype/ftsynth.h>
#include <ft2build.h>

typedef struct {
  float letter_map[LENGTH];
  float transform[LENGTH][4][4];
  float start_color[LENGTH][4];
  float end_color[LENGTH][4];
  float deg[LENGTH];
  int index;
  Character *char_info;
} Lato;

Lato lato_init();

void lato_text_place();

void lato_text_render();
