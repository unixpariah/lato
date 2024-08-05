#include "../include/Context.h"
#include "GLES3/gl3.h"
#include <stdlib.h>

void lato_context_set_font_family(LatoContext *lato_context,
                                  const char *font_family) {
  lato_context->font.family = font_family;
}

void lato_context_set_font_size(LatoContext *lato_context, float size) {
  lato_context->font.size = size;
}

void lato_context_set_font_weight(LatoContext *lato_context, float weight) {
  lato_context->font.weight = weight;
}

void lato_context_set_characters(LatoContext *lato_context, int characters[]) {
  if (lato_context->char_data.type == CHAR_DATA_CHARACTERS) {
    free(lato_context->char_data.data.characters);
  }

  int length = 0;
  while (characters[length] != '\0') {
    length++;
  }

  lato_context->char_data.type = CHAR_DATA_CHARACTERS;
  lato_context->char_data.data.length = length;
  lato_context->char_data.data.characters = characters;
}

void lato_context_set_encoding(LatoContext *lato_context,
                               CharDataType data_type) {
  lato_context->char_data.type = CHAR_DATA_ENCODING;
}

LatoContext lato_context_init(float left, float right, float top,
                              float bottom) {
  LatoContext lato_context = {
      .font =
          {
              .size = 16,
              .family = "Monospace",
              .weight = 10,
          },
      .char_data =
          {
              .type = CHAR_DATA_ENCODING,
              .data.encoding = LATO_ENCODING_ASCII,
          },
  };

  glGenBuffers(1, &lato_context.UBO);

  Mat4 projection;
  ortographic_projection(&projection, left, right, top, bottom);
  glBindBuffer(GL_UNIFORM_BUFFER, lato_context.UBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Mat4), &projection, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  return lato_context;
}

void lato_context_resize_surface(LatoContext *lato_context, float left,
                                 float right, float top, float bottom) {
  glBindBuffer(GL_UNIFORM_BUFFER, lato_context->UBO);

  Mat4 projection;
  ortographic_projection(&projection, left, right, top, bottom);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Mat4), &projection, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void lato_context_destroy(LatoContext *lato_context) {
  if (lato_context->char_data.type == CHAR_DATA_CHARACTERS) {
    free(lato_context->char_data.data.characters);
  }
}
