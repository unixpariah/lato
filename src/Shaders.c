#include "../include/Shaders.h"
#include <assert.h>
#include <stdio.h>

int compile_shader(const char *shader_source, GLuint shader,
                   GLuint shader_program) {
  glShaderSource(shader, 1, &shader_source, 0);

  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog(shader, 512, 0, info_log);
    printf("%s", info_log);
    return 1;
  }

  glAttachShader(shader_program, shader);

  return 0;
}

GLuint create_shader_program(const char *vertex_shader_source,
                             const char *fragment_shader_source) {
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  GLuint shader_program = glCreateProgram();

  assert(compile_shader(vertex_shader_source, vertex_shader, shader_program) ==
         0);

  assert(compile_shader(fragment_shader_source, fragment_shader,
                        shader_program) == 0);

  glLinkProgram(shader_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}

const char *solid_vert =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) in vec2 in_pos;\n"
    "\n"
    "layout(std140) uniform UniformBlock { mat4 projection; };\n"
    "uniform mat4 transform[400];\n"
    "\n"
    "out VS_OUT {\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    "vs_out;\n"
    "\n"
    "void main() {\n"
    "  vec4 position =\n"
    "      projection * transform[gl_InstanceID] * vec4(in_pos, 0.0, 1.0);\n"
    "  gl_Position = position;\n"
    "\n"
    "  vs_out.texCoords = in_pos;\n"
    "  vs_out.index = gl_InstanceID;\n"
    "}\n";

const char *solid_frag =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "\n"
    "uniform vec4 color[400];\n"
    "uniform sampler2DArray text;\n"
    "uniform int letterMap[400];\n"
    "\n"
    "in VS_OUT {\n"
    "  vec2 pos;\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    "fs_in;\n"
    "\n"
    "void main() {\n"
    "  FragColor =\n"
    "      color[fs_in.index] * texture(text, vec3(fs_in.texCoords,\n"
    "      letterMap[fs_in.index])).r;\n"
    "}\n";

const char *gradient_vert =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) in vec2 in_pos;\n"
    "\n"
    "layout(std140) uniform UniformBlock { mat4 projection; };\n"
    "uniform mat4 transform[400];\n"
    "\n"
    "out VS_OUT {\n"
    "  vec2 pos;\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    " vs_out;\n"
    "\n"
    "void main() {\n"
    "  vec4 position =\n"
    "      projection * transform[gl_InstanceID] * vec4(in_pos, 0.0, 1.0);\n"
    "  gl_Position = position;\n"
    "\n"
    "  vs_out.pos = position.xy;\n"
    "  vs_out.texCoords = in_pos;\n"
    "  vs_out.index = gl_InstanceID;\n"
    "}\n";

const char *gradient_frag =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "\n"
    "uniform vec4 startColor[400];\n"
    "uniform vec4 endColor[400];\n"
    "uniform float degrees[400];\n"
    "uniform sampler2DArray text;\n"
    "uniform int letterMap[400];\n"
    "\n"
    "in VS_OUT {\n"
    "  vec2 pos;\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    "fs_in;\n"
    "\n"
    "void main() {\n"
    "  vec2 uv = fs_in.pos - 0.5;\n"
    "\n"
    "  float angle = radians(degrees[fs_in.index]);\n"
    "  vec2 rotatedUV = vec2(cos(angle) * uv.x - sin(angle) * uv.y,\n"
    "                        sin(angle) * uv.x + cos(angle) * uv.y) +\n"
    "                   0.5;\n"
    "\n"
    "  float gradientFactor = smoothstep(0.0, 1.0, rotatedUV.x);\n"
    "  vec4 color =\n"
    "      mix(startColor[fs_in.index], endColor[fs_in.index],\n"
    "      gradientFactor);\n"
    "\n"
    "  FragColor =\n"
    "      color * texture(text, vec3(fs_in.texCoords,\n"
    "      letterMap[fs_in.index])).r;\n"
    "}\n";

const char *triple_gradient_vert =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) in vec2 in_pos;\n"
    "\n"
    "layout(std140) uniform UniformBlock { mat4 projection; };\n"
    "uniform mat4 transform[400];\n"
    "\n"
    "out VS_OUT {\n"
    "  vec2 pos;\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    "vs_out;\n"
    "\n"
    "void main() {\n"
    "  vec4 position =\n"
    "      projection * transform[gl_InstanceID] * vec4(in_pos, 0.0, 1.0);\n"
    "  gl_Position = position;\n"
    "\n"
    "  vs_out.pos = position.xy;\n"
    "  vs_out.texCoords = in_pos;\n"
    "  vs_out.index = gl_InstanceID;\n"
    "}\n";

const char *triple_gradient_frag =
    "#version 460 core\n"
    "\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "\n"
    "uniform vec4 startColor[400];\n"
    "uniform vec4 midColor[400];\n"
    "uniform vec4 endColor[400];\n"
    "uniform float degrees[400];\n"
    "uniform sampler2DArray text;\n"
    "uniform int letterMap[400];\n"
    "\n"
    "in VS_OUT {\n"
    "  vec2 pos;\n"
    "  vec2 texCoords;\n"
    "  flat int index;\n"
    "}\n"
    "fs_in;\n"
    "\n"
    "void main() {\n"
    "  vec2 uv = fs_in.pos - 0.5;\n"
    "\n"
    "  float angle = radians(degrees[fs_in.index]);\n"
    "  vec2 rotatedUV = vec2(cos(angle) * uv.x - sin(angle) * uv.y,\n"
    "                        sin(angle) * uv.x + cos(angle) * uv.y) +\n"
    "                   0.5;\n"
    "\n"
    "  float gradientFactor1 = smoothstep(0.0, 0.5, rotatedUV.x);\n"
    "  float gradientFactor2 = smoothstep(0.5, 1.0, rotatedUV.x);\n"
    "\n"
    "  vec4 color1 =\n"
    "      mix(startColor[fs_in.index], midColor[fs_in.index],\n"
    "      gradientFactor1);\n"
    "  vec4 color2 =\n"
    "      mix(midColor[fs_in.index], endColor[fs_in.index],\n"
    "      gradientFactor2);\n"
    "  vec4 finalColor = mix(color1, color2, gradientFactor2);\n"
    "\n"
    "  FragColor =\n"
    "      finalColor * texture(text, vec3(fs_in.texCoords,\n"
    "      letterMap[fs_in.index])).r;\n"
    "}\n";
