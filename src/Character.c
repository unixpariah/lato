#include "../include/Character.h"

Character character_init(FT_Face face, int key, int index) {
  if (FT_Load_Char(face, key, FT_LOAD_DEFAULT) == 1) {
  }

  if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) == 1) {
  }

  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index,
                  face->glyph->bitmap.width, face->glyph->bitmap.rows, 1,
                  GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  Character character = {
      .texture_id = index,
      .key = key,
      .size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
      .bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
      .advance = {face->glyph->advance.x >> 6, face->glyph->advance.y >> 6},
  };

  return character;
}
