
#pragma once

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
#	include <hpml/vec4.h>
#endif

#ifdef RENDERER_INCLUDE_MAT4
#	include <hpml/mat4.h>
#endif

#ifdef RENDERER_INCLUDE_AFFINE_TRANSFORMATION
#	include <hpml/affine_transformation.h>
#endif

#ifdef RENDERER_INCLUDE_VEC3
#	include <hpml/vec3.h>
#endif

#ifdef RENDERER_INCLUDE_VEC2
#	include <hpml/vec2.h>
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
#	include <renderer/camera_system.h>
#	include <renderer/shader.h>
#	include <renderer/material.h>
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

