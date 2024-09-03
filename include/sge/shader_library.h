/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: shader_library.h is a part of VulkanRenderer

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

#include <sge/defines.h>

#ifdef SGE_VULKAN_DRIVER
	#include <sge/internal/vulkan/vulkan_shader_library.h>
	typedef vulkan_shader_library_t shader_library_t;
	#define SHADER_HANDLE_INVALID VULKAN_SHADER_HANDLE_INVALID
#elif defined(SGE_OPENGL_DRIVER)
	typedef struct opengl_shader_library_t opengl_shader_library_t;
	typedef opengl_shader_library_t shader_library_t;
#elif defined(SGE_DIRECTX_DRIVER)
	typedef struct directx_shader_library_t directx_shader_library_t;
	typedef directx_shader_library_t shader_library_t;
#elif defined(SGE_METAL_DRIVER)
	typedef struct metal_shader_library_t metal_shader_library_t;
	typedef metal_shader_library_t shader_library_t;
#endif

#include <sge/shader.h> 		// for shader_handle_t, shader_t*, shader_create_info_t etc.

typedef enum shader_library_shader_preset_t
{
	SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT,
	SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_VERTEX,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR,
	SHADER_LIBRARY_SHADER_PRESET_SKYBOX,
	SHADER_LIBRARY_SHADER_PRESET_REFLECTION,
	SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH,
	SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TEXTURE,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TRANSPARENT,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TRANSPARENT_CUTOUT,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_VERTEX_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TEXTURE_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TRANSPARENT_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_UNLIT_TRANSPARENT_CUTOUT_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_VERTEX,
	SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TEXTURE,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TRANSPARENT,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TRANSPARENT_CUTOUT,
	SHADER_LIBRARY_SHADER_PRESET_LIT_VERTEX_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TEXTURE_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TRANSPARENT_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_TRANSPARENT_CUTOUT_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR,
	SHADER_LIBRARY_SHADER_PRESET_DIFFUSE,
	SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST,
	SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT,
	SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP,
	SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT,
	SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT,
	SHADER_LIBRARY_SHADER_PRESET_SPECULAR,
	SHADER_LIBRARY_SHADER_PRESET_BUMPED_DIFFUSE,
	SHADER_LIBRARY_SHADER_PRESET_BUMPED_SPECULAR,
	SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_SPECULAR_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_BUMPED_DIFFUSE_OVERLAY,
	SHADER_LIBRARY_SHADER_PRESET_BUMPED_SPECULAR_OVERLAY
} shader_library_shader_preset_t;

BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
SGE_API shader_library_t* shader_library_new(memory_allocator_t* allocator);
SGE_API shader_library_t* shader_library_create(renderer_t* renderer);
SGE_API shader_library_t* shader_library_load_folder(renderer_t* renderer, const char* folder_path);
SGE_API void shader_library_destroy(shader_library_t* library);
SGE_API void shader_library_release_resources(shader_library_t* library);

/* logic functions */

SGE_API shader_handle_t shader_library_create_shader_from_preset(shader_library_t* library, shader_library_shader_preset_t preset);

/*
	description: creates a shader from bytes with identification name 'shader_name'
	params:
		library: ptr to the shader_library_t object
		sb: ptr to shader binary bytes
		length: length of the shader binary
		shader_name: identification name of the created shader
	returns:
		shader_handle_t, handle to the newly created shader
		SHADER_HANDLE_INVALID, if the shader creation failed
 */
SGE_API shader_handle_t shader_library_create_shader(shader_library_t* library, shader_create_info_t* create_info, const char* shader_name);

/*
	description: loads a file and creates a shader from that with identification name 'shader_name'
	params:
		library: ptr to the shader_library_t object
		file_path: file path to load from
		shader_name: identification name of the created shader
	returns:
		shader_handle_t, handle to the newly created shader
		SHADER_HANDLE_INVALID, if the shader creation failed
 */
SGE_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path);

/*
	description: compiles .v3dshader source and creates a shader from that with identification name 'shader_name'
	params:
		library: ptr to the shader_library_t object
		source: file path to load from
		shader_name: identification name of the created shader
	returns:
		shader_handle_t, handle to the newly created shader
		SHADER_HANDLE_INVALID, if the shader creation failed
 */
SGE_API shader_handle_t shader_library_compile_and_load_shader(shader_library_t* library, const char* source, const char* shader_name);

/*
	description: destroys a shader with identification name as 'shader_name'
	params:
		library: ptr to the shader_library_t object
		shader_name: name of the shader in the library
	returns:
		true, if shader destruction succeed
		false, otherwise
 */
SGE_API bool shader_library_destroy_shader(shader_library_t* library, const char* shader_name);

/*
	description: destroys a shader with handle as 'handle'
	params:
		library: ptr to the shader_library_t object
		handle: handle to the shader in the library
	returns:
		true, if the shader destruction succeed
		false, otherwise
 */
SGE_API bool shader_library_destroy_shaderH(shader_library_t* library, shader_handle_t handle);

/* getters */

/*
	description: returns the handle to the shader slot with name 'shader_name' in the library
	params:
		library: ptr to the shader_library_t object
		shader_name: name of the shader in the library
	returns:
		shader_handle_t, handle to the shader slot
		SHADER_HANDLE_INVALID, if the shader slot with name 'shader_name' isn't found
 */
SGE_API shader_handle_t shader_library_get_handle(shader_library_t* library, const char* shader_name);

/*
	description: returns the name of the shader slot with handle 'handle' in the library
	params:
		library: ptr to the shader_library_t object
		handle: handle to the shader slot in the library
	returns:
		const char*, name of the shader slot in the library
		NULL, if the shader slot isn't found with handle 'handle'
 */
SGE_API const char* shader_library_get_nameH(shader_library_t* library, shader_handle_t handle);

/*
	description: returns the name of the shader slot with ptr to the shader_t as 'shader'
	params:
		library: ptr to the shader_library_t object
		shader: ptr to the shader_t object
	returns:
		const char*, name of the shader slot in the library
		NULL, if the shader slot isn't found with ptr as 'shader'
 */

SGE_API shader_t* shader_library_getH(shader_library_t* library, shader_handle_t handle);

/*
	description: returns ptr to the shader_t object in the library with name as 'shader_name'
	params:
		library: ptr to the shader_library_t object
		shader_name: name of the shader in the library
	returns:
		shader_t*, ptr to the shader_t object
		NULL, if the shader isn't found
 */
SGE_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name);

END_CPP_COMPATIBLE
