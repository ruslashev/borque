#pragma once

void gfx_init(const char *title, int width, int height);
void gfx_main_loop(bool *done
    , void (*load_cb)(void)
    , void (*draw_cb)(double)
    , void (*update_cb)(double, double)
    , void (*screen_resize_cb)(int, int)
    , void (*key_event_cb)(unsigned long long, bool)
    , void (*mouse_motion_event_cb)(float, float, int, int)
    , void (*mouse_button_event_cb)(int, bool, int, int)
    , void (*cleanup_cb)(void));
void gfx_lock_mouse();
void gfx_unlock_mouse();

