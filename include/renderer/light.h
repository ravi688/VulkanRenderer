
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
#include <hpml/vec3.h>
#include <hpml/mat4.h>

typedef enum light_type_t
{
	LIGHT_TYPE_UNDEFINED = 0,
	LIGHT_TYPE_DIRECTIONAL = 1,
	LIGHT_TYPE_POINT = 2,
	LIGHT_TYPE_OMIN = LIGHT_TYPE_POINT,
	LIGHT_TYPE_SPOT = 3,
	LIGHT_TYPE_AREA = 4,
	LIGHT_TYPE_CURVE = 5,
	LIGHT_TYPE_VOLUME = 6,
	LIGHT_TYPE_AMBIENT = 7
} light_type_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API light_t* light_new(memory_allocator_t* allocator, light_type_t type);
RENDERER_API light_t* light_create(renderer_t* renderer, light_type_t type);
RENDERER_API void light_create_no_alloc(renderer_t* renderer, light_type_t type, light_t OUT light);
RENDERER_API void light_destroy(light_t* light);
RENDERER_API void light_release_resources(light_t* light);

RENDERER_API void light_set_spot_angle(light_t* light, float angle);
RENDERER_API void light_set_position(light_t* light, vec3_t position);
RENDERER_API void light_set_rotation(light_t* light, vec3_t rotation);
RENDERER_API void light_set_intensity(light_t* light, float intensity);
RENDERER_API void light_set_color(light_t* light, vec3_t color);

END_CPP_COMPATIBLE
