

#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

enum
{
	VULKAN_DESCRIPTOR_SET_GLOBAL 	= 	0,
	VULKAN_DESCRIPTOR_SET_RENDER 	= 	1
	VULKAN_DESCRIPTOR_SET_MATERIAL 	= 	2,
	VULKAN_DESCRIPTOR_SET_OBJECT 	=	0
};

typedef struct vulkan_renderer_t vulkan_renderer_t;
typedef struct vulkan_texture_t vulkan_texture_t;
typedef struct vulkan_buffer_t vulkan_buffer_t;
typedef struct vulkan_pipeline_layout_t vulkan_pipeline_layout_t;

typedef struct vulkan_descriptor_set_create_info_t
{
	VkDescriptorPool pool;				// the pool from the set must be allocated
	VkDescriptorSetLayout layout;		// the layout of the set (contains a set of different types of resource descriptors)
} vulkan_descriptor_set_create_info_t;

typedef struct vulkan_descriptor_set_t
{
	VkDescriptorSet handle;
	VkDescriptorPool pool; //the pool from it has been allocated
} vulkan_descriptor_set_t;

BEGIN_CPP_COMPATIBLE

/* 
	description:
		allocates vulkan_descriptor_set_t object on the heap, zeros out the memory and then returns it
	params:
		nothing
	returns:
		pointer to newly created vulkan_descriptor_set_t object on the heap
 */
RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_new();

/* 
	description:
		creates vulkan descriptor set (VkDescriptorSet) object,
		it internally calls vulkan_descriptor_set_new()
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_descriptor_set_create_info_t object
	returns:
		pointer to newly created vulkan_descriptor_set_t object on the heap
 */
RENDERER_API vulkan_descriptor_set_t* vulkan_descriptor_set_create(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info);


/* 
	description:
		creates a descriptor set (VkDescriptorSet) object,
	params:
		renderer: pointer to the vulkan_renderer_t object
		create_info: pointer to the vulkan_descriptor_set_create_info_t object
		set: pointer to already allocated (stack or heap) vulkan_descriptor_set_t object,
				make sure to initialize the memory to zero or using vulkan_descriptor_set_new() to allocate the object on heap
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_create_no_alloc(vulkan_renderer_t* renderer,  vulkan_descriptor_set_create_info_t* create_info, vulkan_descriptor_set_t* set);

/*
	description:
		destroys (VkFreeDescriptorSets) the vulkan objects (VkDescriptorSet)
	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_destroy(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer);

/*
	description:
		releases heap allocated memory for vulkan_descriptor_set_t object (which must have been allocated/created by vulkan_descriptor_set_new or vulkan_descriptor_set_create)
		if the vulkan_descriptor_set_t object has been allocated on stack then don't call this function.
	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create or allocated by vulkan_descriptor_set_new)
	returns:
		nothing
 */
RENDERER_API void vulkan_descriptor_set_release_resources(vulkan_descriptor_set_t* set);

/*
 	description:
 		binds (vkCmdBindDescriptorSets) the descriptor set to the pipline
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		set_number: set number
 		pipeline_layout: compatible pipeline layout, which has been created with the same descriptor set layout from which this descriptor set has been created
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_bind(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 set_number, vulkan_pipeline_layout_t* pipeline_layout);

/*
 	description:
 		updates the descriptor at binding "binding_index" with a texture resource
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		binding_index: binding number/index of the resource descriptor as defined in the descriptor set layout when this descriptor set has been created
		texture: pointer to a valid vulkan_texture_t object
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_write_texture(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 binding_index, vulkan_texture_t* texture);


/*
 	description:
 		updates the descriptor at binding "binding_index" with a vulkan_buffer resource as uniform buffer
 	params:
		set: pointer to a valid vulkan_descriptor_set_t object (must have been created by calling vulkan_descriptor_set_create_no_alloc or vulkan_descriptor_set_create)
		renderer: pointer to the vulkan_renderer_t object
		binding_index: binding number/index of the resource descriptor as defined in the descriptor set layout when this descriptor set has been created
		texture: pointer to a valid vulkan_buffer_t object
 	returns:
 		nothing
 */
RENDERER_API void vulkan_descriptor_set_write_uniform_buffer(vulkan_descriptor_set_t* set, vulkan_renderer_t* renderer, u32 binding_index, vulkan_buffer_t* buffer);


END_CPP_COMPATIBLE
