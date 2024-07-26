#include <sge/sge_buffer.h>

#include <sge/sge.h> /* for VULKAN_RENDERER() */

static VkBufferUsageFlags get_usage_from_type(sge_buffer_type_t type)
{
	switch(type)
	{
		case SGE_BUFFER_TYPE_VERTEX: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		case SGE_BUFFER_TYPE_INDEX: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		default: DEBUG_LOG_FETAL_ERROR("Unsupported buffer type: %u", type);
	}
	return 0;
}

SGE_API sge_buffer_t* sge_buffer_create(sge_driver_t* driver, sge_buffer_create_info_t* create_info)
{
	_debug_assert__(create_info->size != 0);

	vulkan_buffer_create_info_t v_create_info =
	{
		.data = create_info->data,
		.size = create_info->size,
		.vo_usage_flags = get_usage_from_type(create_info->type),
		.vo_sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
		.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	return vulkan_buffer_create(VULKAN_RENDERER(driver), &v_create_info);
}

SGE_API void sge_buffer_destroy(sge_buffer_t* buffer)
{
	vulkan_buffer_destroy(VULKAN_BUFFER_CAST(buffer));
}

SGE_API void sge_buffer_release_resources(sge_buffer_t* buffer)
{
	vulkan_buffer_release_resources(VULKAN_BUFFER_CAST(buffer));
}


SGE_API void sge_buffer_copy_data(sge_buffer_t* buffer, u32 buffer_offset, void* data, u32 data_size)
{
	vulkan_buffer_copy_data(VULKAN_BUFFER_CAST(buffer), buffer_offset, data, data_size);
}

SGE_API void* sge_buffer_map(sge_buffer_t* buffer)
{
	return vulkan_buffer_map(VULKAN_BUFFER_CAST(buffer));
}

SGE_API void sge_buffer_unmap(sge_buffer_t* buffer)
{
	vulkan_buffer_unmap(VULKAN_BUFFER_CAST(buffer));
}

SGE_API void sge_buffer_set_traits(sge_buffer_t* buffer, const sge_buffer_traits_t* traits)
{
	vulkan_buffer_set_traits(VULKAN_BUFFER_CAST(buffer), REINTERPRET_CONST_CAST(const vulkan_buffer_traits_t*, traits));
}

SGE_API sge_buffer_traits_t* buffer_get_tratis(sge_buffer_t* buffer)
{
	AUTO traits = vulkan_buffer_get_traits(VULKAN_BUFFER_CAST(buffer));
	return REINTERPRET_CAST(sge_buffer_traits_t*, traits);
}
