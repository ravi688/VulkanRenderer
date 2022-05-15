
#pragma once

#include <renderer/defines.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef struct light_t light_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API light_t* light_new();
RENDERER_API light_t* light_create(renderer_t* context);
RENDERER_API void light_destroy(light_t* light);
RENDERER_API void light_release_resources(light_t* light);

/* setters */
RENDERER_API void light_set_position(light_t* light, vec3_t(float) position);
RENDERER_API void light_set_rotation(light_t* light, vec3_t(float) rotation);
RENDERER_API void light_set_intensity(light_t* lighit, float intensity);
RENDERER_API void light_set_color(light_t* light, vec3_t(float) color);
RENDERER_API void light_set_projection(light_t* light, mat4_t(float) projection);
RENDERER_API void light_set_direction(light_t* light, vec3_t(float) dir);

END_CPP_COMPATIBLE
