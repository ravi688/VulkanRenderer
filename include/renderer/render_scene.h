
#pragma once


#include <renderer/defines.h>

#if 	defined(RENDERER_VULKAN_DRIVER)
	typedef struct vulkan_render_scene_t vulkan_render_scene_t;
	typedef vulkan_render_scene_t render_scene_t;
	#include <renderer/internal/vulkan/vulkan_handles.h>
	typedef vulkan_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID VULKAN_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/vulkan/vulkan_render_scene.h>
	#define RENDER_SCENE_CLEAR VULKAN_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE VULKAN_RENDER_SCENE_DONT_CARE

#elif 	defined(RENDERER_OPENGL_DRIVER)
	typedef struct opengl_render_scene_t opengl_render_scene_t;
	typedef opengl_render_scene_t render_scene_t;
	#include <renderer/internal/opengl/opengl_handles.h>
	typedef opengl_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID OPENGL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/opengl/opengl_render_scene.h>
	#define RENDER_SCENE_CLEAR OPENGL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE OPENGL_RENDER_SCENE_DONT_CARE

#elif  	defined(RENDERER_DIRECTX_DRIVER)
	typedef struct directx_render_scene_t directx_render_scene_t;
	typedef directx_render_scene_t render_scene_t;
	#include <renderer/internal/directx/directx_handles.h>
	typedef directx_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID DIRECTX_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/directx/directx_render_scene.h>
	#define RENDER_SCENE_CLEAR DIRECTX_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE DIRECTX_RENDER_SCENE_DONT_CARE

#elif  	defined(RENDERER_METAL_DRIVER)
	typedef struct metal_render_scene_t metal_render_scene_t;
	typedef metal_render_scene_t render_scene_t;
	#include <renderer/internal/metal/metal_handles.h>
	typedef metal_render_scene_object_handle_t render_scene_object_handle_t;
	#define RENDER_SCENE_OBJECT_HANDLE_INVALID METAL_RENDER_SCENE_OBJECT_HANDLE_INVALID

	#include <renderer/internal/metal/metal_render_scene.h>
	#define RENDER_SCENE_CLEAR METAL_RENDER_SCENE_CLEAR
	#define RENDER_SCENE_DONT_CARE METAL_RENDER_SCENE_DONT_CARE

#endif

#include <renderer/render_object.h>
#include <renderer/render_queue.h>

typedef enum render_scene_preset_type_t
{
	RENDER_SCENE_PRESET_TYPE_DEFAULT = 0,
	RENDER_SCENE_PRESET_TYPE_GEOMETRY_ONLY,
	RENDER_SCENE_PRESET_TYPE_ALPHA_TESTED_ONLY,
	RENDER_SCENE_PRESET_TYPE_OVERLAY_ONLY,
	RENDER_SCENE_PRESET_TYPE_TRANSPARENT_ONLY,
} render_scene_preset_type_t;

typedef struct render_scene_create_info_t
{
	render_queue_type_t* required_queues;
	u32 required_queue_count;
} render_scene_create_info_t;

/* constructors & destructors */
RENDERER_API render_scene_t* render_scene_new();
RENDERER_API render_scene_t* render_scene_create(renderer_t* renderer, render_scene_create_info_t* create_info);
RENDERER_API void render_scene_create_no_alloc(renderer_t* renderer, render_scene_create_info_t* create_info, render_scene_t OUT scene);
RENDERER_API render_scene_t* render_scene_create_from_preset(renderer_t* renderer, render_scene_preset_type_t type);
RENDERER_API render_scene_t* render_scene_create_from_mask(renderer_t* renderer, u64 queue_mask);
RENDERER_API void render_scene_destroy(render_scene_t* scene);
RENDERER_API void render_scene_release_resources(render_scene_t* scene);

/* logic functions */
RENDERER_API void render_scene_add_queue(render_scene_t* scene, render_queue_type_t queue_type);
RENDERER_API void render_scene_render(render_scene_t* scene, u64 queue_mask, u32 flags);
RENDERER_API render_object_t* render_scene_getH(render_scene_t* scene, render_scene_object_handle_t handle);
RENDERER_API render_scene_object_handle_t render_scene_create_object(render_scene_t* scene, render_object_type_t object_type, render_queue_type_t queue_type);
RENDERER_API void render_scene_destroy_objectH(render_scene_t* scene, render_scene_object_handle_t handle);
