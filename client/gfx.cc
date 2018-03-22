#include "gfx.hh"
#include "../common/utils.hh"
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>

static SDL_Window *window;
static SDL_GLContext glcontext;
static int pre_lock_mouse_x, pre_lock_mouse_y, window_width, window_height;
static unsigned long long int frame_idx;

void gfx_init(const char *title, int width, int height) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    die("Failed to initialize SDL: %s", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED
      , SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
  if (!window)
    die("Failed to create window");

  glcontext = SDL_GL_CreateContext(window);

  if (SDL_GL_SetSwapInterval(0) == -1)
    printf("Warning: failed to set vsync: %s\n", SDL_GetError());

  GLenum err = glewInit();
  if (err != GLEW_OK)
    die("Failed to initialze glew: %s", glewGetErrorString(err));
}

void gfx_main_loop(bool *done
    , void (*load_cb)(void)
    , void (*draw_cb)(double)
    , void (*update_cb)(double, double)
    , void (*screen_resize_cb)(int, int)
    , void (*key_event_cb)(unsigned long long, bool)
    , void (*mouse_motion_event_cb)(float, float, int, int)
    , void (*mouse_button_event_cb)(int, bool, int, int)
    , void (*cleanup_cb)(void)) {
  load_cb();

  int w, h;
  SDL_GL_GetDrawableSize(window, &w, &h);
  screen_resize_cb(w, h);

  const int ticks_per_second = 50, max_update_ticks = 15;
  double t = 0, dt = 1. / ticks_per_second
    , current_time = SDL_GetTicks() / 1000., accumulator = 0;

  while (!*done) {
    double real_time = SDL_GetTicks() / 1000.
      , elapsed = real_time - current_time;
    elapsed = std::min(elapsed, max_update_ticks * dt);
    current_time = real_time;
    accumulator += elapsed;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          *done = true;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (event.key.repeat == 0)
            key_event_cb(event.key.keysym.sym, event.type == SDL_KEYDOWN);
          break;
        case SDL_MOUSEMOTION:
          mouse_motion_event_cb(event.motion.xrel, event.motion.yrel,
              event.motion.x, event.motion.y);
          break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          int button;
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:   button = 1; break;
            case SDL_BUTTON_MIDDLE: button = 2; break;
            case SDL_BUTTON_RIGHT:  button = 3; break;
            case SDL_BUTTON_X1:     button = 4; break;
            case SDL_BUTTON_X2:     button = 5; break;
            default:                button = -1;
          }
          mouse_button_event_cb(button, event.type == SDL_MOUSEBUTTONDOWN,
              event.motion.x, event.motion.y);
          break;
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
          screen_resize_cb(event.window.data1, event.window.data2);
          break;
      }
    }

    while (accumulator >= dt) {
      update_cb(dt, t);
      t += dt;
      accumulator -= dt;
    }

    draw_cb(accumulator / dt);

    SDL_GL_SwapWindow(window);

    ++frame_idx;
  }

  cleanup_cb();

  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void gfx_lock_mouse() {
  SDL_GetMouseState(&pre_lock_mouse_x, &pre_lock_mouse_y);
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void gfx_unlock_mouse() {
  SDL_SetRelativeMouseMode(SDL_FALSE);
  SDL_WarpMouseInWindow(window, pre_lock_mouse_x, pre_lock_mouse_y);
}

