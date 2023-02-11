#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/io.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/debug.h>
#include <string.h>

/* constructors & destructors */
RENDERER_API vulkan_shader_library_t* vulkan_shader_library_new(memory_allocator_t* allocator)
{
	vulkan_shader_library_t* library = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_LIBRARY, vulkan_shader_library_t);
	memzero(library, vulkan_shader_library_t);
	return library;
}

RENDERER_API void vulkan_shader_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t OUT library)
{
	memzero(library, vulkan_shader_library_t);

	library->renderer = renderer;
	library->relocation_table = buf_create(sizeof(buf_ucount_t), 1, 0);
	library->shaders = buf_create(sizeof(vulkan_shader_library_slot_t), 1, 0);
	log_msg("Vulkan shader library has been created successfully\n");
}

RENDERER_API vulkan_shader_library_t* vulkan_shader_library_create(vulkan_renderer_t* renderer)
{
	vulkan_shader_library_t* library = vulkan_shader_library_new(renderer->allocator);
	vulkan_shader_library_create_no_alloc(renderer, library);
	return library;
}

RENDERER_API vulkan_shader_library_t* vulkan_shader_library_load_folder(vulkan_renderer_t* renderer, const char* folder_path)
{
	NOT_IMPLEMENTED_FUNCTION();
	// // create a shader library
	// vulkan_shader_library_t* library = vulkan_shader_library_craete(renderer);

	// // get all the files of type .sb recursively
	// BUFFER files = directory_get_files(folder_path, "*.sb", DIRECTORY_SEARCH_TYPE_RECURSIVE);
	
	// // load each .sb file
	// int file_count = buf_get_element_count(&files);
	// for(int i = 0; i < file_count; i++)
	// {
	// 	file_info_t* info = buf_get_ptr_at(&files, i);
	// 	vulkan_shader_handle_t handle = vulkan_shader_library_load_shader(library, info->path);
	// 	ASSERT(handle != SHADER_HANDLE_INVALID, "Loading shader \"%s\" failed\n", info->path);
	// }
	// buf_free(&files);
	// return library;
}

RENDERER_API void vulkan_shader_library_destroy(vulkan_shader_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		string_destroy(&slot->name);
		vulkan_shader_destroy(slot->shader);
	}
	log_msg("Vulkan shader library has been destroyed successfully\n");
}

RENDERER_API void vulkan_shader_library_release_resources(vulkan_shader_library_t* library)
{
	buf_free(&library->relocation_table);
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		vulkan_shader_release_resources(slot->shader);
	}
	buf_free(&library->shaders);
	// TODO
	// heap_free(library);
}


/* logic functions */

static vulkan_shader_handle_t vulkan_shader_library_add(vulkan_shader_library_t* library, vulkan_shader_t* shader, const char* vulkan_shader_name)
{
	buf_ucount_t index = buf_get_element_count(&library->shaders);

	// generate a new handle
	buf_ucount_t handle = buf_get_element_count(&library->relocation_table);
	buf_push(&library->relocation_table, &index);

	// create a new slot
	vulkan_shader_library_slot_t slot = 
	{
		.name = (vulkan_shader_name != NULL) ? string_create(library->renderer->allocator, vulkan_shader_name) : string_null(),
		.shader = shader,
		.handle = handle
	};
	shader->handle = handle;
	buf_push(&library->shaders, &slot);
	return handle;
}

RENDERER_API vulkan_shader_handle_t vulkan_shader_library_create_shader(vulkan_shader_library_t* library, vulkan_shader_create_info_t* create_info, const char* vulkan_shader_name)
{
	return vulkan_shader_library_add(library, vulkan_shader_create(library->renderer, create_info), vulkan_shader_name);
}

RENDERER_API vulkan_shader_handle_t vulkan_shader_library_load_shader(vulkan_shader_library_t* library, vulkan_shader_load_info_t* load_info, const char* shader_name)
{
	return vulkan_shader_library_add(library, vulkan_shader_load(library->renderer, load_info), shader_name);
}

static bool vulkan_shader_library_remove(vulkan_shader_library_t* library, const char* vulkan_shader_name)
{
	NOT_IMPLEMENTED_FUNCTION();
}

static bool vulkan_shader_library_removeH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API bool vulkan_shader_library_destroy_shader(vulkan_shader_library_t* library, const char* vulkan_shader_name)
{
	vulkan_shader_t* shader = vulkan_shader_library_get(library, vulkan_shader_name);
	if(shader == NULL)
		return false;
	vulkan_shader_destroy(shader);
	return vulkan_shader_library_remove(library, vulkan_shader_name);
}

RENDERER_API bool vulkan_shader_library_destroy_shaderH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle)
{
	vulkan_shader_t* shader = vulkan_shader_library_getH(library, handle);
	if(shader == NULL)
		return false;
	vulkan_shader_destroy(shader);
	return vulkan_shader_library_removeH(library, handle);
}

/* getters */

RENDERER_API const char* vulkan_shader_library_get_nameH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle)
{
	buf_ucount_t index;
	buf_get_at(&library->relocation_table, handle, &index);

	vulkan_shader_library_slot_t slot;
	buf_get_at(&library->shaders, index, &slot);
	return slot.name.data;
}

RENDERER_API vulkan_shader_handle_t vulkan_shader_library_get_handle(vulkan_shader_library_t* library, const char* vulkan_shader_name)
{
	// TODO: Optimize it with hashing the string to integers
	buf_ucount_t count = buf_get_element_count(&library->shaders);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_shader_library_slot_t* slot = buf_get_ptr_at(&library->shaders, i);
		if(strcmp(slot->name.data, vulkan_shader_name) == 0)
			return slot->handle;
	}
	return VULKAN_SHADER_HANDLE_INVALID;
}

RENDERER_API vulkan_shader_t* vulkan_shader_library_getH(vulkan_shader_library_t* library, vulkan_shader_handle_t handle)
{
	buf_ucount_t index;
	buf_get_at(&library->relocation_table, handle, &index);

	vulkan_shader_library_slot_t slot;
	buf_get_at(&library->shaders, index, &slot);
	return slot.shader;
}

RENDERER_API vulkan_shader_t* vulkan_shader_library_get(vulkan_shader_library_t* library, const char* vulkan_shader_name)
{
	return vulkan_shader_library_getH(library, vulkan_shader_library_get_handle(library, vulkan_shader_name));
}
