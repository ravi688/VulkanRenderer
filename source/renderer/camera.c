
#include <renderer/camera.h>
#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/renderer.h>
#include <renderer/render_pass_pool.h>

#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

/* constructors & destructors */
RENDERER_API camera_t* camera_new()
{
	return vulkan_camera_new();
}

RENDERER_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type)
{
	render_pass_pool_t* pool = renderer_get_render_pass_pool(renderer);
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type),
		.default_render_pass = render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN_CLEAR),
	};
	return vulkan_camera_create(renderer->vulkan_handle, &create_info);
}

RENDERER_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, camera_t OUT camera)
{
	render_pass_pool_t* pool = renderer_get_render_pass_pool(renderer);
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type),
		.default_render_pass = render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN),
	};
	vulkan_camera_create_no_alloc(renderer->vulkan_handle, &create_info, camera);
}

RENDERER_API void camera_destroy(camera_t* camera)
{
	vulkan_camera_destroy(camera);
}

RENDERER_API void camera_release_resources(camera_t* camera)
{
	vulkan_camera_release_resources(camera);
}

/* logic functions */
RENDERER_API void camera_set_clear(camera_t* camera, color_t color, float depth)
{
	vulkan_camera_set_clear(camera, color, depth);
}

RENDERER_API void camera_set_active(camera_t* camera, bool is_active)
{
	vulkan_camera_set_active(camera, is_active);
}

RENDERER_API bool camera_is_active(camera_t* camera)
{
	return vulkan_camera_is_active(camera);
}

RENDERER_API void camera_render(camera_t* camera, render_scene_t* scene)
{
	vulkan_camera_render(camera, scene);
}

RENDERER_API void camera_render_to_texture(camera_t* camera, render_scene_t* scene, texture_t* texture)
{
	vulkan_camera_render_to_texture(camera, scene, VULKAN_RENDER_TARGET_TECHNIQUE_ATTACH, texture);
}

/* getters */

RENDERER_API mat4_t(float) camera_get_view(camera_t* camera)
{
	return vulkan_camera_get_view(camera);
}

RENDERER_API mat4_t(float) camera_get_transform(camera_t* camera)
{
	return vulkan_camera_get_transform(camera);
}

RENDERER_API mat4_t(float) camera_get_projection(camera_t* camera)
{
	return vulkan_camera_get_projection(camera);
}

RENDERER_API vec3_t(float) camera_get_position(camera_t* camera)
{
	return vulkan_camera_get_position(camera);
}

RENDERER_API vec3_t(float) camera_get_rotation(camera_t* camera)
{
	return vulkan_camera_get_rotation(camera);
}

RENDERER_API vec2_t(float) camera_get_clip_planes(camera_t* camera)
{
	return vulkan_camera_get_clip_planes(camera);
}

RENDERER_API float camera_get_near_clip_plane(camera_t* camera)
{
	return vulkan_camera_get_near_clip_plane(camera);
}

RENDERER_API float camera_get_far_clip_plane(camera_t* camera)
{
	return vulkan_camera_get_far_clip_plane(camera);
}

RENDERER_API float camera_get_field_of_view(camera_t* camera)
{
	return vulkan_camera_get_field_of_view(camera);
}

RENDERER_API float camera_get_height(camera_t* camera)
{
	return vulkan_camera_get_height(camera);
}


/* setters */

RENDERER_API void camera_set_transform(camera_t* camera, mat4_t(float) transform)
{
	vulkan_camera_set_transform(camera, transform);
}

RENDERER_API void camera_set_position(camera_t* camera, vec3_t(float) position)
{
	vulkan_camera_set_position(camera, position);
}

RENDERER_API void camera_set_rotation(camera_t* camera, vec3_t(float) rotation)
{
	vulkan_camera_set_rotation(camera, rotation);
}

RENDERER_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	vulkan_camera_set_clip_planes(camera, near_clip_plane, far_clip_plane);
}

RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov)
{
	vulkan_camera_set_field_of_view(camera, fov);
}

RENDERER_API void camera_set_height(camera_t* camera, float height)
{
	vulkan_camera_set_height(camera, height);
}
