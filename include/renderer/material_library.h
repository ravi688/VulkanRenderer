
#pragma once

#include <renderer/defines.h>

#ifdef RENDERER_VULKAN_DRIVER
	#include <renderer/internal/vulkan/vulkan_material_library.h>
	typedef vulkan_material_library_t material_library_t;
	typedef vulkan_material_t material_t;
	typedef vulkan_material_handle_t material_handle_t;
	#define MATERIAL_HANDLE_INVALID VULKAN_MATERIAL_HANDLE_INVALID
	typedef struct vulkan_shader_t vulkan_shader_t;
	typedef vulkan_shader_t shader_t;
	typedef vulkan_shader_library_t shader_library_t;
	typedef vulkan_shader_handle_t shader_handle_t;
#elif defined(RENDERER_OPENGL_DRIVER)
	#include <renderer/internal/opengl/opengl_material_library.h>
	typedef opengl_material_library_t material_library_t;
	typedef opengl_material_t material_t;
	typedef opengl_material_handle_t material_handle_t;
	#define MATERIAL_HANDLE_INVALID OPENGL_MATERIAL_HANDLE_INVALID
	typedef struct opengl_shader_t opengl_shader_t;
	typedef opengl_shader_t shader_t;
	typedef opengl_shader_library_t shader_library_t;
	typedef opengl_shader_handle_t shader_handle_t;
#elif defined(RENDERER_DIRECTX_DRIVER)
	#include <renderer/internal/directx/directx_material_library.h>
	typedef directx_material_library_t material_library_t;
	typedef directx_material_t material_t;
	typedef directx_material_handle_t material_handle_t;
	#define MATERIAL_HANDLE_INVALID DIRECTX_MATERIAL_HANDLE_INVALID
	typedef struct directx_shader_t directx_shader_t;
	typedef directx_shader_t shader_t;
	typedef directx_shader_library_t shader_library_t;
	typedef directx_shader_handle_t shader_handle_t;
#elif defined(RENDERER_METAL_DRIVER)
	#include <renderer/internal/metal/metal_material_library.h>
	typedef metal_material_library_t material_library_t;
	typedef metal_material_t material_t;
	typedef metal_material_handle_t material_handle_t;
	#define MATERIAL_HANDLE_INVALID METAL_MATERIAL_HANDLE_INVALID
	typedef struct metal_shader_t metal_shader_t;
	typedef metal_shader_t shader_t;
	typedef metal_shader_library_t shader_library_t;
	typedef metal_shader_handle_t shader_handle_t;
#endif

/* constructors & destructors */
RENDERER_API material_library_t* material_library_new();
RENDERER_API material_library_t* material_library_create(renderer_t* renderer, shader_library_t* shader_library);
RENDERER_API void material_library_destroy(material_library_t* library);
RENDERER_API void material_library_release_resources(material_library_t* library);

/* logic functions */

/*
	description: creates a material into the library with a shader named as 'shader_name' with the identification name as 'material_name'
	params:
		library: ptr to the material_library_t
		shader_name: name of the shader to be used in the material, the shader must be present in the shader library
		material_name: identification name of this material_t object created
	returns:
		material_handle_t, handle to the material_t object just created
		MATERIAL_HANDLE_INVALID, if the creation failed
 */
RENDERER_API material_handle_t material_library_create_material(material_library_t* library, const char* shader_name, const char* material_name);

/*
	description: creates a material into the library with a shader handle 'handle' with the identification name as 'material_name'
	params:
		library: ptr to the material_library_t
		handle: handle to the shader to be used in the material, the shader must be present in the shader library
		material_name: identification name of this material_t object created
	returns:
		material_handle_t, handle to the material_t object just created
		MATERIAL_HANDLE_INVALID, if the creation failed
 */
RENDERER_API material_handle_t material_library_create_materialH(material_library_t* library, shader_handle_t handle, const char* material_name);

/*
	description: loads and creates a material into the library with the identification name as 'material_name'
	params:
		library: ptr to the material_library_t
		file_path: path of the material file (.mat)
		material_name: identification name of this material_t object created
	returns:
		material_handle_t, handle to the material_t object just loaded & created
		MATERAIL_HANDLE_INVALID, if the creation or load failed
 */
RENDERER_API material_handle_t material_library_load_material(material_library_t* library, const char* file_path);

/*
	description: destroys & removes a material in the library with the material named as 'material_name'
	params:
		library: ptr to the material_library_t
		material_name: identification name of the material to be destroyed
	returns:
		true, if the destruction succeed
		false, if the destruction failed
 */
RENDERER_API bool material_library_destroy_material(material_library_t* library, const char* material_name);

/*
	description: destroys & removes a material in the library with the material having handle as 'handle'
	params:
		library: ptr to the material_library_t
		handle: handle to the material to be destroyed
	returns:
		true, if the destruction succeed
		false, if the destruction failed
 */
RENDERER_API bool material_library_destroy_materialH(material_library_t* library, material_handle_t handle);

/*
	description: serializes the material_library_t object into stream of bytes (BUFFER* object)
	params:
		library: ptr to the material_library_t object to be serialized
	returns:
		ptr to the BUFFER object containing the serialized bytes
		NULL, if the serialization failed
 */
RENDERER_API BUFFER* material_library_serialize(material_library_t* library);

/*
	description: deserializes a stream of bytes into material_library_t object
	params:
		bytes: ptr to the bytes
		length: length of the bytes array
	returns:
		ptr to the deserialized material_library_t object
		NULL if the deserialization failed
 */
RENDERER_API material_library_t* material_library_deserialize(void* bytes, u64 length);

/*
	description: deserializes a stream of bytes into material_library_t object
	params:
		bytes: ptr to the bytes
		length: length of the bytes array
		out_library: ptr to the already existing material_library_t object
	returns:
		true if the deserialization succeed, otherwise false
 */
RENDERER_API bool material_library_deserialize_no_alloc(void* bytes, u64 length, material_library_t* out_library);

/* getters */

/*
	description: returns a handle to a material with name 'material_name' in the library
	params:
		library: ptr to the material_library_t
		material_name: identification name of the material_t object in the library
	returns:
		material_handle_t, handle to the material object
		MATERIAL_HANDLE_INVALID, if the material object isn't found with name 'material_name'
 */
RENDERER_API material_handle_t material_library_get_handle(material_library_t* library, const char* material_name);

/*
	description: returns a ptr to the material with the name 'material_name'
	params:
		library: ptr to the material_library_t
		material_name: identification name of the material_t object in the library
	returns:
		material_t*, ptr to the material object
		NULL, if the material object isn't found with the name 'name'
 */
RENDERER_API material_t* material_library_get(material_library_t* library, const char* material_name);

/*
	description: returns a ptr to the material with the handle 'handle'
	params:
		library: ptr to the material_library_t
		handle: handle to the material object in the library
	returns:
		material_t*, ptr to the material object 
		NULL, if the material object isn't found with handle 'handle'
 */
RENDERER_API material_t* material_library_getH(material_library_t* library, material_handle_t handle);

/*
	description: returns the name of the material with the handle 'handle'
	params:
		library: ptr to the material_library_t
		handle: handle to the material object in the library
	returns:
		const char*, name of the material object
		NULL, if the material object isn't found with handle 'handle'
 */
RENDERER_API const char* material_library_get_nameH(material_library_t* library, material_handle_t handle);

