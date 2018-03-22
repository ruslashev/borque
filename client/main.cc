#include <cstdio>
#include <vector>
#include <GL/glew.h>
#include "gfx.hh"
#include "../common/utils.hh"

static bool g_done = false;
GLuint vao, vbo, vert_shader, frag_shader, shader_program;
GLint resolution_unif, mouse_unif, time_unif;

GLuint compile_shader(GLuint type, const std::string &source) {
  GLuint id = glCreateShader(type);

  const char *source_cstr = source.c_str();
  glShaderSource(id, 1, &source_cstr, nullptr);
  glCompileShader(id);

  GLint compile_status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compile_status);

  if (compile_status)
    return id;

  GLint log_len;
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);

  std::vector<char> info_log(log_len);
  glGetShaderInfoLog(id, log_len, nullptr, &info_log[0]);
  std::string info_log_str(info_log.begin(), info_log.end());

  glDeleteShader(id);

  die("Failed to compile %s shader:\n%s",
      (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", info_log_str.c_str());
}

static void load() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);

  GLfloat screen_quad_vertices[] = {
    -1,  1,  1, -1,  1,  1,
    -1,  1, -1, -1,  1, -1,
  };

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_vertices),
      screen_quad_vertices, GL_STATIC_DRAW);

  vert_shader = compile_shader(GL_VERTEX_SHADER, read_file("vsh.glsl"));
  frag_shader = compile_shader(GL_FRAGMENT_SHADER, read_file("fsh.glsl"));

  shader_program = glCreateProgram();
  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);
  glUseProgram(shader_program);

  GLint pos_attrib = glGetAttribLocation(shader_program, "position");
  glEnableVertexAttribArray(pos_attrib);
  glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

  resolution_unif = glGetUniformLocation(shader_program, "resolution");
  mouse_unif = glGetUniformLocation(shader_program, "mouse");
  time_unif = glGetUniformLocation(shader_program, "time");
}

static void draw(double alpha) {
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

static void update(double dt, double t) {
  glUniform1f(time_unif, t);
}

static void screen_resize(int w, int h) {
  glViewport(0, 0, w, h);
  glUniform2f(resolution_unif, w, h);
}

static void key_event(unsigned long long key, bool down) {
}

static void cleanup() {
  glDeleteProgram(shader_program);
  glDeleteShader(frag_shader);
  glDeleteShader(vert_shader);

  glDeleteBuffers(1, &vbo);

  glDeleteVertexArrays(1, &vao);
}

static void mouse_motion_event(float xrel, float yrel, int x, int y) {

}

static void mouse_button_event(int button, bool down, int xrel, int yrel) {
  glUniform2f(mouse_unif, xrel, yrel);
}

int main() {
  int window_width = 1000, window_height = (double)window_width * (3. / 4.) + 0.5;
  gfx_init("borque", window_width, window_height);

  gfx_main_loop(&g_done, load, draw, update, screen_resize, key_event,
      mouse_motion_event, mouse_button_event, cleanup);
}

