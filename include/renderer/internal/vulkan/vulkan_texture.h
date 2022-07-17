
#pragma once

#include <vulkan/vulkan.h>
#include <renderer/defines.h>

#include <renderer/internal/vulkan/vulkan_image.h>			 // vulkan_image_t
#include <renderer/internal/vulkan/vulkan_image_view.h> 	 // vulkan_image_view_t
#include <renderer/internal/vulkan/vulkan_types.h> 			 // vulkan_render_target_technique_t

typedef enum vulkan_texture_type_t
{
	VULKAN_TEXTURE_TYPE_ALBEDO = 0, // to be used when the texture is meant to represent color on the display
	VULKAN_TEXTURE_TYPE_NORMAL = 1, // to be used when the texture is meant to represent normals
	VULKAN_TEXTURE_TYPE_COLOR = 2,  // when the texture meant to store/represent color but not on the display
	VULKAN_TEXTURE_TYPE_DEPTH = 3,  // when the texture meant to store a single channel, typically depth values
	VULKAN_TEXTURE_TYPE_CUBE = BIT32(4),	// to be used when the texture (or textures) is meant to represent a cube map
	VULKAN_TEXTURE_TYPE_RENDER_TARGET = BIT32(5),

	VULKAN_TEXTURE_TYPE_ALBEDO_CUBE = VULKAN_TEXTURE_TYPE_ALBEDO | VULKAN_TEXTURE_TYPE_CUBE,
	VULKAN_TEXTURE_TYPE_COLOR_CUBE = VULKAN_TEXTURE_TYPE_COLOR | VULKAN_TEXTURE_TYPE_CUBE,
	VULKAN_TEXTURE_TYPE_DEPTH_CUBE = VULKAN_TEXTURE_TYPE_DEPTH | VULKAN_TEXTURE_TYPE_CUBE,

	VULKAN_TEXTURE_TYPE_ALBEDO_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_ALBEDO_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
	VULKAN_TEXTURE_TYPE_COLOR_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_COLOR_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET,
	VULKAN_TEXTURE_TYPE_DEPTH_CUBE_RENDER_TARGET = VULKAN_TEXTURE_TYPE_DEPTH_CUBE | VULKAN_TEXTURE_TYPE_RENDER_TARGET
} vulkan_texture_type_t;

typedef enum vulkan_texture_usage_t
{
	VULKAN_TEXTURE_USAGE_NONE = 0,
	VULKAN_TEXTURE_USAGE_SAMPLED = BIT32(0),		// when the texture used in the shader
	VULKAN_TEXTURE_USAGE_TRANSFER_DST = BIT32(1), 	// when the texture to be filled with data
	VULKAN_TEXTURE_USAGE_TRANSFER_SRC = BIT32(2), 	// when the texture data has to be copied somewhere
	VULKAN_TEXTURE_USAGE_PRESENT = BIT32(3), 		// when the texture used as a post-processing intermediate image, and finally present it
	VULKAN_TEXTURE_USAGE_RENDER_TARGET = BIT32(4), 	// when the texture used as a render target to draw over it
	VULKAN_TEXTURE_USAGE_ATTACHMENT = BIT32(5), 	// when the texture used as an attachment in the framebuffer
	VULKAN_TEXTURE_USAGE_COPY = VULKAN_TEXTURE_USAGE_TRANSFER_SRC,
	VULKAN_TEXTURE_USAGE_PASTE = VULKAN_TEXTURE_USAGE_TRANSFER_DST
} vulkan_texture_usage_t;

typedef enum vulkan_texture_usage_stage_t
{
	VULKAN_TEXTURE_USAGE_STAGE_UNDEFINED = 0,
	VULKAN_TEXTURE_USAGE_STAGE_INITIAL,
	VULKAN_TEXTURE_USAGE_STAGE_USAGE,
	VULKAN_TEXTURE_USAGE_STAGE_FINAL
} vulkan_texture_usage_stage_t;

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
	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count; 		// ignored if this is a depth texture, otherwise it must be 4

	vulkan_texture_type_t type;						// type of the final vulkan texure i.e VULKAN_TEXTURE_TYPE_ALBEDO, VULKAN_TEXTURE_TYPE_NORMAL or VULKAN_TEXTURE_TYPE_CUBE
	vulkan_texture_usage_t initial_usage; 			// intial usage of the texture
	vulkan_texture_usage_t usage; 					// runtime usage of the texture
	vulkan_texture_usage_t final_usage; 			// final usage of the texture
	vulkan_render_target_technique_t technique; 	// ignored if this texture is not being used as a render target
} vulkan_texture_create_info_t;

typedef struct vulkan_texture_t
{
	vulkan_renderer_t* renderer; 							// pointer to the vulkan_renderer_t object
	vulkan_image_t image;									// vulkan image object

	/* image view: for sampling the texture or writing to the 2D render texture */
	vulkan_image_view_t image_view; 						// vulkan image view object
	
	VkSampler vo_image_sampler; 							// vulkan image sampler object

	// internal use only
	VkDescriptorType vo_descriptor_type;	 				// type of descriptor to which this texture would be bound

	/* image views: for writing/renderingg to the texture, typically Cube render texture */
	vulkan_image_view_t* image_views;
	u32 image_view_count;

	u32 width;
	u32 height;
	u32 depth;
	u8 channel_count;

	vulkan_texture_type_t type;

	vulkan_texture_usage_t initial_usage;
	vulkan_texture_usage_t usage;
	vulkan_texture_usage_t final_usage;
	vulkan_render_target_technique_t technique;

	vulkan_texture_usage_stage_t current_stage;
} vulkan_texture_t;

BEGIN_CPP_COMPATIBLE

/*	
	description:
		creates a vulkan texture of a type 'type'
	params:
		renderer: pointer to the renderer object
		create_info: pointer to the create info object
	returns:
		pointer to the newly created vulkan texture object
 */
RENDERER_API vulkan_texture_t* vulkan_texture_create(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info);
RENDERER_API void vulkan_texture_create_no_alloc(vulkan_renderer_t* renderer, vulkan_texture_create_info_t* create_info, vulkan_texture_t OUT texture);

/*
	description:
		destroys vulkan objects
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_texture_destroy(vulkan_texture_t* texture);

/*
	description:
		releases some extra heap memory allocated
	params:
		texture: pointer to the valid vulkan_texture_t object
	returns:
		nothing
 */
RENDERER_API void vulkan_texture_release_resources(vulkan_texture_t* texture);

/* transitions from initial usage state to the runtime usage state */
RENDERER_API void vulkan_texture_set_usage_stage(vulkan_texture_t* texture, vulkan_texture_usage_stage_t stage);

/* uploads the data to the GPU local memory */
RENDERER_API void vulkan_texture_upload_data(vulkan_texture_t* texture, u32 data_count, vulkan_texture_data_t* data);

END_CPP_COMPATIBLE
