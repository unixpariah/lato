#include "../include/Lato.h"
#include <stdio.h>
#include <stdlib.h>

LatoErrorCode lato_init(Lato *lato, LatoContext *lato_context) {
  glEnable(GL_BLEND);

  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
    return LATO_ERR_FT_INIT;
  }

  char *font_path;
  LatoErrorCode res = get_font_path(&font_path, lato_context->font.family);
  if (res != LATO_OK)
    return res;

  FT_Face face;
  if (FT_New_Face(ft, font_path, 0, &face) == 1)
    return LATO_ERR_FT_FACE;

  if (FT_Set_Pixel_Sizes(face, 256, 256) == 1)
    return LATO_ERR_FT_PIXEL_SIZE;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &lato->texture_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, lato->texture_array);

  unsigned int size = 0;
  switch (lato_context->char_data.type) {
  case CHAR_DATA_CHARACTERS: {
    size = lato_context->char_data.data.length;
    int arr_size = 0;

    for (unsigned int i = 0; i < size; i++) {
      if (lato_context->char_data.data.characters[i] > arr_size) {
        arr_size = lato_context->char_data.data.characters[i];
      }
    }

    lato->char_info = (Character *)malloc(arr_size + 1 * sizeof(Character));

    break;
  }
  case CHAR_DATA_ENCODING: {
    switch (lato_context->char_data.data.encoding) {
    case LATO_ENCODING_ASCII: {
      size = 128;
      lato->char_info = (Character *)malloc(128 * sizeof(Character));
      break;
    }
    }
  }
  }

  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, size, 0, GL_RED,
               GL_UNSIGNED_BYTE, NULL);

  for (unsigned int i = 0; i < size; i++) {
    Character character;

    LatoErrorCode res;
    int key;
    if (lato_context->char_data.type == CHAR_DATA_CHARACTERS) {
      key = lato_context->char_data.data.characters[i];
      res = character_init(&character, face,
                           lato_context->char_data.data.characters[i], i);
    } else {
      key = i;
      res = character_init(&character, face, i, i);
    }

    if (res != LATO_OK)
      return res;

    lato->char_info[key] = character;
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

  return LATO_OK;
}

void lato_destroy(Lato *lato, LatoContext *lato_context) {
  glDeleteTextures(1, &lato->texture_array);
  if (lato_context->char_data.type == CHAR_DATA_CHARACTERS) {
    for (int i = 0; i < lato_context->char_data.data.length; i++) {
      character_destroy(
          &lato->char_info[lato_context->char_data.data.characters[i]]);
    }
  } else {
    for (int i = 0; i < lato_context->char_data.data.length; i++) {
      character_destroy(&lato->char_info[i]);
    }
  }
  free(lato->char_info);
}
