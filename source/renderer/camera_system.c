
#include <renderer/camera_system.h>
#include <renderer/internal/vulkan/vulkan_camera_system.h>
#include <renderer/renderer.h>

/* constructors and destructors */
RENDERER_API camera_system_t* camera_system_new(memory_allocator_t* allocator)
{
	return vulkan_camera_system_new(allocator);
}

RENDERER_API camera_system_t* camera_system_create(renderer_t* renderer)
{
	return vulkan_camera_system_create(renderer->vulkan_handle);
}

RENDERER_API void camera_system_destroy(camera_system_t* system)
{
	vulkan_camera_system_destroy(system);
}

RENDERER_API void camera_system_release_resources(camera_system_t* system)
{
	vulkan_camera_system_release_resources(system);
}


/* getters */
RENDERER_API u32 camera_system_get_count(camera_system_t* system)
{
	return camera_system_get_count(system);
}

RENDERER_API camera_t* camera_system_get_at(camera_system_t* system, buf_ucount_t index)
{
	return vulkan_camera_system_get_at(system, index);
}

RENDERER_API camera_t* camera_system_getH(camera_system_t* system, camera_handle_t handle)
{
	return vulkan_camera_system_getH(system, handle);
}



/* logic functions */
RENDERER_API camera_handle_t camera_system_create_camera(camera_system_t* system, camera_projection_type_t projection_type)
{
	return vulkan_camera_system_create_camera(system, REINTERPRET_TO(vulkan_camera_projection_type_t, projection_type));
}

RENDERER_API void camera_system_destroy_cameraH(camera_system_t* system, camera_handle_t handle)
{
	vulkan_camera_system_destroy_cameraH(system, handle);
}

