
#pragma once

#include <renderer/defines.h>

/* API level object selection */
#ifdef RENDERER_VULKAN_DRIVER
	typedef struct vulkan_renderer_t vulkan_renderer_t;
	typedef vulkan_renderer_t driver_t;
	typedef struct vulkan_render_pass_pool_t vulkan_render_pass_pool_t;
	typedef vulkan_render_pass_pool_t render_pass_pool_t;
	typedef struct vulkan_shader_library_t vulkan_shader_library_t;
	typedef vulkan_shader_library_t shader_library_t;
	typedef struct vulkan_material_library_t vulkan_material_library_t;
	typedef vulkan_material_library_t material_library_t;
#elif RENDERER_OPENGL_DRIVER
	typedef struct opengl_renderer_t opengl_renderer_t;
	typedef opengl_renderer_t driver_t;
	typedef struct opengl_render_pass_pool_t opengl_render_pass_pool_t;
	typedef opengl_render_pass_pool_t render_pass_pool_t;
	typedef struct opengl_shader_library_t opengl_shader_library_t;
	typedef opengl_shader_library_t shader_library_t;
	typedef struct opengl_material_library_t opengl_material_library_t;
	typedef opengl_material_library_t material_library_t;
#elif RENDERER_DIRECTX_DRIVER
	typedef struct directx_renderer_t directx_renderer_t;
	typedef directx_renderer_t driver_t;
	typedef struct directx_render_pass_pool_t directx_render_pass_pool_t;
	typedef directx_render_pass_pool_t render_pass_pool_t;
	typedef struct directx_shader_library_t directx_shader_library_t;
	typedef directx_shader_library_t shader_library_t;
	typedef struct directx_material_library_t directx_material_library_t;
	typedef directx_material_library_t material_library_t;
#elif RENDERER_METAL_DRIVER
	typedef struct metal_renderer_t metal_renderer_t;
	typedef metal_renderer_t driver_t;
	typedef struct metal_render_pass_pool_t metal_render_pass_pool_t;
	typedef metal_render_pass_pool_t render_pass_pool_t;
	typedef struct metal_shader_library_t metal_shader_library_t;
	typedef metal_shader_library_t shader_library_t;
	typedef struct material_shader_library_t material_shader_library_t;
	typedef metal_material_library_t material_library_t;
#endif

typedef struct render_window_t render_window_t;

typedef struct renderer_t
{
	union
	{
		driver_t*
#ifdef RENDERER_VULKAN_DRIVER
		vulkan_driver, *vulkan_handle;
#elif RENDERER_OPENGL_DRIVER
		opengl_driver, *opengl_handle;
#elif RENDERER_DIRECTX_DRIVER
		directx_driver, directx_handle;
#elif RENDERER_METAL_DRIVER
		metal_driver, *metal_handle;
#endif
		driver_t* handle;
		driver_t* driver;
	};
	// shader_library_t* shader_library;
	// material_library_t* material_library;
	// render_pass_pool_t* render_pass_pool;
} renderer_t;

typedef enum renderer_gpu_type_t
{
	RENDERER_GPU_TYPE_AUTO = 0,
	RENDERER_GPU_TYPE_INTEGRATED,
	RENDERER_GPU_TYPE_DISCRETE
} renderer_gpu_type_t;

BEGIN_CPP_COMPATIBLE

RENDERER_API renderer_t* renderer_init(renderer_gpu_type_t gpu_type, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
RENDERER_API void renderer_terminate(renderer_t* renderer);
RENDERER_API void renderer_update(renderer_t* renderer);
RENDERER_API bool renderer_is_running(renderer_t* renderer);

/*
	description:
		begins command buffer recording
	params:
		renderer: ptr to the renderer_t object
	returns:
		nothing
 */
RENDERER_API void renderer_begin_frame(renderer_t* renderer);

/*
	description:
		ends command buffer recording
 */
RENDERER_API void renderer_end_frame(renderer_t* renderer);

/* getters */
RENDERER_API render_window_t* renderer_get_window(renderer_t* renderer);
RENDERER_API shader_library_t* renderer_get_shader_library(renderer_t* renderer);
RENDERER_API material_library_t* renderer_get_material_library(renderer_t* renderer);
RENDERER_API render_pass_pool_t* renderer_get_render_pass_pool(renderer_t* renderer);

END_CPP_COMPATIBLE

#ifdef RENDERER_INCLUDE_EVERYTHING_INTERNAL
#	ifndef RENDERER_INCLUDE_API_INTERNAL
#		define RENDERER_INCLUDE_API_INTERNAL
#	endif
#	ifndef RENDERER_INCLUDE_CORE_INTERNAL
#		define RENDERER_INCLUDE_CORE_INTERNAL
#	endif
#	ifndef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#		define RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#	endif
#	ifndef RENDERER_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#		define RENDERER_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#	endif
#endif

#ifdef RENDERER_INCLUDE_EVERYTHING
#	ifndef RENDERER_INCLUDE_DEBUG
#		define RENDERER_INCLUDE_DEBUG
#	endif
#	ifndef RENDERER_INCLUDE_LOW_LEVEL
#		define RENDERER_INCLUDE_LOW_LEVEL
#	endif
#	ifndef RENDERER_INCLUDE_DATA_STRUCTURES
#		define RENDERER_INCLUDE_DATA_STRUCTURES
#	endif
#	ifndef RENDERER_INCLUDE_CORE
#		define RENDERER_INCLUDE_CORE
#	endif
#	ifndef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	endif
#	ifndef RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#	ifndef RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#		define RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#	endif
#	ifndef RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#		define RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#	endif
#	ifndef RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
#		define RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
#	endif
// #	define RENDERER_INCLUDE_2D_LIGHT_SYSTEM
// #	define RENDERER_INCLUDE_2D_MESH_RENDER_SYSTEM
// #	define RENDERER_INCLUDE_2D_TEXT_RENDER_SYSTEM
#endif

// TODO: abstract memory allocator into the renderer object itself
#	include <renderer/memory_allocator.h>

#ifdef RENDERER_INCLUDE_DEBUG
#	include <renderer/assert.h>
#	include <renderer/debug.h>
#endif

#ifdef RENDERER_INCLUDE_TIMING
#	include <renderer/time.h>
#endif

#ifdef RENDERER_INCLUDE_MATH
#	ifndef RENDERER_INCLUDE_VEC4
#		define RENDERER_INCLUDE_VEC4
#	endif
#	ifndef RENDERER_INCLUDE_VEC3
#		define RENDERER_INCLUDE_VEC3
#	endif
#	ifndef RENDERER_INCLUDE_VEC2
#		define RENDERER_INCLUDE_VEC2
#	endif
#	ifndef RENDERER_INCLUDE_AFFINE_TRANSFORMATION
#		define RENDERER_INCLUDE_AFFINE_TRANSFORMATION
#	endif
#	ifndef RENDERER_INCLUDE_MAT4
#		define RENDERER_INCLUDE_MAT4
#	endif
#endif
	
#ifdef RENDERER_INCLUDE_AFFINE_TRANSFORMATION
#	ifndef RENDERER_INCLUDE_VEC4
#		define RENDERER_INCLUDE_VEC4
#	endif
#	ifndef RENDERER_INCLUDE_MAT4
#		define RENDERER_INCLUDE_MAT4
#	endif
#endif

#ifdef RENDERER_INCLUDE_VEC4
#	include <hpml/vec4/header_config.h>
#	include <hpml/vec4/vec4.h>
#endif

#ifdef RENDERER_INCLUDE_MAT4
#	include <hpml/memory/header_config.h>
#	include <hpml/memory/memory.h>
#	include <hpml/mat4/header_config.h>
#	include <hpml/mat4/mat4.h>
#endif

#ifdef RENDERER_INCLUDE_AFFINE_TRANSFORMATION
#	include <hpml/affine_transformation/header_config.h>
#	include <hpml/affine_transformation/affine_transformation.h>
#endif

#ifdef RENDERER_INCLUDE_VEC3
#	include <hpml/vec3/header_config.h>
#	include <hpml/vec3/vec3.h>
#endif

#ifdef RENDERER_INCLUDE_VEC2
#	include <hpml/vec2/header_config.h>
#	include <hpml/vec2/vec2.h>
#endif

#ifdef RENDERER_INCLUDE_LOW_LEVEL
#	include <renderer/io.h>
#	include <renderer/multi_buffer.h>
#	include <renderer/type_system.h>
#	include <renderer/garbage_collector.h>
#	include <renderer/event_system.h>
# 	include <renderer/struct_descriptor.h>
#endif

#ifdef RENDERER_INCLUDE_API_INTERNAL
#	ifdef RENDERER_VULKAN_DRIVER
#		include <renderer/internal/vulkan/vulkan_instance_buffer.h>
#		include <renderer/internal/vulkan/vulkan_attachment.h>
#		include <renderer/internal/vulkan/vulkan_buffer.h>
#		include <renderer/internal/vulkan/vulkan_defines.h>
#		include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#		include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#		include <renderer/internal/vulkan/vulkan_graphics_pipeline.h>
#		include <renderer/internal/vulkan/vulkan_handles.h>
#		include <renderer/internal/vulkan/vulkan_image.h>
#		include <renderer/internal/vulkan/vulkan_image_view.h>
#		include <renderer/internal/vulkan/vulkan_instance.h>
#		include <renderer/internal/vulkan/vulkan_logical_device.h>
#		include <renderer/internal/vulkan/vulkan_physical_device.h>
#		include <renderer/internal/vulkan/vulkan_pipeline_layout.h>
#		include <renderer/internal/vulkan/vulkan_queue.h>
#		include <renderer/internal/vulkan/vulkan_renderer.h>
#		include <renderer/internal/vulkan/vulkan_result.h>
#		include <renderer/internal/vulkan/vulkan_shader_module.h>
#		include <renderer/internal/vulkan/vulkan_swapchain.h>
#		include <renderer/internal/vulkan/vulkan_texture.h>
#		include <renderer/internal/vulkan/vulkan_to_string.h>
#		include <renderer/internal/vulkan/vulkan_types.h>
#	endif
#endif

#ifdef RENDERER_INCLUDE_DATA_STRUCTURES
#	include <renderer/dictionary.h>
#	include <renderer/string.h>
#  	include <bufferlib/buffer.h>
#endif

#ifdef RENDERER_INCLUDE_CORE
#	include <renderer/render_window.h>
#	include <renderer/render_object.h>
#	include <renderer/render_queue.h>
# 	include <renderer/render_scene.h>
#	include <renderer/render_pass.h>
# 	include <renderer/render_pass_pool.h>
#	include <renderer/shader_library.h>
#	include <renderer/material_library.h>
#	include <renderer/shader.h>
#	include <renderer/material.h>
#	include <renderer/camera.h>
#	include <renderer/color.h>
#	include <renderer/texture.h>
#endif

#ifdef RENDERER_INCLUDE_CORE_INTERNAL
#	ifdef RENDERER_VULKAN_DRIVER
#		include <renderer/internal/vulkan/vulkan_render_object.h>
#		include <renderer/internal/vulkan/vulkan_render_queue.h>
#		include <renderer/internal/vulkan/vulkan_render_pass.h>
# 		include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#		include <renderer/internal/vulkan/vulkan_shader_library.h>
#		include <renderer/internal/vulkan/vulkan_material_library.h>
#		include <renderer/internal/vulkan/vulkan_shader.h>
#		include <renderer/internal/vulkan/vulkan_material.h>
#		include <renderer/internal/vulkan/vulkan_camera.h>
#		include <renderer/internal/vulkan/vulkan_texture.h>
#	endif
#endif

#ifdef RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
//TODO: <renderer/text_mesh3d.h>
#	include <renderer/text_mesh.h>
#	ifndef RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#endif

#ifdef RENDERER_INCLUDE_2D_TEXT_RENDER_SYSTEM
#	include <renderer/text_mesh2d.h>
#	ifndef RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#endif

#ifdef RENDERER_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	include <renderer/glyph_mesh_pool.h>
#	include <renderer/font.h>
#endif

#ifdef RENDERER_INCLUDE_3D_MESH_RENDER_SYSTEM
#	include <renderer/mesh3d.h>
#	ifndef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	endif
#endif

#ifdef RENDERER_INCLUDE_2D_MESH_RENDER_SYSTEM
#	include <renderer/mesh2d.h>
#	ifndef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM	
#	endif
#endif

#ifdef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	include <renderer/mesh.h>
#endif

#ifdef RENDERER_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#	ifdef RENDERER_VULKAN_DRIVER
#		include <renderer/internal/vulkan/vulkan_mesh.h>
#	endif
#endif

#ifdef RENDERER_INCLUDE_3D_LIGHT_SYSTEM
#	include <renderer/light.h>
#endif


#ifdef RENDERER_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#	ifdef RENDERER_VULKAN_DRIVER
#		include <renderer/internal/vulkan/vulkan_light.h>
#	endif
#endif

