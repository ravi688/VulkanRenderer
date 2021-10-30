
#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <stdint.h>
#include <stdbool.h>

#include <vec3/header_config.h>
#include <vec3/vec3.h>
#include <vec2/header_config.h>
#include <vec2/vec2.h>
#include <engine/mesh.h>
#include <engine/mesh_renderer.h>
#include <engine/mesh_filter.h>
#include <engine/material.h>
#include <engine/color.h>
#include <engine/material.h>
#include <engine/component.h>
#include <engine/object.h>
#include <engine/scene.h>
#include <engine/scene_manager.h>
#include <engine/renderer/renderer.h>


typedef struct engine_t engine_t;

engine_t* engine_init(uint32_t screen_width, uint32_t screen_height, const char* window_name);
void engine_terminate(engine_t* engine); 
bool engine_is_running(engine_t* engine);
void engine_poll_events(engine_t* engine);

void engine_start(engine_t* engine); 
void engine_update(engine_t* engine); 



void* engine_get_window(engine_t* engine);
scene_manager_t* engine_get_scene_manager(engine_t* engine); 
renderer_t* engine_get_renderer(engine_t* engine);


#endif/*__ENGINE_H__*/