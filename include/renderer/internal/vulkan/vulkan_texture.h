
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

#include <renderer/internal/vulkan/vulkan_image.h>			 // vulkan_image_t
#include <renderer/internal/vulkan/vulkan_image_view.h> 	 // vulkan_image_view_t

typedef struct renderer_t renderer_t;

typedef enum vulkan_texture_type_t
{
	VULKAN_TEXTURE_TYPE_ALBEDO = 0, // to be used when the texture is meant to represent color
	VULKAN_TEXTURE_TYPE_NORMAL = 1, // to be used when the texture is meant to represent normals
	VULKAN_TEXTURE_TYPE_CUBE = 2,	// to be used when the texture (or textures) is meant to represent a cube map
} vulkan_texture_type_t;

typedef struct vulkan_texture_data_t
{
	void* data;				// pointer to the bytes (texel values)
	u32 width;				// width of the texture in texels
	u32 height;				// height of the texture in texels
	u32 depth; 				// depth of the texture in texels
	u8 channel_count;		// number of channels present in the texture, for now it must be 4 (RGBA)
} vulkan_texture_data_t;


typedef struct vulkan_texture_create_info_t
{
	u32 data_count; 								// individual loaded texture data count
	vulkan_texture_data_t* data;					// pointer to the loaded texture data(s)
	vulkan_texture_type_t type;						// type of the final vulkan texure i.e VULKAN_TEXTURE_TYPE_ALBEDO, VULKAN_TEXTURE_TYPE_NORMAL or VULKAN_TEXTURE_TYPE_CUBE
} vulkan_texture_create_info_t;

typedef struct vulkan_texture_t
{
	renderer_t* renderer; 							// pointer to the renderer_t object
	vulkan_image_t* image;							// pointer to the vulkan image object
	vulkan_image_view_t* image_view; 				// pointer to the vulkan image view object
	VkSampler image_sampler; 						// vulkan image sampler object
} vulkan_texture_t;

/*	
	description:
		creates a vulkan texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created vulkan texture object
 */
vulkan_texture_t* vulkan_texture_create(renderer_t* renderer, vulkan_texture_create_info_t* create_info);

/*
	description:
		destroys vulkan objects
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
void vulkan_texture_destroy(vulkan_texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
void vulkan_texture_release_resources(vulkan_texture_t* texture);
