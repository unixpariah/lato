#include "../include/Lato.h"
#include <assert.h>
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
  FT_Done_FreeType(ft);

  return file_exists;
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
  lato_init(&lato, &context);
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

  lato_destroy(&lato, &context);
  lato_context_destroy(&context);
}

int main() {
  test_lato_init();

  assert(test_font_path("Arial"));
  assert(test_font_path("Monospace"));

  return 0;
}
