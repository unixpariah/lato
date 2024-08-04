#include "../include/Lato.h"
#include "GL/gl.h"
#include "GLES3/gl3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LatoErrorCode lato_init(Lato *lato, LatoContext *lato_context) {
  glEnable(GL_BLEND);

  // TODO: Implement all the shaders
  lato->shaders[0] = create_shader_program(solid_vert, solid_frag);
  lato->shaders[1] = create_shader_program(gradient_vert, gradient_frag);
  // lato->shaders[2] =
  //     create_shader_program(triple_gradient_vert, triple_gradient_frag);

  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
    return LATO_ERR_FT_INIT;
  }

  char *font_path;
  LatoErrorCode res = get_font_path(&font_path, lato_context->font.family);
  if (res != LATO_OK)
    return res;

  FT_Face face;
  if (FT_New_Face(ft, font_path, 0, &face) == 1) {
    free(font_path);
    return LATO_ERR_FT_FACE;
  }

  if (FT_Set_Pixel_Sizes(face, 256, 256) == 1) {
    free(font_path);
    return LATO_ERR_FT_PIXEL_SIZE;
  }

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

    lato->char_info = (Character *)calloc(arr_size + 1, sizeof(Character));
    if (lato->char_info == NULL) {
      free(font_path);
      glDeleteTextures(1, &lato->texture_array);
      FT_Done_Face(face);
      FT_Done_FreeType(ft);
      return LATO_ERR_OUT_OF_MEMORY;
    }

    break;
  }
  case CHAR_DATA_ENCODING: {
    switch (lato_context->char_data.data.encoding) {
    case LATO_ENCODING_ASCII: {
      size = 128;
      lato->char_info = (Character *)malloc(128 * sizeof(Character));
      if (lato->char_info == NULL) {
        free(font_path);
        glDeleteTextures(1, &lato->texture_array);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return LATO_ERR_OUT_OF_MEMORY;
      }
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

    lato->char_info[key] = character;

    if (res != LATO_OK) {
      free(font_path);
      return res;
    }
  }

  lato->index = 0;

  for (int i = 0; i < LENGTH; i++) {
    lato->letter_map[i] = 0;
    mat4(&lato->transform[i]);
  }

  free(font_path);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

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

      if (buffer == NULL) {
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

  FcPatternDestroy(match);
  FcPatternDestroy(pattern);
  FcConfigDestroy(config);
  FcFini();

  return LATO_ERR_FC_FONT_MATCH;
}

void lato_text_place(Lato *lato, LatoContext *lato_context, char *text, float x,
                     float y) {
  GLuint active_texture;
  glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&active_texture);

  if (active_texture != lato->texture_array) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, lato->texture_array);
  }

  float scale_a = lato_context->font.size / 256.0;
  float move = 0;

  int index = 0;
  while (text[index] != '\0') {
    Character character = lato->char_info[(int)text[index]];

    float x_pos = x + character.bearing[0] * scale_a + move;
    float y_pos = y - character.bearing[1] * scale_a;

    scale(&lato->transform[lato->index], lato_context->font.size,
          lato_context->font.size, 0);
    Mat4 translate_mat;
    translate(&translate_mat, x_pos, y_pos, 0);

    mul(&lato->transform[lato->index], &translate_mat);
    lato->letter_map[lato->index] = character.texture_id;
    // TODO: Set colors

    move += character.advance[0] * scale_a;
    lato->index++;

    if (lato->index == LENGTH) {
      lato_text_render_call(lato, lato_context);
    }

    index++;
  }
}

void lato_text_render_call(Lato *lato, LatoContext *lato_context) {
  // TODO: Finish the render call
  lato->index = 0;
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
  for (int i = 0; i < 3; i++) {
    glDeleteProgram(lato->shaders[i]);
  }
}
