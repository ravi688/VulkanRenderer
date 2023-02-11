
#pragma once

#include<renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_camera_system_t vulkan_camera_system_t;
	typedef vulkan_camera_system_t camera_system_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_camera_handle_t camera_handle_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_camera_system_t opengl_camera_system_t;
	typedef opengl_camera_system_t camera_system_t;
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_camera_handle_t camera_handle_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_camera_system_t directx_camera_system_t;
	typedef directx_camera_system_t camera_system_t;
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_camera_handle_t camera_handle_t;
#elif defined(RENDERER_METAL_DRIVER)
	typedef struct metal_camera_system_t metal_camera_system_t;
	typedef metal_camera_system_t camera_system_t;
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_camera_handle_t camera_handle_t;
#endif

#include <renderer/camera.h> 		// camera_projection_type_t

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API camera_system_t* camera_system_new(memory_allocator_t* allocator);
RENDERER_API camera_system_t* camera_system_create(renderer_t* renderer);
RENDERER_API void camera_system_destroy(camera_system_t* system);
RENDERER_API void camera_system_release_resources(camera_system_t* system);

/* getters */
RENDERER_API u32 camera_system_get_count(camera_system_t* system);
RENDERER_API camera_t* camera_system_get_at(camera_system_t* system, buf_ucount_t index);
RENDERER_API camera_t* camera_system_getH(camera_system_t* system, camera_handle_t handle);


/* logic functions */
RENDERER_API camera_handle_t camera_system_create_camera(camera_system_t* system, camera_projection_type_t projection_type);
RENDERER_API void camera_system_destroy_cameraH(camera_system_t* system, camera_handle_t handle);

END_CPP_COMPATIBLE

