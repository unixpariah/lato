#include "../include/Context.h"
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

LatoContext lato_context_init() {
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
  return lato_context;
}

void lato_context_destroy(LatoContext *lato_context) {
  if (lato_context->char_data.type == CHAR_DATA_CHARACTERS) {
    free(lato_context->char_data.data.characters);
  }
}
