
#include <renderer/light.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_light.h>

/* constructors & destructors */
RENDERER_API light_t* light_new()
{
	return vulkan_light_new();
}

RENDERER_API light_t* light_create(renderer_t* renderer, light_type_t type)
{
	return vulkan_light_create(renderer->vulkan_handle, REINTERPRET_TO(vulkan_light_type_t, type));
}

RENDERER_API void light_create_no_alloc(renderer_t* renderer, light_type_t type, light_t OUT light)
{
	return vulkan_light_create_no_alloc(renderer->vulkan_handle, REINTERPRET_TO(vulkan_light_type_t, type), light);
}

RENDERER_API void light_destroy(light_t* light)
{
	vulkan_light_destroy(light);
}

RENDERER_API void light_release_resources(light_t* light)
{
	vulkan_light_release_resources(light);
}

/* setters */
RENDERER_API void light_set_position(light_t* light, vec3_t(float) position)
{
	vulkan_light_set_position(light, position);
}

RENDERER_API void light_set_rotation(light_t* light, vec3_t(float) rotation)
{
	vulkan_light_set_rotation(light, rotation);
}

RENDERER_API void light_set_intensity(light_t* light, float intensity)
{
	vulkan_light_set_intensity(light, intensity);
}

RENDERER_API void light_set_color(light_t* light, vec3_t(float) color)
{
	vulkan_light_set_color(light, color);
}

RENDERER_API void light_set_projection(light_t* light, mat4_t(float) projection)
{
	vulkan_light_set_projection(light, projection);
}

RENDERER_API void light_set_direction(light_t* light, vec3_t(float) dir)
{
	vulkan_light_set_direction(light, dir);
}

