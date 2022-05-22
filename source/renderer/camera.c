
#include <renderer/camera.h>

#include <renderer/internal/vulkan/vulkan_camera.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

/* constructors & destructors */
RENDERER_API camera_t* camera_new()
{
	return vulkan_camera_new();
}

RENDERER_API camera_t* camera_create(renderer_t* renderer, camera_projection_type_t projection_type, float height_or_angle)
{
	return vulkan_camera_create(renderer->vulkan_handle, REINTERPRET_TO(projection_type, vulkan_camera_projection_type_t), height_or_angle);
}

RENDERER_API void camera_create_no_alloc(renderer_t* renderer, camera_projection_type_t projection_type, float height_or_angle, camera_t OUT camera)
{
	vulkan_camera_create_no_alloc(renderer->vulkan_handle REINTERPRET_TO(projection_type, vulkan_camera_projection_type_t), height_or_angle, camera);
}

RENDERER_API void camera_destroy(camera_t* camera)
{
	vulkan_camera_destroy(camera);
}

RENDERER_API void camera_release_resources(camera_t* camera)
{
	vulkan_camera_release_resources(camera);
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

RENDERER_API float camera_get_aspect_ratio(camera_t* camera)
{
	return vulkan_camera_get_aspect_ration(camera);
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
	vulkan_camera_set_rotation(camera, position);
}

RENDERER_API void camera_set_aspect_ratio(camera_t* camera, float aspect_ratio)
{
	vulkan_camera_set_aspect_ratio(camera, aspect_ratio);
}

RENDERER_API void camera_set_clip_planes(camera_t* camera, float near_clip_plane, float far_clip_plane)
{
	vulkan_camera_set_clip_planes(camera, near_clip_plane, far_clip_plane);
}

RENDERER_API void camera_set_field_of_view(camera_t* camera, float fov)
{
	vulkan_camera_set_field_of_view(camera, fov);
}

RENDERER_API void camera_set_height(camera_t* camera)
{
	vulkan_camera_set_height(camera);
}
