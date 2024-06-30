#include <renderer/internal/vulkan/vulkan_formatted_buffer.h>
#include <renderer/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/memory_allocator.h>
#include <renderer/struct_descriptor.h>

RENDERER_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_new(memory_allocator_t* allocator)
{
	vulkan_formatted_buffer_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_FORMATTED_BUFFER, vulkan_formatted_buffer_t);
	memzero(buffer, vulkan_formatted_buffer_t);
	VULKAN_OBJECT_INIT(buffer, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return buffer;
}

RENDERER_API void vulkan_formatted_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info, vulkan_formatted_buffer_t* buffer)
{
	VULKAN_OBJECT_MEMZERO(buffer, vulkan_formatted_buffer_t);
	buffer->renderer = renderer;
	buffer->format = struct_descriptor_clone_p(create_info->format);
	buffer->host_buffered_buffer = create_info->host_buffered_buffer;
	buffer->is_user_supplied_host_buffered_buffer = (create_info->host_buffered_buffer != NULL);
	/* if the user hasn't supplied already existing host buffered buffer object then create internally */
	if(!buffer->is_user_supplied_host_buffered_buffer)
	{
		vulkan_host_buffered_buffer_create_info_t _create_info = 
		{
			.stride = 1 /* 1 byte */,
			.capacity = struct_descriptor_min_sizeof(buffer->format),
			.vo_usage_flags = create_info->vo_usage_flags
		};
		buffer->host_buffered_buffer = vulkan_host_buffered_buffer_create(renderer, &_create_info);
	}
	buffer->array_buffer_map = dictionary_create(struct_field_t*, buffer_t*, 1, dictionary_key_comparer_ptr);
	struct_descriptor_map(buffer->format, buf_get_ptr(vulkan_host_buffered_buffer_get_host_buffer(buffer->host_buffered_buffer)));
}

RENDERER_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_create(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info)
{
	AUTO buffer = vulkan_formatted_buffer_new(renderer->allocator);
	vulkan_formatted_buffer_create_no_alloc(renderer, create_info, buffer);
	return buffer;
}

RENDERER_API void vulkan_formatted_buffer_destroy(vulkan_formatted_buffer_t* buffer)
{
	struct_descriptor_unmap(buffer->format);
	if(!buffer->is_user_supplied_host_buffered_buffer)
		vulkan_host_buffered_buffer_destroy(buffer->host_buffered_buffer);
	struct_descriptor_destroy(buffer->renderer->allocator, buffer->format);

	/* iterate over each raw buffer */
	u32 array_buffer_count = dictionary_get_count(&buffer->array_buffer_map);
	for(u32 i = 0; i < array_buffer_count; i++)
	{
		/* and release memory */
		buffer_t* raw_buffer;
		dictionary_get_value_at(&buffer->array_buffer_map, i, &raw_buffer);
		buf_free(raw_buffer);
	}
	dictionary_clear(&buffer->array_buffer_map);
}

RENDERER_API void vulkan_formatted_buffer_release_resources(vulkan_formatted_buffer_t* buffer)
{
	if(!buffer->is_user_supplied_host_buffered_buffer)
		vulkan_host_buffered_buffer_release_resources(buffer->host_buffered_buffer);
	dictionary_free(&buffer->array_buffer_map);
	if(VULKAN_OBJECT_IS_INTERNAL(buffer))
		memory_allocator_dealloc(buffer->renderer->allocator, buffer);
}


RENDERER_API buffer_t* vulkan_formatted_buffer_get_array_buffer(vulkan_formatted_buffer_t* buffer, const char* name)
{
	struct_field_t* field = struct_descriptor_get_field(buffer->format, name);

	/* if this field is not an array then return just NULL */
	if(!struct_field_is_array(field))
	{
		_debug_assert__(struct_field_is_array(field));
		return NULL;
	}

	/* if no raw buffer existings for this struct field array, then create and add it */
	buffer_t* raw_buffer = NULL;
	if(!dictionary_try_get_value(&buffer->array_buffer_map, &field, CAST_TO(void*, &raw_buffer)))
	{
		if(struct_field_is_variable_sized(field))
			raw_buffer = memory_allocator_BUFcreate(buffer->renderer->allocator, NULL, struct_field_array_get_stride(field), 0, 0);
		else
			raw_buffer = memory_allocator_BUFcreate_m(buffer->renderer->allocator, NULL, struct_field_get_base_address(buffer->format, field), 
																								struct_field_array_get_stride(field), 0, 0);
		dictionary_add(&buffer->array_buffer_map, &field, &raw_buffer);
	}
	_debug_assert__(raw_buffer != NULL);
	if(!buf_is_static(raw_buffer))
		buffer->is_dynamic_buffer_dirty = true;
	else
		vulkan_host_buffered_buffer_set_dirty(buffer->host_buffered_buffer, true);
	return raw_buffer;
}

RENDERER_API u32 vulkan_formatted_buffer_get_array_length(vulkan_formatted_buffer_t* buffer, const char* name)
{
	return struct_descriptor_get_array_size(buffer->format, name);
}

RENDERER_API void vulkan_formatted_buffer_set_uint(vulkan_formatted_buffer_t* buffer, const char* name, u32 value)
{
	struct_descriptor_set_uint(buffer->format, struct_descriptor_get_field_handle(buffer->format, name), &value);
	vulkan_host_buffered_buffer_set_dirty(buffer->host_buffered_buffer, true);
}

typedef buffer_t* buffer_ptr_t;
typedef struct_field_t* struct_field_ptr_t;
typedef_pair_t(buffer_ptr_t, struct_field_ptr_t);

static pair_t(buffer_ptr_t, struct_field_ptr_t) get_dynamic_array_buffer(vulkan_formatted_buffer_t* buffer)
{
	u32 array_buffer_count = dictionary_get_count(&buffer->array_buffer_map);
	for(u32 i = 0; i < array_buffer_count; i++)
	{
		buffer_t* raw_buffer;
		dictionary_get_value_at(&buffer->array_buffer_map, i, &raw_buffer);
		/* NOTE: here, we are returning the first found dynamic buffer because we already know only max one dynamic buffer can exist */
		if(!buf_is_static(raw_buffer))
		{
			struct_field_t* field;
			dictionary_get_key_at(&buffer->array_buffer_map, i, &field);
			return make_pair(buffer_ptr_t, struct_field_ptr_t) { raw_buffer, field };
		}
	}
	return make_pair(buffer_ptr_t, struct_field_ptr_t) { NULL, NULL };
}

RENDERER_API bool vulkan_formatted_buffer_commit(vulkan_formatted_buffer_t* buffer, bool OUT is_resized)
{
	if(!vulkan_formatted_buffer_is_dirty(buffer)) return false;

	if(buffer->is_dynamic_buffer_dirty)
	{
		pair_t(buffer_ptr_t, struct_field_ptr_t) result = get_dynamic_array_buffer(buffer);
		_debug_assert__(result.first != NULL);
		/* ensure the target buffer (the buffer to which the format is mapped to) can hold the dynamic array also */
		struct_field_set_array_size(buffer->format, result.second, buf_get_element_count(result.first));
		buffer_t* target_buffer = vulkan_host_buffered_buffer_get_host_buffer(buffer->host_buffered_buffer);
		buf_ensure_capacity(target_buffer, struct_descriptor_sizeof(buffer->format));
		buf_set_element_count(target_buffer, struct_descriptor_sizeof(buffer->format));

		/* map the buffer again as it might have resized by buf_ensure_capacity */
		struct_descriptor_map(buffer->format, buf_get_ptr(target_buffer));
		/* get raw address to the start of the dynamic array in the target buffer */
		void* address = struct_field_get_base_address(buffer->format, result.second);

		/* copy the contents of the dynamic array to the target buffer */

		/* number of elements */
		u32 array_size = buf_get_element_count(result.first);
		/* in bytes */
		u32 element_size = buf_get_element_size(result.first);
		_debug_assert__(element_size == struct_field_array_get_stride(result.second));
		_debug_assert__(array_size == struct_field_get_array_size(result.second));
		memcpy(address, buf_get_ptr(result.first), element_size * array_size);
		buffer->is_dynamic_buffer_dirty = false;

		vulkan_host_buffered_buffer_set_dirty(buffer->host_buffered_buffer, true);
	}

	if(vulkan_host_buffered_buffer_is_dirty(buffer->host_buffered_buffer))
		return vulkan_host_buffered_buffer_commit(buffer->host_buffered_buffer, is_resized);

	return false;
}
