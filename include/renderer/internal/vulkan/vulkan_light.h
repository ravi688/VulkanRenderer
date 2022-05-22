
#pragma once

#include <renderer/defines.h>
#include <renderer/struct_descriptor.h>
#include <renderer/internal/vulkan/vulkan_buffer.h>

#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>

typedef enum vulkan_light_type_t
{
	VULKAN_LIGHT_TYPE_DIRECTIONAL = 0,
	VULKAN_LIGHT_TYPE_POINT = 1,
	VULKAN_LIGHT_TYPE_OMIN = VULKAN_LIGHT_TYPE_POINT,
	VULKAN_LIGHT_TYPE_SPOT = 2,
	VULKAN_LIGHT_TYPE_AREA = 3,
	VULKAN_LIGHT_TYPE_CURVE = 4,
	VULKAN_LIGHT_TYPE_VOLUME = 5,
	VULKAN_LIGHT_TYPE_AMBIENT = 6
} vulkan_light_type_t;

typedef struct vulkan_light_t
{
	vulkan_renderer_t* renderer;
	// vulkan_light_data_t data;
	vulkan_buffer_t buffer;
	struct_descriptor_t struct_definition;

	struct_field_handle_t transform_handle;
	struct_field_handle_t projection_handle;
	struct_field_handle_t view_handle;
	struct_field_handle_t dir_handle;
	struct_field_handle_t color_handle;
	struct_field_handle_t intensity_handle;
} vulkan_light_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_light_t* vulkan_light_new();
RENDERER_API vulkan_light_t* vulkan_light_create(vulkan_renderer_t* rendere, vulkan_light_type_t type);
RENDERER_API void vulkan_light_create(vulkan_renderer_t* rendere, vulkan_light_type_t type, vulkan_light_t OUT light);
RENDERER_API void vulkan_light_destroy(vulkan_light_t* light);
RENDERER_API void vulkan_light_release_resources(vulkan_light_t* light);

/* setters */
RENDERER_API void vulkan_light_set_position(vulkan_light_t* light, vec3_t(float) position);
RENDERER_API void vulkan_light_set_rotation(vulkan_light_t* light, vec3_t(float) rotation);
RENDERER_API void vulkan_light_set_intensity(vulkan_light_t* lighit, float intensity);
RENDERER_API void vulkan_light_set_color(vulkan_light_t* light, vec3_t(float) color);
RENDERER_API void vulkan_light_set_projection(vulkan_light_t* light, mat4_t(float) projection);
RENDERER_API void vulkan_light_set_direction(vulkan_light_t* light, vec3_t(float) dir);

END_CPP_COMPATIBLE
