#include "../include/Lato.h"
#include "GL/gl.h"
#include "GL/glext.h"
#include "freetype/freetype.h"
#include <string.h>

void get_font_path(char **buffer, const char *font_name);

Lato lato_init() {
  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
  }

  FT_Face face;

  char *buffer;
  get_font_path(&buffer, "JetBrains Mono");

  if (FT_New_Face(ft, buffer, 0, &face) == 1) {
  }

  if (FT_Set_Pixel_Sizes(face, 256, 256)) {
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint texture_array;
  glGenTextures(1, &texture_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED,
               GL_UNSIGNED_BYTE, NULL);

  Character char_info[128];
  for (int i = 0; i < 128; i++) {
    char_info[i] = character_init(face, i, i);
  }

  free(buffer);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  Lato lato = {
      .index = 0,
      .char_info = char_info,
  };

  return lato;
}

void get_font_path(char **buffer, const char *font_name) {
  if (FcInit() != FcTrue) {
    return;
  }

  FcConfig *config = FcInitLoadConfigAndFonts();
  if (config == NULL) {
    FcFini();
    return;
  }

  FcPattern *pattern = FcNameParse((const FcChar8 *)font_name);
  if (pattern == NULL) {
    FcConfigDestroy(config);
    FcFini();
    return;
  }

  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);

  FcResult result;
  FcPattern *match = FcFontMatch(config, pattern, &result);

  if (match != NULL) {
    FcChar8 *font_path;
    if (FcPatternGetString(match, FC_FILE, 0, &font_path) == FcResultMatch) {
      if (font_path != NULL) {
        size_t length = strlen((const char *)font_path) + 1;
        if (buffer != NULL) {
          memcpy(buffer, font_path, length);
        }
      }
    }
  }

  FcPatternDestroy(match);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  FcFini();
}
