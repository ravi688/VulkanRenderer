
#pragma once

#include <Defines.h>
#include <renderer/renderer.h>
#include <renderer/render_window.h>
#include <ecs/scene_manager.h>


extern renderer_t* RENDERER;
extern render_window_t* WINDOW;
extern scene_manager_t* SCENE_MANAGER;
extern float TIME;
extern float DELTA_TIME;


static ALWAYS_INLINE float __window_height() { return (float)WINDOW->height; }
static ALWAYS_INLINE float __window_width() { return (float)WINDOW->width; }

#define WINDOW_HEIGHT __window_height()
#define WINDOW_WIDTH __window_width()
