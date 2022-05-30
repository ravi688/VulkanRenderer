
#pragma once

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_light_t vulkan_light_t;
	typedef vulkan_light_t light_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_light_t opengl_light_t;
	typedef opengl_light_t light_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_light_t directx_light_t;
	typedef directx_light_t light_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_light_t metal_light_t;
	typedef metal_light_t light_t;
#endif

#include <renderer/defines.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef enum light_type_t
{
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_POINT = 1,
	LIGHT_TYPE_OMIN = LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT = 2,
	LIGHT_TYPE_AREA = 3,
	LIGHT_TYPE_CURVE = 4,
	LIGHT_TYPE_VOLUME = 5,
	LIGHT_TYPE_AMBIENT = 6
} light_type_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API light_t* light_new();
RENDERER_API light_t* light_create(renderer_t* renderer, light_type_t type);
RENDERER_API void light_create_no_alloc(renderer_t* renderer, light_type_t type, light_t OUT light);
RENDERER_API void light_destroy(light_t* light);
RENDERER_API void light_release_resources(light_t* light);

RENDERER_API void light_set_position(light_t* light, vec3_t(float) position);
RENDERER_API void light_set_rotation(light_t* light, vec3_t(float) rotation);
RENDERER_API void light_set_intensity(light_t* light, float intensity);
RENDERER_API void light_set_color(light_t* light, vec3_t(float) color);
RENDERER_API void light_set_projection(light_t* light, mat4_t(float) projection);
RENDERER_API void light_set_direction(light_t* light, vec3_t(float) dir);

END_CPP_COMPATIBLE