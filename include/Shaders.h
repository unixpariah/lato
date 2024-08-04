#ifndef SHADER_H
#define SHADER_H

#include <GL/gl.h>
#include <GLES3/gl3.h>

GLuint create_shader_program(const char *vertex_shader_source,
                             const char *fragment_shader_source);

extern const char *solid_vert;
extern const char *solid_frag;

extern const char *gradient_vert;
extern const char *gradient_frag;

extern const char *triple_gradient_vert;
extern const char *triple_gradient_frag;

#endif
