#pragma once

#include <sge/defines.h>
#include <sge/alloc.h> /* for memcopy */

#ifdef SGE_VULKAN_DRIVER
	#include <sge/internal/vulkan/vulkan_buffer.h>
	typedef struct vulkan_buffer_t vulkan_buffer_t;
	typedef vulkan_buffer_t sge_buffer_t;
#elif SGE_OPENGL_DRIVER
	#include <sge/internal/opengl/opengl_buffer.h>
	typedef struct opengl_buffer_t opengl_buffer_t;
	typedef opengl_buffer_t sge_buffer_t;
#elif SGE_DIRECTX_DRIVER
	#include <sge/internal/directx/directx_buffer.h>
	typedef struct directx_buffer_t directx_buffer_t;
	typedef directx_buffer_t sge_buffer_t;
#elif SGE_METAL_DRIVER
	#include <sge/internal/metal/metal_buffer.h>
	typedef struct metal_buffer_t metal_buffer_t;
	typedef metal_buffer_t sge_buffer_t;
#endif

typedef enum sge_buffer_type_t
{
	SGE_BUFFER_TYPE_VERTEX,
	SGE_BUFFER_TYPE_INDEX
} sge_buffer_type_t;

/* IMPORTANT: This must be in sync with vulkan_buffer_traits_t */
typedef struct sge_buffer_traits_t
{
	/* number of elements in this buffer */
	u32 element_count;
	/* size of each element in this buffer */
	u32 element_size;
} sge_buffer_traits_t;

typedef struct sge_buffer_create_info_t
{
	sge_buffer_type_t type;
	/* pointer to a host side memory to copy data from 
	 * this can also be set to NULL if no initialization is needed. */
	void* data;
	/* size of the memory pointed by above pointer in bytes */
	u32 size;
	/* additional traits which can be used as a meta data */
	sge_buffer_traits_t traits;
} sge_buffer_create_info_t;

BEGIN_CPP_COMPATIBLE

SGE_API sge_buffer_t* sge_buffer_create(sge_driver_t* driver, sge_buffer_create_info_t* create_info);
SGE_API void sge_buffer_destroy(sge_buffer_t* buffer);
SGE_API void sge_buffer_release_resources(sge_buffer_t* buffer);

SGE_API void sge_buffer_copy_data(sge_buffer_t* buffer, u32 buffer_offset, void* data, u32 data_size);
SGE_API void* sge_buffer_map(sge_buffer_t* buffer);
SGE_API void sge_buffer_unmap(sge_buffer_t* buffer);

SGE_API void sge_buffer_set_traits(sge_buffer_t* buffer, const sge_buffer_traits_t* traits);
SGE_API sge_buffer_traits_t* sge_buffer_get_traits(sge_buffer_t* buffer);

END_CPP_COMPATIBLE