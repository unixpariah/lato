#include "../include/Lato.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LatoErrorCode lato_init(Lato *lato, const char *font_family,
                        CharDataType char_data_type, CharData char_data) {
  lato->font.family = font_family;
  lato->font.size = 16;
  lato->font.weight = 10;

  lato->char_data.type = char_data_type;
  lato->char_data.data = char_data;

  lato->index = 0;

  for (int i = 0; i < LENGTH; i++) {
    lato->instance_data.letter_map[i] = 0;
    mat4(&lato->instance_data.transform[i]);
  }

  int length = 0;
  switch (lato->char_data.type) {
  case CHAR_DATA_ENCODING:
    switch (lato->char_data.data.encoding) {
    case LATO_ENCODING_ASCII:
      length = 128;
      break;
    }
    break;
  case CHAR_DATA_CHARACTERS:
    length = lato->char_data.data.length;
    break;
  }

  glEnable(GL_BLEND);

  float color[4] = {0, 0, 0, 0};
  lato_set_solid_color(lato, color);

  lato->shaders[0] = create_shader_program(solid_vert, solid_frag);
  lato->shaders[1] = create_shader_program(gradient_vert, gradient_frag);
  lato->shaders[2] =
      create_shader_program(triple_gradient_vert, triple_gradient_frag);

  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
    return LATO_ERR_FT_INIT;
  }

  char *font_path;
  LatoErrorCode res = get_font_path(&font_path, font_family);
  if (res != LATO_OK)
    return res;

  FT_Face face;
  if (FT_New_Face(ft, font_path, 0, &face) == 1) {
    return LATO_ERR_FT_FACE;
  }

  free(font_path);

  if (FT_Set_Pixel_Sizes(face, 256, 256) == 1) {
    return LATO_ERR_FT_PIXEL_SIZE;
  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &lato->texture_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, lato->texture_array);

  switch (lato->char_data.type) {
  case CHAR_DATA_ENCODING:
    lato->char_info = (Character *)calloc(length, sizeof(Character));
    break;
  case CHAR_DATA_CHARACTERS: {
    int arr_size = lato->char_data.data.characters[0];
    for (int i = 0; i < length; i++) {
      if (lato->char_data.data.characters[i] > arr_size) {
        arr_size = lato->char_data.data.characters[i];
      }
    }

    lato->char_info = (Character *)calloc(arr_size + 1, sizeof(Character));
    break;
  };
  }

  if (lato->char_info == NULL) {
    glDeleteTextures(1, &lato->texture_array);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return LATO_ERR_OUT_OF_MEMORY;
  }

  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, length, 0, GL_RED,
               GL_UNSIGNED_BYTE, NULL);

  for (int i = 0; i < length; i++) {
    Character character;

    LatoErrorCode res;
    int key;

    if (lato->char_data.type == CHAR_DATA_CHARACTERS) {
      key = lato->char_data.data.characters[i];
      res = character_init(&character, face, lato->char_data.data.characters[i],
                           i);
    } else {
      key = i;
      res = character_init(&character, face, i, i);
    }

    lato->char_info[key] = character;

    if (res != LATO_OK)
      return res;
  }

  glGenBuffers(1, &lato->UBO);
  glGetIntegerv(GL_VIEWPORT, lato->viewport);

  Mat4 projection;
  ortographic_projection(&projection, lato->viewport[0], lato->viewport[3],
                         lato->viewport[1], lato->viewport[2]);

  glBindBuffer(GL_UNIFORM_BUFFER, lato->UBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(projection), &projection,
               GL_STATIC_DRAW);

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

void lato_text_place(Lato *lato, char *text, float x, float y) {
  GLuint active_texture;
  glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&active_texture);

  if (active_texture != lato->texture_array) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, lato->texture_array);
  }

  float scale_a = lato->font.size / 256.0;
  float move = 0;

  int index = 0;
  while (text[index] != '\0') {
    Character character = lato->char_info[(int)text[index]];

    float x_pos = x + character.bearing[0] * scale_a + move;
    float y_pos = y - character.bearing[1] * scale_a;

    scale(&lato->instance_data.transform[lato->index], lato->font.size,
          lato->font.size, 0);
    Mat4 translate_mat;
    translate(&translate_mat, x_pos, y_pos, 0);

    mul(&lato->instance_data.transform[lato->index], &translate_mat);
    lato->instance_data.letter_map[lato->index] = character.texture_id;

    GLuint active_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&active_program);

    switch (lato->color.type) {
    case COLOR_SOLID: {
      if (active_program != lato->shaders[0]) {
        glUseProgram(lato->shaders[0]);
      }

      for (int i = 0; i < 4; i++) {
        lato->instance_data.color1[index][i] = lato->color.color1[i];
      }

      break;
    }
    case COLOR_GRADIENT: {
      if (active_program != lato->shaders[1]) {
        glUseProgram(lato->shaders[1]);
      }

      for (int i = 0; i < 4; i++) {
        lato->instance_data.color1[index][i] = lato->color.color1[i];
        lato->instance_data.color2[index][i] = lato->color.color2[i];
      }

      lato->instance_data.degrees[index] = lato->color.degrees;

      break;
    }
    case COLOR_TRIPLE_GRADIENT: {
      if (active_program != lato->shaders[2]) {
        glUseProgram(lato->shaders[2]);
      }

      for (int i = 0; i < 4; i++) {
        lato->instance_data.color1[index][i] = lato->color.color1[i];
        lato->instance_data.color2[index][i] = lato->color.color2[i];
        lato->instance_data.color3[index][i] = lato->color.color3[i];
      }

      lato->instance_data.degrees[index] = lato->color.degrees;

      break;
    }
    }

    move += character.advance[0] * scale_a;
    lato->index++;

    if (lato->index == LENGTH) {
      lato_text_render_call(lato);
    }

    index++;
  }
}

void lato_text_render_call(Lato *lato) {
  GLint new_viewport[4];
  glGetIntegerv(GL_VIEWPORT, new_viewport);

  for (int i = 0; i < 4; i++) {
    if (lato->viewport[i] != new_viewport[i]) {
      for (int j = 0; j < 4; j++) {
        lato->viewport[j] = new_viewport[j];
      }

      Mat4 projection;
      ortographic_projection(&projection, lato->viewport[0], lato->viewport[3],
                             lato->viewport[1], lato->viewport[2]);

      glBindBuffer(GL_UNIFORM_BUFFER, lato->UBO);
      glBufferData(GL_UNIFORM_BUFFER, sizeof(projection), &projection,
                   GL_STATIC_DRAW);
      break;
    }
  }

  GLuint current_program = 0;

  switch (lato->color.type) {
  case COLOR_SOLID: {
    current_program = lato->shaders[0];
    glUniform4fv(glGetUniformLocation(lato->shaders[0], "color"), lato->index,
                 &lato->instance_data.color1[0][0]);
    break;
  }
  case COLOR_GRADIENT: {
    current_program = lato->shaders[1];
    glUniform4fv(glGetUniformLocation(lato->shaders[0], "startColor"),
                 lato->index, &lato->instance_data.color1[0][0]);
    glUniform4fv(glGetUniformLocation(lato->shaders[0], "endColor"),
                 lato->index, &lato->instance_data.color2[0][0]);
    glUniform1fv(glGetUniformLocation(lato->shaders[0], "degrees"), lato->index,
                 &lato->instance_data.degrees[0]);
    break;
  }
  case COLOR_TRIPLE_GRADIENT: {
    return;

    current_program = lato->shaders[2];

    GLuint active_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint *)&active_program);
    if (active_program != current_program) {
      glUseProgram(current_program);
    }

    glUniform4fv(glGetUniformLocation(lato->shaders[0], "startColor"),
                 lato->index, &lato->instance_data.color1[0][0]);
    glUniform4fv(glGetUniformLocation(lato->shaders[0], "midColor"),
                 lato->index, &lato->instance_data.color2[0][0]);
    glUniform4fv(glGetUniformLocation(lato->shaders[0], "endColor"),
                 lato->index, &lato->instance_data.color3[0][0]);
    glUniform1fv(glGetUniformLocation(lato->shaders[0], "degrees"), lato->index,
                 &lato->instance_data.degrees[0]);
    break;
  }
  }

  glUniformMatrix4fv(glGetUniformLocation(current_program, "transform"),
                     lato->index, GL_FALSE,
                     &lato->instance_data.transform[0][0][0]);

  glUniformMatrix4fv(glGetUniformLocation(current_program, "letterMap"),
                     lato->index, GL_FALSE, &lato->instance_data.letter_map[0]);

  lato->index = 0;
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void lato_set_solid_color(Lato *lato, float color[4]) {
  Color color_struct = {
      .type = COLOR_SOLID,
      .color1 = {color[0], color[1], color[2], color[3]},
  };

  lato->color = color_struct;
}

void lato_set_gradient_color(Lato *lato, float start_color[4],
                             float end_color[4], float deg) {
  Color color_struct = {
      .type = COLOR_GRADIENT,
      .color1 =
          {
              start_color[0],
              start_color[1],
              start_color[2],
              start_color[3],
          },
      .color2 =
          {
              end_color[0],
              end_color[1],
              end_color[2],
              end_color[3],
          },
      .degrees = deg,
  };

  lato->color = color_struct;
}

void lato_set_triple_gradient_color(Lato *lato, float start_color[4],
                                    float mid_color[4], float end_color[4],
                                    float deg) {
  Color color_struct = {
      .type = COLOR_TRIPLE_GRADIENT,
      .color1 =
          {
              start_color[0],
              start_color[1],
              start_color[2],
              start_color[3],
          },
      .color2 =
          {
              mid_color[0],
              mid_color[1],
              mid_color[2],
              mid_color[3],
          },
      .color3 =
          {
              end_color[0],
              end_color[1],
              end_color[2],
              end_color[3],
          },
      .degrees = deg,
  };

  lato->color = color_struct;
}

void lato_destroy(Lato *lato) {
  glDeleteTextures(1, &lato->texture_array);
  if (lato->char_data.type == CHAR_DATA_CHARACTERS) {
    for (int i = 0; i < lato->char_data.data.length; i++) {
      character_destroy(&lato->char_info[lato->char_data.data.characters[i]]);
    }
  } else {
    for (int i = 0; i < lato->char_data.data.length; i++) {
      character_destroy(&lato->char_info[i]);
    }
  }
  free(lato->char_info);
  for (int i = 0; i < 3; i++) {
    glDeleteProgram(lato->shaders[i]);
  }
}
