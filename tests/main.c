#include "../include/Lato.h"
#include "GL/glext.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_font_path(const char *font_name) {
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

void test_lato_init() {
  int keycodes[7] = {'h', 'e', 'l', 'o', 'w', 'r', 'd'};

  CharData char_data = {
      .length = 7,
      .characters = malloc(sizeof(int) * 7),
  };

  char_data.characters = keycodes;

  Lato lato;
  lato_init(&lato, "Monospace", CHAR_DATA_CHARACTERS, char_data);

  float color[4] = {0, 0, 0, 0};
  lato_set_solid_color(&lato, color);

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

  lato_destroy(&lato);
}

void glMessageCallback(unsigned int source, unsigned int err_type,
                       unsigned int id, unsigned int severity, int length,
                       const char *message, const void *_) {
  printf("%d %d %d %d %d %s", source, err_type, id, severity, length, message);
}

int main() {
  assert(test_font_path("Arial"));
  assert(test_font_path("Monospace"));

  glfwInit();
  GLFWwindow *window = glfwCreateWindow(800, 600, "GLFW Window", NULL, NULL);
  glfwMakeContextCurrent(window);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glMessageCallback, 0);

  test_lato_init();

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
