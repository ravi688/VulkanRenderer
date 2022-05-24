
#include <renderer/material_library.h>

#include <renderer/memory_allocator.h>


/* constructors & destructors */
RENDERER_API material_library_t* material_library_new()
{
	material_library_t* library = heap_new(material_library_t);
	memset(library, 0, sizeof(material_library_t));
	return library;
}

RENDERER_API material_library_t* material_library_create(shader_library_t* shader_library)
{
	material_library_t* library = material_library_new();
	library->shader_library = shader_library;
	library->relocation_table = buf_create(sizeof(material_handle_t), 1, 0);
	library->materials = buf_create(sizeof(material_library_slot_t), 1, 0);
	return library;
}

RENDERER_API void material_library_destroy(material_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		string_destroy(slot->path);
		material_destroy(slot->material);
	}
}

RENDERER_API void material_library_release_resources(material_library_t* library)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		material_release_resources(slot->material);
	}	
}


/* logic functions */
RENDERER_API material_handle_t material_library_add(material_library_t* library, material_t* material, const char* material_name)
{
	buf_ucount_t index = buf_get_element_count(&library->materials);

	// generate a new handle
	buf_ucount_t handle = buf_get_element_count(&library->relocation_table);
	buf_push(&library->relocation_table, &index);

	// create a new slot
	material_library_slot_t slot = 
	{
		.name = (material_name != NULL) ? string_create(material_name) : NULL,
		.material = material,
		.handle = handle
	};
	buf_push(&library->materials, &slot);
	return handle;
}

RENDERER_API material_handle_t material_library_create_material(material_library_t* library, const char* shader_name, const char* material_name)
{
	material_create_info_t create_info = 
	{
		.shader = shader_library_get(library->shader_library, shader_name),
		.is_vertex_attrib_from_file = true
	};
	if(create_info.shader == NULL)
	{
		log_wrn("Material creation failed as shader with name \"%s\" isn't found in the shader library\n", shader_name);
		return MATERIAL_HANDLE_INVALID;
	}
	return material_library_add(library, material_create(library->context, &create_info), material_name);
}

RENDERER_API material_handle_t material_library_create_materialH(material_library_t* library, shader_handle_t handle, const char* material_name)
{
	material_create_info_t create_info = 
	{
		.shader = shader_library_getH(library->shader_library, handle),
		.is_vertex_attrib_from_file = true
	};
	if(create_info.shader == NULL)
	{
		log_wrn("Material creation failed as shader with name \"%s\" isn't found in the shader library\n", shader_name);
		return MATERIAL_HANDLE_INVALID;
	}
	return material_library_add(library, material_create(library->context, &create_info), material_name);
}

RENDERER_API material_handle_t material_library_load_material(material_library_t* library, const char* file_path, const char* material_name)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API bool material_library_destroy_material(material_library_t* library, const char* material_name)
{
	material_t* material = material_library_get(library, material_name);
	if(material == NULL)
		return false;
	material_destroy(material);
	return material_library_remove(library, material_name);
}

RENDERER_API bool material_library_destroy_materialH(material_library_t* library, material_handle_t handle)
{
	material_t* material = material_library_getH(library, handle);
	if(material == NULL)
		return false;
	material_destroy(material);
	return material_library_removeH(library, handle);
}

RENDERER_API bool material_library_remove(material_library_t* library, const char* material_name)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API bool material_library_removeH(material_library_t* library, material_handle_t handle)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API BUFFER* material_library_serialize(material_library_t* library)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API material_library_t* material_library_deserialize(void* bytes, u64 length)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API bool material_library_deserialize_no_alloc(void* bytes, u64 length, material_library_t* out_library)
{
	NOT_IMPLEMETED_FUNCTION();
}


/* getters */

static material_library_slot_t* material_library_get_slot_by_ptr(material_library_t* library, material_t* material)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		if(slot->material == material)
			return slot;
	}
	return NULL;
}

static material_library_slot_t* material_library_get_slot(material_library_t* library, const char* material_name)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_library_slot_t* slot = buf_get_ptr_at(&library->materials, i);
		if(strcmp(slot->name, material_name) == 0)
			return slot;
	}
	return NULL;
}

static material_library_slot_t* material_library_get_slotH(material_library_t* library, material_handle_t handle)
{
	buf_ucount_t count = buf_get_element_count(&library->materials);
	for(buf_ucount_t i = 0; i < count; i++)
	{
		material_library_slot_t* slot  = buf_get_ptr_at(&library->materials, i);
		if(slot->handle == handle)
			return slot;
	}
	return NULL;
}

RENDERER_API material_handle_t material_library_get_handle(material_library_t* library, const char* material_name)
{
	material_library_slot_t* slot = material_library_get_slot(library, material_name);
	return (slot == NULL) ? MATERIAL_HANDLE_INVALID : slot->handle;
}

RENDERER_API material_t* material_library_get(material_library_t* library, const char* material_name)
{
	material_library_slot_t* slot = material_library_get_slot(library, material_name);
	return (slot == NULL) ? NULL : slot->material;
}

RENDERER_API material_t* material_library_getH(material_library_t* library, material_handle_t handle)
{
	material_library_slot_t* slot = material_library_get_slotH(library, handle);
	return (slot == NULL) ? NULL : slot->material;
}

RENDERER_API const char* material_library_get_nameH(material_library_t* library, material_handle_t handle)
{
	material_library_slot_t* slot = material_library_get_slotH(library, handle);
	return (slot == NULL) ? NULL : slot->name;
}

RENDERER_API const char* material_library_get_name(material_library_t* library, material_t* material)
{
	material_library_slot_t* slot = material_library_get_slot_by_ptr(library, material);
	return (slot == NULL) ? NULL : slot->name;
}


/* setters */
RENDERER_API material_handle_t material_library_set(material_library_t* library, const char* name, material_t* material)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API material_handle_t material_library_setH(material_library_t* library, material_handle_t handle, material_t* material)
{
	NOT_IMPLEMETED_FUNCTION();
}

RENDERER_API material_handle_t material_library_set_nameH(material_library_t* library, material_handle_t handle, const char* material_name)
{
	NOT_IMPLEMETED_FUNCTION();
}

