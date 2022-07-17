
#pragma once

#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef enum vulkan_light_type_t
{
	VULKAN_LIGHT_TYPE_UNDEFINED = 0,
	VULKAN_LIGHT_TYPE_DIRECTIONAL = 1,
	VULKAN_LIGHT_TYPE_POINT = 2,
	VULKAN_LIGHT_TYPE_OMIN = VULKAN_LIGHT_TYPE_POINT,
	VULKAN_LIGHT_TYPE_SPOT = 3,
	VULKAN_LIGHT_TYPE_AREA = 4,
	VULKAN_LIGHT_TYPE_CURVE = 5,
	VULKAN_LIGHT_TYPE_VOLUME = 6,
	VULKAN_LIGHT_TYPE_AMBIENT = 7
} vulkan_light_type_t;

typedef struct vulkan_light_t
{
	vulkan_renderer_t* renderer;
	vulkan_buffer_t buffer;
	struct_descriptor_t struct_definition;

	/* internal use only, however can be used externally also! */
	vulkan_light_type_t type;

	struct_field_handle_t projection_handle;	 // STRUCT_FIELD_INVALID_HANDLE in case of ambient light
	struct_field_handle_t view_handle; 			 // STRUCT_FIELD_INVALID_HANDLE in case of ambient light
	struct_field_handle_t color_handle;
	struct_field_handle_t intensity_handle;

	// cpu side cache to reduce GPU memory access
	mat4_t(float) projection;
	mat4_t(float) view;
	vec3_t(float) color;
	float intensity;

	vec3_t(float) position;
	mat4_t(float) rotation;
	vec3_t(float) euler_rotation;

} vulkan_light_t;

typedef vulkan_light_t vulkan_ambient_light_t;

typedef struct vulkan_directional_light_t
{
	vulkan_light_t base;
	struct_field_handle_t direction_handle;
	vec3_t(float) direction;
} vulkan_directional_light_t;

typedef struct vulkan_point_light_t
{
	vulkan_light_t base;
	struct_field_handle_t position_handle;
} vulkan_point_light_t;

typedef struct vulkan_spot_light_t
{
	vulkan_light_t base;
	struct_field_handle_t angle_handle;
	struct_field_handle_t direction_handle;
	struct_field_handle_t position_handle;
	float angle;
	vec3_t(float) direction;
} vulkan_spot_light_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_light_t* vulkan_light_new(vulkan_light_type_t type);
RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* renderer, vulkan_light_type_t type);
RENDERER_API void vulkan_light_create_no_alloc(vulkan_renderer_t* renderer, vulkan_light_type_t type, vulkan_light_t OUT light);
RENDERER_API void vulkan_light_destroy(vulkan_light_t* light);
RENDERER_API void vulkan_light_release_resources(vulkan_light_t* light);

/* setters */
RENDERER_API void vulkan_light_set_intensity(vulkan_light_t* light, float intensity);
RENDERER_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t(float) color);
RENDERER_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t(float) position);
RENDERER_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t(float) rotation);
RENDERER_API void vulkan_light_set_spot_angle(vulkan_light_t* light, float angle);


END_CPP_COMPATIBLE
