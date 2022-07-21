
#pragma once


#include <renderer/defines.h>
#include <renderer/internal/vulkan/vulkan_handles.h>
#include <renderer/internal/vulkan/vulkan_camera.h>

typedef struct vulkan_camera_system_t
{
	vulkan_renderer_t* renderer;
	library_t lib;
} vulkan_camera_system_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API vulkan_camera_system_t* vulkan_camera_system_new();
RENDERER_API vulkan_camera_system_t* vulkan_camera_system_create(vulkan_renderer_t* renderer);
RENDERER_API void vulkan_camera_system_destroy(vulkan_camera_system_t* system);
RENDERER_API void vulkan_camera_system_release_resources(vulkan_camera_system_t* system);

/* getters */
static FORCE_INLINE u32 vulkan_camera_system_get_count(vulkan_camera_system_t* system) 
{
	return library_get_count(&system->lib);
}
static FORCE_INLINE vulkan_camera_t* vulkan_camera_system_get_at(vulkan_camera_system_t* system, buf_ucount_t index)
{
	return library_get_at(&system->lib, index);
}
RENDERER_API vulkan_camera_t* vulkan_camera_system_getH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);


/* logic functions */
RENDERER_API vulkan_camera_handle_t vulkan_camera_system_create_camera(vulkan_camera_system_t* system, vulkan_camera_projection_type_t projection_type);
RENDERER_API void vulkan_camera_system_destroy_cameraH(vulkan_camera_system_t* system, vulkan_camera_handle_t handle);

END_CPP_COMPATIBLE
