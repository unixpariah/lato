#include "../include/Lato.h"

LatoErrorCode get_font_path(char **buffer, const char *font_name);

LatoErrorCode lato_init(Lato *lato) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
    return LATO_ERR_FT_INIT;
  }

  char *font_path;
  LatoErrorCode res = get_font_path(&font_path, "JetBrains Mono");
  if (res != LATO_OK)
    return res;

  FT_Face face;
  if (FT_New_Face(ft, font_path, 0, &face) == 1)
    return LATO_ERR_FT_FACE;

  if (FT_Set_Pixel_Sizes(face, 256, 256) == 1)
    return LATO_ERR_FT_PIXEL_SIZE;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint texture_array;
  glGenTextures(1, &texture_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, 128, 0, GL_RED,
               GL_UNSIGNED_BYTE, NULL);

  for (int i = 0; i < 128; i++) {
    LatoErrorCode res = character_init(&lato->char_info[i], face, i, i);
    if (res != LATO_OK)
      return res;
  }

  free(font_path);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  lato->index = 0;

  for (int i = 0; i < LENGTH; i++) {
    lato->letter_map[i] = 0;
    mat4(&lato->transform[i]);
  }

  return LATO_OK;
}

LatoErrorCode get_font_path(char **buffer, const char *font_name) {
  // Initialize FontConfig
  if (FcInit() != FcTrue) {
    return LATO_ERR_FC_INIT;
  }

  FcConfig *config = FcInitLoadConfigAndFonts();
  if (config == NULL) {
    FcFini();
    return LATO_ERR_FC_LOAD_CONFIG;
  }

  FcPattern *pattern = FcNameParse((const FcChar8 *)font_name);
  if (pattern == NULL) {
    FcConfigDestroy(config);
    FcFini();
    return LATO_ERR_FC_NAME_PARSE;
  }

  FcConfigSubstitute(config, pattern, FcMatchPattern);
  FcDefaultSubstitute(pattern);

  FcResult result;
  FcPattern *match = FcFontMatch(config, pattern, &result);

  if (match != NULL) {
    FcChar8 *font_path;
    if (FcPatternGetString(match, FC_FILE, 0, &font_path) == FcResultMatch &&
        font_path != NULL) {
      size_t length = strlen((const char *)font_path) + 1;
      *buffer = malloc(length);

      if (*buffer == NULL) {
        FcPatternDestroy(match);
        FcPatternDestroy(pattern);
        FcConfigDestroy(config);
        FcFini();
        return LATO_ERR_OUT_OF_MEMORY;
      }

      memcpy(*buffer, font_path, length);
      (*buffer)[length - 1] = '\0';

      FcPatternDestroy(match);
      FcPatternDestroy(pattern);
      FcConfigDestroy(config);
      FcFini();

      return LATO_OK;
    }
  }

  if (*buffer != NULL) {
    free(*buffer);
  }
  FcPatternDestroy(match);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  FcFini();

  return LATO_ERR;
}
