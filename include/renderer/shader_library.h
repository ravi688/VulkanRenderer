
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_shader_library.h>
	typedef vulkan_shader_library_t shader_library_t;
	typedef vulkan_shader_t shader_t;
	typedef vulkan_shader_create_info_t shader_create_info_t;
	typedef vulkan_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID VULKAN_SHADER_HANDLE_INVALID
#elif defined(RENDERER_OPENGL_DRIVER)
	#include <renderer/internal/opengl/opengl_shader_library.h>
	typedef opengl_shader_library_t shader_library_t;
	typedef opengl_shader_t shader_t;
	typedef opengl_shader_create_info_t shader_create_info_t;
	typedef opengl_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID OPENGL_SHADER_HANDLE_INVALID
#elif defined(RENDERER_DIRECTX_DRIVER)
	#include <renderer/internal/directx/directx_shader_library.h>
	typedef directx_shader_library_t shader_library_t;
	typedef directx_shader_t shader_t;
	typedef directx_shader_create_info_t shader_create_info_t;
	typedef directx_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID DIRECTX_SHADER_HANDLE_INVALID
#elif defined(RENDERER_METAL_DRIVER)
	#include <renderer/internal/metal/metal_shader_library.h>
	typedef metal_shader_library_t shader_library_t;
	typedef metal_shader_t shader_t;
	typedef metal_shader_create_info_t shader_create_info_t;
	typedef metal_shader_handle_t shader_handle_t;
	#define SHADER_HANDLE_INVALID METAL_SHADER_HANDLE_INVALID
#endif

typedef enum shader_library_shader_preset_t
{
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

/* constructors & destructors */
RENDERER_API shader_library_t* shader_library_new();
RENDERER_API shader_library_t* shader_library_create(renderer_t* renderer);
RENDERER_API shader_library_t* shader_library_load_folder(renderer_t* renderer, const char* folder_path);
RENDERER_API void shader_library_destroy(shader_library_t* library);
RENDERER_API void shader_library_release_resources(shader_library_t* library);

/* logic functions */

RENDERER_API shader_handle_t shader_library_create_shader_from_preset(shader_library_t* library, shader_library_shader_preset_t preset);

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
RENDERER_API shader_handle_t shader_library_create_shader(shader_library_t* library, shader_create_info_t* create_info, const char* shader_name);

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
RENDERER_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path);

/*
	description: destroys a shader with identification name as 'shader_name'
	params:
		library: ptr to the shader_library_t object
		shader_name: name of the shader in the library
	returns:
		true, if shader destruction succeed
		false, otherwise
 */
RENDERER_API bool shader_library_destroy_shader(shader_library_t* library, const char* shader_name);

/*
	description: destroys a shader with handle as 'handle'
	params:
		library: ptr to the shader_library_t object
		handle: handle to the shader in the library
	returns:
		true, if the shader destruction succeed
		false, otherwise
 */
RENDERER_API bool shader_library_destroy_shaderH(shader_library_t* library, shader_handle_t handle);

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
RENDERER_API shader_handle_t shader_library_get_handle(shader_library_t* library, const char* shader_name);

/*
	description: returns the name of the shader slot with handle 'handle' in the library
	params:
		library: ptr to the shader_library_t object
		handle: handle to the shader slot in the library
	returns:
		const char*, name of the shader slot in the library
		NULL, if the shader slot isn't found with handle 'handle'
 */
RENDERER_API const char* shader_library_get_nameH(shader_library_t* library, shader_handle_t handle);

/*
	description: returns the name of the shader slot with ptr to the shader_t as 'shader'
	params:	
		library: ptr to the shader_library_t object
		shader: ptr to the shader_t object
	returns:
		const char*, name of the shader slot in the library
		NULL, if the shader slot isn't found with ptr as 'shader'
 */

RENDERER_API shader_t* shader_library_getH(shader_library_t* library, shader_handle_t handle);

/*
	description: returns ptr to the shader_t object in the library with name as 'shader_name'
	params:
		library: ptr to the shader_library_t object
		shader_name: name of the shader in the library
	returns:
		shader_t*, ptr to the shader_t object
		NULL, if the shader isn't found
 */
RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name);
