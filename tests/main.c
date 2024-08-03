#include "../include/Lato.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int test_font_path(const char *font_name) {
  FT_Library ft;
  FT_Init_FreeType(&ft);

  char *font_path;
  if (get_font_path(&font_path, font_name) != LATO_OK) {
    return 0;
  }

  struct stat statbuf;
  int file_exists =
      (stat(font_path, &statbuf)) == 0 && S_ISREG(statbuf.st_mode);

  free(font_path);

  return file_exists;
}

FT_Face init_ft(int character_amount) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft) == 1) {
  }

  char *font_path;
  LatoErrorCode res = get_font_path(&font_path, "Monospace");
  if (res != LATO_OK) {
  }

  FT_Face face;
  if (FT_New_Face(ft, font_path, 0, &face) == 1) {
  }
  if (FT_Set_Pixel_Sizes(face, 256, 256) == 1) {
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  GLuint texture_array;
  glGenTextures(1, &texture_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_ARRAY, texture_array);
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, 256, 256, character_amount, 0,
               GL_RED, GL_UNSIGNED_BYTE, NULL);

  return face;
}

void test_lato_init() {
  int keycode_arr[] = {'h', 'e', 'l', 'o', 'w', 'r', 'd', '\0'};
  int *keycodes = (int *)malloc(sizeof(keycode_arr));
  memcpy(keycodes, keycode_arr, sizeof(keycode_arr));

  LatoContext context = lato_context_init();

  lato_context_set_characters(&context, keycodes);
  lato_context_set_font_size(&context, 40);
  lato_context_set_font_weight(&context, 16);
  lato_context_set_font_family(&context, "Monospace");

  Lato lato;
  lato_init(&lato, context);
  assert(lato.char_info['h'].size[0] == 108);
  assert(lato.char_info['h'].size[1] == 188);
  assert(lato.char_info['e'].size[0] == 117);
  assert(lato.char_info['e'].size[1] == 139);
  assert(lato.char_info['l'].size[0] == 123);
  assert(lato.char_info['l'].size[1] == 191);
  assert(lato.char_info['o'].size[0] == 116);
  assert(lato.char_info['o'].size[1] == 139);
  assert(lato.char_info['w'].size[0] == 134);
  assert(lato.char_info['w'].size[1] == 133);
  assert(lato.char_info['r'].size[0] == 134);
  assert(lato.char_info['r'].size[1] == 136);
  assert(lato.char_info['d'].size[0] == 117);
  assert(lato.char_info['d'].size[1] == 191);
}

int main() {
  test_lato_init();

  assert(test_font_path("Arial"));
  assert(test_font_path("Monospace"));

  return 0;
}
