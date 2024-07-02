#pragma once

#include <sge/defines.h>
#include <bufferlib/buffer.h> /* buffer_t* */
#include <sge/internal/vulkan/vulkan_object.h>
#include <sge/internal/vulkan/vulkan_host_buffered_buffer.h>
#include <vulkan/vulkan_core.h> /* for VkBufferUsageFlags */
#include <sge/dictionary.h>

typedef struct struct_descriptor_t struct_descriptor_t;
typedef struct vulkan_host_buffered_buffer_t vulkan_host_buffered_buffer_t;

typedef struct vulkan_formatted_buffer_create_info_t
{
	/* it is internally cloned, therefore you can't really modify (add or remove fields) it once vulkan_formatted_buffer_t object is created */
	struct_descriptor_t* format;
	/* this usage flags is used when creating the vulkan_host_buffered_buffer_t object internally, otherwise it is ignored */
	VkBufferUsageFlags vo_usage_flags;
	/* if supplied NULL, then it will be created internally,
	 * this also means vo_usage_flags will be used */
	vulkan_host_buffered_buffer_t* host_buffered_buffer;
} vulkan_formatted_buffer_create_info_t;

typedef dictionary_t /* key: struct_field_t*, value: buffer_t* */ array_buffer_map_t;

typedef struct vulkan_formatted_buffer_t
{
	__VULKAN_OBJECT__;
	vulkan_renderer_t* renderer;
	/* clone of create_info.format */
	struct_descriptor_t* format;
	/* set to true internally when create_info.host_buffered_buffer is NULL */
	bool is_user_supplied_host_buffered_buffer;
	/* points to create_info.host_buffered_buffer or internally created vulkan_host_buffered_buffer_t object */
	vulkan_host_buffered_buffer_t* host_buffered_buffer;
	/* stores buffer_t objects for each array field in 'format' */
	array_buffer_map_t array_buffer_map;
	/* the target buffer which the 'format' is mapped to */
	buffer_t target_buffer;
	bool is_dynamic_buffer_dirty;
} vulkan_formatted_buffer_t;

/* performs dynamic casting (expensive), use only when you don't know the source type */
#define VULKAN_FORMATTED_BUFFER(ptr) VULKAN_OBJECT_UP_CAST(vulkan_formatted_buffer_t*, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, ptr)
#define VULKAN_FORMATTED_BUFFER_CONST(ptr) VULKAN_OBJECT_UP_CAST_CONST(const vulkan_formatted_buffer_t*, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, ptr)
/* otherwise (if you are sure that the type is VULKAN_OBJECT_TYPE_FORMATTED_BUFFER) use the following,
 * this macro expands to just a C-style cast in release mode so it is very efficient as compared to the dynamic casting above */
#define VULKAN_FORMATTED_BUFFER_CAST(ptr) VULKAN_OBJECT_TYPE_CAST(vulkan_formatted_buffer_t*, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, ptr)
#define VULKAN_FORMATTED_BUFFER_CAST_CONST(ptr) VULKAN_OBJECT_TYPE_CAST_CONST(const vulkan_formatted_buffer_t*, VULKAN_OBJECT_TYPE_FORMATTED_BUFFER, ptr)

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SGE_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_new(memory_allocator_t* allocator);
SGE_API void vulkan_formatted_buffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info, vulkan_formatted_buffer_t* buffer);
SGE_API vulkan_formatted_buffer_t* vulkan_formatted_buffer_create(vulkan_renderer_t* renderer, vulkan_formatted_buffer_create_info_t* create_info);
SGE_API void vulkan_formatted_buffer_destroy(vulkan_formatted_buffer_t* buffer);
SGE_API void vulkan_formatted_buffer_release_resources(vulkan_formatted_buffer_t* buffer);

/* logic functions */
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_buffer_t* vulkan_formatted_buffer_get_device_buffer(vulkan_formatted_buffer_t* buffer)
{ return vulkan_host_buffered_buffer_get_device_buffer(buffer->host_buffered_buffer); }
/* NOTE: the returned buffer_t object may also be static if the array has size explicitly specified in 'format' */
SGE_API buffer_t* vulkan_formatted_buffer_get_array_buffer(vulkan_formatted_buffer_t* buffer, const char* name);
SGE_API u32 vulkan_formatted_buffer_get_array_length(vulkan_formatted_buffer_t* buffer, const char* name);
SGE_API void vulkan_formatted_buffer_set_uint(vulkan_formatted_buffer_t* buffer, const char* name, u32 value);
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE bool vulkan_formatted_buffer_is_dirty(vulkan_formatted_buffer_t* buffer) 
{ return buffer->is_dynamic_buffer_dirty || vulkan_host_buffered_buffer_is_dirty(buffer->host_buffered_buffer); }
SGE_API bool vulkan_formatted_buffer_commit(vulkan_formatted_buffer_t* buffer, bool OUT is_resized);

END_CPP_COMPATIBLE
