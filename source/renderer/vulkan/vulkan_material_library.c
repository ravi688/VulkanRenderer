#include <renderer/internal/vulkan/vulkan_material_library.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_material.h>
#include <renderer/memory_allocator.h>
#include <renderer/debug.h>

/* constructors & destructors */
RENDERER_API vulkan_material_library_t* vulkan_material_library_new()
{
	vulkan_material_library_t* library = heap_new(vulkan_material_library_t);
	memzero(library, vulkan_material_library_t);
	return library;
}

RENDERER_API void vulkan_material_library_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library, vulkan_material_library_t OUT library)
{
	memzero(library, vulkan_material_library_t);

	library->renderer = renderer;
	library->shader_library = shader_library;
	library->relocation_table = buf_create(sizeof(vulkan_material_handle_t), 1, 0);
	library->materials = buf_create(sizeof(vulkan_material_library_slot_t), 1, 0);
	log_msg("Vulkan material library has been created successfully\n");
}

RENDERER_API vulkan_material_library_t* vulkan_material_library_create(vulkan_renderer_t* renderer, vulkan_shader_library_t* shader_library)
{
	vulkan_material_library_t* library = vulkan_material_library_new();
	vulkan_material_library_create_no_alloc(renderer, shader_library, library);
	return library;
}

RENDERER_API void vulkan_material_library_destroy(vulkan_material_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		string_destroy(&slot->name);
		vulkan_material_destroy(slot->material);
	}
	log_msg("Vulkan material library has been destroyed successfully\n");
}

RENDERER_API void vulkan_material_library_release_resources(vulkan_material_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		vulkan_material_release_resources(slot->material);
	}
	// TODO
	// heap_free(library);
}


/* logic functions */
static vulkan_material_handle_t vulkan_material_library_add(vulkan_material_library_t* library, vulkan_material_t* material, const char* material_name)
{
	buf_ucount_t index = buf_get_element_count(&library->materials);

	// generate a new handle
	buf_ucount_t handle = buf_get_element_count(&library->relocation_table);
	buf_push(&library->relocation_table, &index);

	// create a new slot
	vulkan_material_library_slot_t slot = 
	{
		.name = (material_name != NULL) ? string_create(material_name) : string_null(),
		.material = material,
		.handle = handle
	};
	material->handle = handle;
	buf_push(&library->materials, &slot);
	return handle;
}

RENDERER_API vulkan_material_handle_t vulkan_material_library_create_material(vulkan_material_library_t* library, const char* shader_name, const char* material_name)
{
	vulkan_shader_t* shader = vulkan_shader_library_get(library->shader_library, shader_name);
	if(shader == NULL)
	{
		log_wrn("Material creation failed as shader with name \"%s\" isn't found in the shader library\n", shader_name);
		return VULKAN_MATERIAL_HANDLE_INVALID;
	}
	return vulkan_material_library_add(library, vulkan_material_create(library->shader_library->renderer, shader), material_name);
}

RENDERER_API vulkan_material_handle_t vulkan_material_library_create_materialH(vulkan_material_library_t* library, vulkan_shader_handle_t handle, const char* material_name)
{
	vulkan_shader_t* shader = vulkan_shader_library_getH(library->shader_library, handle);
	if(shader == NULL)
	{
		log_wrn("Material creation failed as shader with the specified handle isn't found in the shader library\n");
		return VULKAN_MATERIAL_HANDLE_INVALID;
	}
	return vulkan_material_library_add(library, vulkan_material_create(library->shader_library->renderer, shader), material_name);
}

RENDERER_API vulkan_material_handle_t vulkan_material_library_load_material(vulkan_material_library_t* library, const char* file_path)
{
	NOT_IMPLEMENTED_FUNCTION();
}


static bool vulkan_material_library_remove(vulkan_material_library_t* library, const char* material_name)
{
	NOT_IMPLEMENTED_FUNCTION();
}

static bool vulkan_material_library_removeH(vulkan_material_library_t* library, vulkan_material_handle_t handle)
{
	NOT_IMPLEMENTED_FUNCTION();
}


RENDERER_API bool vulkan_material_library_destroy_material(vulkan_material_library_t* library, const char* material_name)
{
	vulkan_material_t* material = vulkan_material_library_get(library, material_name);
	if(material == NULL)
		return false;
	vulkan_material_destroy(material);
	return vulkan_material_library_remove(library, material_name);
}

RENDERER_API bool vulkan_material_library_destroy_materialH(vulkan_material_library_t* library, vulkan_material_handle_t handle)
{
	vulkan_material_t* material = vulkan_material_library_getH(library, handle);
	if(material == NULL)
		return false;
	vulkan_material_destroy(material);
	return vulkan_material_library_removeH(library, handle);
}

RENDERER_API BUFFER* vulkan_material_library_serialize(vulkan_material_library_t* library)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API vulkan_material_library_t* vulkan_material_library_deserialize(void* bytes, u64 length)
{
	NOT_IMPLEMENTED_FUNCTION();
}

RENDERER_API bool vulkan_material_library_deserialize_no_alloc(void* bytes, u64 length, vulkan_material_library_t* out_library)
{
	NOT_IMPLEMENTED_FUNCTION();
}


/* getters */

static vulkan_material_library_slot_t* vulkan_material_library_get_slot(vulkan_material_library_t* library, const char* material_name)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		if(strcmp(slot->name.data, material_name) == 0)
			return slot;
	}
	return NULL;
}

static vulkan_material_library_slot_t* vulkan_material_library_get_slotH(vulkan_material_library_t* library, vulkan_material_handle_t handle)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		vulkan_material_library_slot_t* slot  = buf_get_ptr_at(&library->materials, i);
		if(slot->handle == handle)
			return slot;
	}
	return NULL;
}

RENDERER_API vulkan_material_handle_t vulkan_material_library_get_handle(vulkan_material_library_t* library, const char* material_name)
{
	vulkan_material_library_slot_t* slot = vulkan_material_library_get_slot(library, material_name);
	return (slot == NULL) ? VULKAN_MATERIAL_HANDLE_INVALID : slot->handle;
}

RENDERER_API vulkan_material_t* vulkan_material_library_get(vulkan_material_library_t* library, const char* material_name)
{
	vulkan_material_library_slot_t* slot = vulkan_material_library_get_slot(library, material_name);
	return (slot == NULL) ? NULL : slot->material;
}

RENDERER_API vulkan_material_t* vulkan_material_library_getH(vulkan_material_library_t* library, vulkan_material_handle_t handle)
{
	vulkan_material_library_slot_t* slot = vulkan_material_library_get_slotH(library, handle);
	return (slot == NULL) ? NULL : slot->material;
}
