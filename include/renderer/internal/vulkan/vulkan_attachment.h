
#pragma once

#include <renderer/defines.h>
#include <vulkan/vulkan.h>

#include <renderer/internal/vulkan/vulkan_image.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>

typedef enum vulkan_attachment_next_pass_usage_t
{
	/* this attachment is of no use after completing this render pass and intermediate subpasses */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_NONE = 0,
	/* if this attachment would be used as a Input attachment for the next subpasses */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_INPUT = 1,
	/* if this attachment would be used as a Sampled Image for the next renderpass */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_SAMPLED = 1 << 1,
	/* if this attachment would be used as a Present Image for the presentation engine */
	VULKAN_ATTACHMENT_NEXT_PASS_USAGE_PRESENT = 1 << 2
} vulkan_attachment_next_pass_usage_t;

typedef enum vulkan_attachment_type_t
{
	/* for color attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_COLOR = 0,
	/* for depth only attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_DEPTH = 1 << 1,
	/* for stencil only attachment, format would be automatically deduced */
	VULKAN_ATTACHMENT_TYPE_STENCIL = 1 << 2,
	/* for depth & stencil attachment, format would be automatically decuded */
	VULKAN_ATTACHMENT_TYPE_DEPTH_STENCIL = VULKAN_ATTACHMENT_TYPE_DEPTH | VULKAN_ATTACHMENT_TYPE_STENCIL
} vulkan_attachment_type_t;

typedef struct vulkan_attachment_create_info_t
{
	/* width and height of the attachment */
	u32 width, height;
	/* type of the attachment to be created */
	vulkan_attachment_type_t type;
	/* usage of this attachment to be created */
	vulkan_attachment_next_pass_usage_t next_pass_usage;
	/* format of the attachment to be created */
	VkFormat format;
} vulkan_attachment_create_info_t;

/* NOTE: this should be in sync with vulkan_texture_t */
typedef struct vulkan_attachment_t
{
	vulkan_renderer_t* renderer;
	/* image for this attachment */
	vulkan_image_t image;
	/* image view for this attachment */
	vulkan_image_view_t image_view;

	/* sampler for this attachment if it has to be used in the next render pass, otherwise VK_NULL_HANDLE */
	VkSampler sampler;

	/* type of the descriptor to which this attachment to be bound as vulkan_texture_t */
	VkDescriptorType vo_descriptor_type;
} vulkan_attachment_t;


BEGIN_CPP_COMPATIBLE

/* constructors & destructors */
RENDERER_API vulkan_attachment_t* vulkan_attachment_new();
RENDERER_API vulkan_attachment_t* vulkan_attachment_create(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info);
RENDERER_API void vulkan_attachment_create_no_alloc(vulkan_renderer_t* renderer, vulkan_attachment_create_info_t* create_info, vulkan_attachment_t OUT attachment);
RENDERER_API void vulkan_attachment_destroy(vulkan_attachment_t* attachment);
RENDERER_API void vulkan_attachment_release_resources(vulkan_attachment_t* attachment);

/* getters */
RENDERER_API vulkan_image_t* vulkan_attachment_get_image(vulkan_attachment_t* attachment);
RENDERER_API vulkan_image_view_t* vulkan_attachment_get_image_view(vulkan_attachment_t* attachment);

END_CPP_COMPATIBLE