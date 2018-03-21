#include <cstdio>
#include <GL/glew.h>
#include "gfx.hh"

static bool g_done = false;
GLuint vao, vbo, vertexShader, fragmentShader, shaderProgram;

static void load() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  const char *vert_shader_src =
    "#version 120 \n"
    "attribute vec2 position; \n"
    "void main() { \n"
    "  gl_Position = vec4(position, 0.0, 1.0); \n"
    "} \n";
  const char *frag_shader_src =
    "#version 120 \n"
    "void main() { \n"
    "  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
    "} \n";

  glGenBuffers(1, &vbo);

  GLfloat vertices[] = {
     0.0f,  0.5f,
     0.5f, -0.5f,
    -0.5f, -0.5f
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vert_shader_src, NULL);
  glCompileShader(vertexShader);

  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &frag_shader_src, NULL);
  glCompileShader(fragmentShader);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

static void draw() {
  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void update(double dt, double t) {
}

static void key_event(unsigned long long key, bool down) {
}

static void destroy() {
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);

  glDeleteBuffers(1, &vbo);

  glDeleteVertexArrays(1, &vao);
}

int main() {
  int window_width = 1200, window_height = (double)window_width * (3. / 4.) + 0.5;
  gfx_init("borque", window_width, window_height);

  gfx_main_loop(&g_done, load, draw, update, key_event, destroy);
}

