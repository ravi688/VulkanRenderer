/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: include_helper.h is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


#pragma once

#ifdef SGE_INCLUDE_EVERYTHING_INTERNAL
#	ifndef SGE_INCLUDE_API_INTERNAL
#		define SGE_INCLUDE_API_INTERNAL
#	endif
#	ifndef SGE_INCLUDE_CORE_INTERNAL
#		define SGE_INCLUDE_CORE_INTERNAL
#	endif
#	ifndef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#		define SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#	endif
#	ifndef SGE_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#		define SGE_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#	endif
#endif

#ifdef SGE_INCLUDE_EVERYTHING
#	ifndef SGE_INCLUDE_DEBUG
#		define SGE_INCLUDE_DEBUG
#	endif
#	ifndef SGE_INCLUDE_LOW_LEVEL
#		define SGE_INCLUDE_LOW_LEVEL
#	endif
#	ifndef SGE_INCLUDE_DATA_STRUCTURES
#		define SGE_INCLUDE_DATA_STRUCTURES
#	endif
#   ifndef SGE_INCLUDE_MATH
#       define SGE_INCLUDE_MATH
#   endif
#	ifndef SGE_INCLUDE_CORE
#		define SGE_INCLUDE_CORE
#	endif
#	ifndef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	endif
#	ifndef SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#	ifndef SGE_INCLUDE_3D_LIGHT_SYSTEM
#		define SGE_INCLUDE_3D_LIGHT_SYSTEM
#	endif
#	ifndef SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#		define SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#	endif
#	ifndef SGE_INCLUDE_3D_TEXT_RENDER_SYSTEM
#		define SGE_INCLUDE_3D_TEXT_RENDER_SYSTEM
#	endif
// #	define SGE_INCLUDE_2D_LIGHT_SYSTEM
// #	define SGE_INCLUDE_2D_MESH_RENDER_SYSTEM
// #	define SGE_INCLUDE_2D_TEXT_RENDER_SYSTEM
#endif

// TODO: abstract memory allocator into the renderer object itself
#	include <sge/memory_allocator.h>

#ifdef SGE_INCLUDE_DEBUG
#	include <sge/assert.h>
#	include <sge/debug.h>
#endif

#ifdef SGE_INCLUDE_TIMING
#	include <sge/time.h>
#endif

#ifdef SGE_INCLUDE_MATH
#	ifndef SGE_INCLUDE_VEC4
#		define SGE_INCLUDE_VEC4
#	endif
#	ifndef SGE_INCLUDE_VEC3
#		define SGE_INCLUDE_VEC3
#	endif
#	ifndef SGE_INCLUDE_VEC2
#		define SGE_INCLUDE_VEC2
#	endif
#	ifndef SGE_INCLUDE_AFFINE_TRANSFORMATION
#		define SGE_INCLUDE_AFFINE_TRANSFORMATION
#	endif
#	ifndef SGE_INCLUDE_MAT4
#		define SGE_INCLUDE_MAT4
#	endif
#endif

#ifdef SGE_INCLUDE_AFFINE_TRANSFORMATION
#	ifndef SGE_INCLUDE_VEC4
#		define SGE_INCLUDE_VEC4
#	endif
#	ifndef SGE_INCLUDE_MAT4
#		define SGE_INCLUDE_MAT4
#	endif
#endif

#ifdef SGE_INCLUDE_VEC4
#	include <hpml/vec4.h>
#endif

#ifdef SGE_INCLUDE_MAT4
#	include <hpml/mat4.h>
#endif

#ifdef SGE_INCLUDE_AFFINE_TRANSFORMATION
#	include <hpml/affine_transformation.h>
#endif

#ifdef SGE_INCLUDE_VEC3
#	include <hpml/vec3.h>
#endif

#ifdef SGE_INCLUDE_VEC2
#	include <hpml/vec2.h>
#endif

#ifdef SGE_INCLUDE_LOW_LEVEL
#	include <sge/io.h>
#	include <sge/system/display.h>
#	include <sge/type_system.h>
#	include <sge/garbage_collector.h>
# 	include <sge/struct_descriptor.h>
#endif

#ifdef SGE_INCLUDE_API_INTERNAL
#	ifdef SGE_VULKAN_DRIVER
#		include <sge/internal/vulkan/vulkan_instance_buffer.h>
#		include <sge/internal/vulkan/vulkan_attachment.h>
#		include <sge/internal/vulkan/vulkan_buffer.h>
#		include <sge/internal/vulkan/vulkan_defines.h>
#		include <sge/internal/vulkan/vulkan_descriptor_set.h>
#		include <sge/internal/vulkan/vulkan_descriptor_set_layout.h>
#		include <sge/internal/vulkan/vulkan_graphics_pipeline.h>
#		include <sge/internal/vulkan/vulkan_handles.h>
#		include <sge/internal/vulkan/vulkan_image.h>
#		include <sge/internal/vulkan/vulkan_image_view.h>
#		include <sge/internal/vulkan/vulkan_instance.h>
#		include <sge/internal/vulkan/vulkan_logical_device.h>
#		include <sge/internal/vulkan/vulkan_physical_device.h>
#		include <sge/internal/vulkan/vulkan_pipeline_layout.h>
#		include <sge/internal/vulkan/vulkan_queue.h>
#		include <sge/internal/vulkan/vulkan_renderer.h>
#		include <sge/internal/vulkan/vulkan_result.h>
#		include <sge/internal/vulkan/vulkan_shader_module.h>
#		include <sge/internal/vulkan/vulkan_swapchain.h>
#		include <sge/internal/vulkan/vulkan_texture.h>
#		include <sge/internal/vulkan/vulkan_bitmap_glyph_atlas_texture.h>
#		include <sge/internal/vulkan/vulkan_host_buffered_buffer.h>
#		include <sge/internal/vulkan/vulkan_render_texture.h>
#		include <sge/internal/vulkan/vulkan_to_string.h>
#		include <sge/internal/vulkan/vulkan_types.h>
#	endif
#endif

#ifdef SGE_INCLUDE_DATA_STRUCTURES
#	include <common/dictionary.h>
#	include <sge/string.h>
#	include <common/hash_table.h>
#	include <sge/buffer2d_view.h>
#	include <sge/buffer2d.h>
# 	include <common/multi_buffer.h>
#   include <sge/string_builder.h>
#  	include <bufferlib/buffer.h>
#endif

#ifdef SGE_INCLUDE_CORE
#	include <sge/render_window.h>
#	include <sge/render_object.h>
#	include <sge/render_queue.h>
# 	include <sge/render_scene.h>
#	include <sge/render_pass.h>
# 	include <sge/render_pass_pool.h>
#	include <sge/shader_library.h>
#	include <sge/material_library.h>
#	include <sge/camera_system.h>
#	include <sge/shader.h>
#	include <sge/material.h>
#	include <sge/color.h>
#	include <sge/texture.h>
#	include <sge/render_texture.h>
#endif

#ifdef SGE_INCLUDE_CORE_INTERNAL
#	ifdef SGE_VULKAN_DRIVER
#		include <sge/internal/vulkan/vulkan_render_object.h>
#		include <sge/internal/vulkan/vulkan_render_queue.h>
#		include <sge/internal/vulkan/vulkan_render_pass.h>
# 		include <sge/internal/vulkan/vulkan_render_pass_pool.h>
#		include <sge/internal/vulkan/vulkan_shader_library.h>
#		include <sge/internal/vulkan/vulkan_material_library.h>
#		include <sge/internal/vulkan/vulkan_shader.h>
#		include <sge/internal/vulkan/vulkan_material.h>
#		include <sge/internal/vulkan/vulkan_camera.h>
#		include <sge/internal/vulkan/vulkan_texture.h>
#		include <sge/internal/vulkan/vulkan_render_texture.h>
#	endif
#endif

#ifdef SGE_INCLUDE_3D_TEXT_RENDER_SYSTEM
#	include <sge/text_mesh.h>
#	ifndef SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#endif

#ifdef SGE_INCLUDE_2D_TEXT_RENDER_SYSTEM
#	include <sge/text_mesh2d.h>
#	ifndef SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	endif
#endif

#ifdef SGE_INCLUDE_CORE_TEXT_RENDER_SYSTEM
#	include <sge/glyph_mesh_pool.h>
#	include <sge/font.h>
#endif

#ifdef SGE_INCLUDE_3D_MESH_RENDER_SYSTEM
#	include <sge/mesh3d.h>
#	ifndef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	endif
#endif

#ifdef SGE_INCLUDE_2D_MESH_RENDER_SYSTEM
#	include <sge/mesh2d.h>
#	ifndef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#		define SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	endif
#endif

#ifdef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM
#	include <sge/mesh.h>
#endif

#ifdef SGE_INCLUDE_CORE_MESH_RENDER_SYSTEM_INTERNAL
#	ifdef SGE_VULKAN_DRIVER
#		include <sge/internal/vulkan/vulkan_mesh.h>
#	endif
#endif

#ifdef SGE_INCLUDE_3D_LIGHT_SYSTEM
#	include <sge/light.h>
#endif


#ifdef SGE_INCLUDE_3D_LIGHT_SYSTEM_INTERNAL
#	ifdef SGE_VULKAN_DRIVER
#		include <sge/internal/vulkan/vulkan_light.h>
#	endif
#endif
