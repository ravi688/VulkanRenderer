
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

RENDERER_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type, float n_plane, float f_plane, float height_or_angle)
{
	render_pass_pool_t* pool = renderer_get_render_pass_pool(renderer);
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type),
		.field_of_view = height_or_angle,
		.default_render_pass = render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN),
		.near_clip_plane = n_plane,
		.far_clip_plane = f_plane
	};
	return vulkan_camera_create(renderer->vulkan_handle, &create_info);
}

RENDERER_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, float n_plane, float f_plane, float height_or_angle, camera_t OUT camera)
{
	render_pass_pool_t* pool = renderer_get_render_pass_pool(renderer);
	vulkan_camera_create_info_t create_info = 
	{
		.projection_type = REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type),
		.field_of_view = height_or_angle,
		.default_render_pass = render_pass_pool_create_pass_from_preset(pool, RENDER_PASS_POOL_PASS_PRESET_COLOR_SWAPCHAIN),
		.near_clip_plane = n_plane,
		.far_clip_plane = f_plane
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
RENDERER_API void camera_render(camera_t* camera, render_queue_t* queue)
{
	vulkan_camera_render(camera, queue);
}

/* getters */
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
