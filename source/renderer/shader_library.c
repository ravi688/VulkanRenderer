
#include <renderer/shader_library.h>.

#include <renderer/io.h>
#include <renderer/memory_allocator.h>


/* constructors & destructors */
RENDERER_API shader_library_t* shader_library_new()
{
	shader_library_t* library = heap_new(shader_library_t);
	memset(library, 0, sizeof(shader_library_t));
	return library;
}

RENDERER_API shader_library_t* shader_library_create(renderer_t* context)
{
	shader_library_t* library = shader_library_new();
	library->relocation_table = buf_create(sizeof(buf_ucount_t), 1, 0);
	library->shaders = buf_create(sizeof(shader_library_slot_t), 1, 0);
	return library;
}

RENDERER_API shader_library_t* shader_library_load_folder(renderer_t* context, const char* folder_path)
{
	NOT_IMPLEMENTED_FUNCTION();
	// // create a shader library
	// shader_library_t* library = shader_library_craete(context);

	// // get all the files of type .sb recursively
	// BUFFER files = directory_get_files(folder_path, "*.sb", DIRECTORY_SEARCH_TYPE_RECURSIVE);
	
	// // load each .sb file
	// int file_count = buf_get_element_count(&files);
	// for(int i = 0; i < file_count; i++)
	// {
	// 	file_info_t* info = buf_get_ptr_at(&files, i);
	// 	shader_handle_t handle = shader_library_load_shader(library, info->path);
	// 	ASSERT(handle != SHADER_HANDLE_INVALID, "Loading shader \"%s\" failed\n", info->path);
	// }
	// buf_free(&files);
	// return library;
}

RENDERER_API void shader_library_destroy(shader_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		string_destroy(slot->name);
		shader_destroy(slot->shader);
	}
}

RENDERER_API void shader_library_release_resources(shader_library_t* library)
{
	buf_free(&library->relocation_table);
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		shader_release_resources(slot->shader);
	}
	buf_free(&library->shaders);
	heap_free(library);
}


/* logic functions */
RENDERER_API shader_handle_t shader_library_create_shader(shader_library_t* library, void* sb, u32 length, const char* shader_name)
{
	// WARNING | TODO: buf_create with capacity 0 must not allocate any memory!
	BUFFER sb = buf_create(sizeof(char), 0, 0);
	buf_set_ptr(&sb, sb);
	buf_set_element_count(&sb, length);
	buf_set_capacity(&sb, length);
	return shader_library_add(library, shader_create(library->context, &sb), shader_name);
}

RENDERER_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path, const char* shader_name)
{
	return shader_library_add(library, shader_load(library->context, file_path), shader_name);
}

RENDERER_API shader_handle_t shader_library_add(shader_library_t* library, shader_t* shader, const char* shader_name)
{
	buf_ucount_t index = buf_get_element_count(&library->shaders);

	// generate a new handle
	buf_ucount_t handle = buf_get_element_count(&library->relocation_table);
	buf_push(&library->relocation_table, &index);

	// create a new slot
	shader_library_slot_t slot = 
	{
		.name = (shader_name != NULL) ? string_create(shader_name) : NULL,
		.shader = shader,
		.handle = handle
	};
	buf_push(&library->shaders, &slot);
	return handle;
}

RENDERER_API bool shader_library_destroy_shader(shader_library_t* library, const char* shader_name)
{
	shader_t* shader = shader_library_get(library, shader_name);
	if(shader == NULL)
		return false;
	shader_destroy(shader);
	return shader_library_remove(library, shader_name);
}

RENDERER_API bool shader_library_destroy_shaderH(shader_library_t* library, shader_handle_t handle)
{
	shader_t* shader = shader_library_getH(library, handle);
	if(shader == NULL)
		return false;
	shader_destroy(shader);
	return shader_library_removeH(library, handle);
}

RENDERER_API bool shader_library_remove(shader_library_t* library, const char* shader_name)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API bool shader_library_removeH(shader_library_t* library, shader_handle_t handle)
{
	NOT_IMPLEMENTED_FUNCTION();
}


/* getters */

RENDERER_API shader_handle_t shader_library_get_handle(shader_library_t* library, const char* shader_name)
{
	// TODO: Optimize it with hashing the string to integers
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		if(strcmp(slot->name, shader_name) == 0)
			return slot->handle;
	}
	return SHADER_HANDLE_INVALID;
}

RENDERER_API const char* shader_library_get_nameH(shader_library_t* library, shader_handle_t handle)
{
	buf_ucount_t index;
	buf_get_at(library->relocation_table, handle, &index);
	
	shader_library_slot_t slot;
	buf_get_at(library->shaders, index, &slot);
	return slot.name;	
}

RENDERER_API const char* shader_library_get_name(shader_library_t* library, shader_t* shader)
{
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		if(slot->shader == shader)
			return slot->name;
	}
	return NULL;
}

RENDERER_API shader_t* shader_library_getH(shader_library_t* library, shader_handle_t handle)
{
	buf_ucount_t index;
	buf_get_at(library->relocation_table, handle, &index);

	shader_library_slot_t slot;
	buf_get_at(library->shaders, index, &slot);
	return slot.shader;
}

RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name)
{
	return shader_library_getH(library, shader_library_get_handle(library, shader_name));
}


/* setters */
RENDERER_API shader_handle_t shader_library_setH(shader_library_t* library, shader_handle_t handle, shader_t* shader)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API shader_handle_t shader_library_set(shader_library_t* library, const char* shader_name, shader_t* shader)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API shader_handle_t shader_library_set_nameH(shader_library_t* library, shader_handle_t handle, const char* shader_name)
{
	NOT_IMPLEMENTED_FUNCTION();
}
