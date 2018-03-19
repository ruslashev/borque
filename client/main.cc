#include <cstdio>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "gfx.hh"
#include "../3rdparty/imgui/imgui.h"

static bool g_done = false;

static void load() {
  glClearColor(5.f / 255.f, 5.f / 255.f, 5.f / 255.f, 1.f);
}

static void draw() {
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::ShowTestWindow();
}

static void update(double dt, double t) {
}

static void key_event(unsigned long long key, bool down) {
}

static void destroy() {
}

int main() {
  int window_width = 1200, window_height = (double)window_width * (3. / 4.) + 0.5;
  gfx_init("borque", window_width, window_height);

  gfx_main_loop(&g_done, load, draw, update, key_event, destroy);
}

