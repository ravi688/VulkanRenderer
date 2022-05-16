
#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

#include <renderer/shader_library.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/string.h>

typedef buf_ucount_t material_handle_t;
#define MATERIAL_HANDLE_INVALID (~0ULL)

typedef struct material_library_slot_t
{
	string_t name;
	material_t* material;
	material_handle_t handle;
} material_library_slot_t;

typedef struct material_library_t
{
	shader_library_t* shader_library;
	BUFFER relocation_table;
	BUFFER materials;
} material_library_t;


/* constructors & destructors */
RENDERER_API material_library_t* material_library_new();
RENDERER_API material_library_t* material_library_create(shader_library_t* shader_library);
RENDERER_API void material_library_destroy(material_library_t* library);
RENDERER_API void material_library_release_resources(material_library_t* library);

/* logic functions */

/*
	description: adds a material into the libray with identification name as 'material_name'
	params:
		library: ptr to the material_library_t
		material: ptr to the material_t object ( to be added into library )
		material_name: identification name of this material_t object
	returns:
		material_handle_t, handle to the material_t object just added
		MATERIAL_HANDLE_INVLIAD, if the addition failed
	NOTE:
		multiple material_t objects could be added with the same identification name,
		in that case the first one will always be considered first in every operation.
 */
RENDERER_API material_handle_t material_library_add(material_library_t* library, material_t* material, const char* material_name);

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
	description: removes a material from the library with the material named as 'material_name', but doesn't destroys it
	params:
		library: ptr to the material_library_t
		material_name: identification name of the material to be removed
	returns:
		true, if the removal succeed
		false, if the removal failed
 */
RENDERER_API bool material_library_remove(material_library_t* library, const char* material_name);

/*
	description: removes a material from the library with the material having handle as 'handle'
	params:
		library: ptr to the material_library_t
		material_name: handle to the material to be removed
	returns:
		true, if the removal succeed
		false, if the removal failed
 */
RENDERER_API bool material_library_removeH(material_library_t* library, material_handle_t handle);

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

/*
	description: returns the name of the material with the ptr to the material_t object
	params:
		library: ptr to the material_library_t
		material: ptr to the material_t object
	returns:
		const char*, name of the material object
		NULL, if the material object isn't found
 */
RENDERER_API const char* material_librayr_get_name(material_library_t* library, material_t* material);

/* setters */

/*
	description: sets the material_t object to the slot with identification name as 'name'
	params:
		library: ptr to the material_libray_t
		name: identification name of the material slot
		material: ptr to the material_t object
	returns:
		material_handle_t, handle to the material_t object with name 'name'
		MATERIAL_HANDLE_INVALID, if setting or adding failed
	NOTE:
		if the slot with material name 'name' isn't found then it will call material_library_add
 */
RENDERER_API material_handle_t material_library_set(material_library_t* library, const char* name, material_t* material);

/*
	description: sets the material_t object to the slot with material handle as 'handle'
	params:
		library: ptr to the material_library_t object
		handle: handle to the material slot
		material: ptr to the material_t object
	returns:
		material_handle_t, handle to the material_t object with name 'name'
		MATERIAL_HANDLE_INVALID, if setting or adding failed
	NOTE:
		if the slot with material handle 'handle' isn't found then it will call material_library_add
 */
RENDERER_API material_handle_t material_library_setH(material_library_t* library, material_handle_t handle, material_t* material);

/*
	description: sets the name of the material slot with handle 'handle'
	params:
		library: ptr to the material_library_t object
		handle: handle to the material slot
		material_name: identification name of the material slot to be set
	returns:
		material_handle_t, handle (same that is passed)
		or a valid material_handle_t with a new slot in case the passed handle isn't valid
	NOTE:
		if the slot with material handle 'handle' isn't found then it will call material_library_add
 */
RENDERER_API material_handle_t material_library_set_nameH(material_library_t* library, material_handle_t handle, const char* material_name);