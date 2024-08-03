#include "../include/Character.h"
#include <stdio.h>

LatoErrorCode character_init(Character *character, FT_Face face, int key,
                             int index) {
  if (FT_Load_Char(face, key, FT_LOAD_DEFAULT) == 1) {
    return LATO_ERR_FT_LOAD_CHAR;
  }

  if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) == 1) {
    return LATO_ERR_FT_RENDER_GLYPH;
  }

  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index,
                  face->glyph->bitmap.width, face->glyph->bitmap.rows, 1,
                  GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  character->texture_id = index;
  character->size[0] = face->glyph->bitmap.width;
  character->size[1] = face->glyph->bitmap.rows;
  character->bearing[0] = face->glyph->bitmap_left;
  character->bearing[1] = face->glyph->bitmap_top;
  character->advance[0] = face->glyph->advance.x >> 6;
  character->advance[1] = face->glyph->advance.y >> 6;

  return LATO_OK;
}
