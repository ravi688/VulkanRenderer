
#include <renderer/internal/vulkan/vulkan_framebuffer.h>
#include <renderer/internal/vulkan/vulkan_render_pass.h>
#include <renderer/internal/vulkan/vulkan_attachment.h>
#include <renderer/internal/vulkan/vulkan_image_view.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>
#include <renderer/internal/vulkan/vulkan_allocator.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

typedef struct image_view_u32_pair_t
{
	/* replacment image view at that index */
	vulkan_image_view_t* view;
	/* index of the VkImageView in the list of image views in the framebuffer */
	u32 index;
} image_view_u32_pair_t;

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_new(memory_allocator_t* allocator)
{
	vulkan_framebuffer_t* framebuffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_FRAMEBUFFER, vulkan_framebuffer_t);
	memzero(framebuffer, vulkan_framebuffer_t);
	return framebuffer;
}

RENDERER_API vulkan_framebuffer_t* vulkan_framebuffer_create(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info)
{
	vulkan_framebuffer_t* framebuffer = vulkan_framebuffer_new(renderer->allocator);
	vulkan_framebuffer_create_no_alloc(renderer, create_info, framebuffer);
	return framebuffer;
}

RENDERER_API void vulkan_framebuffer_create_no_alloc(vulkan_renderer_t* renderer, vulkan_framebuffer_create_info_t* create_info, vulkan_framebuffer_t OUT framebuffer)
{
	memzero(framebuffer, vulkan_framebuffer_t);
	framebuffer->renderer = renderer;
	framebuffer->vo_handle = VK_NULL_HANDLE;
	framebuffer->pass = create_info->render_pass;
	framebuffer->id = create_info->id;
	framebuffer->image_views = memory_allocator_alloc_obj_array(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VKAPI_IMAGE_VIEW_ARRAY, VkImageView, framebuffer->pass->attachment_count);
	framebuffer->modification_stack = buf_create(sizeof(image_view_u32_pair_t), 2, 0);

	vulkan_framebuffer_refresh_info_t info = 
	{
		.width = renderer->window->width,
		.height = renderer->window->height,
		.is_update_image_views = true
	};
	vulkan_framebuffer_refresh(framebuffer, &info);
}

RENDERER_API void vulkan_framebuffer_destroy(vulkan_framebuffer_t* framebuffer)
{
	vkDestroyFramebuffer(framebuffer->renderer->logical_device->vo_handle, framebuffer->vo_handle, VULKAN_ALLOCATION_CALLBACKS(framebuffer->renderer));
	framebuffer->vo_handle = VK_NULL_HANDLE;
	buf_clear(&framebuffer->modification_stack, NULL);
}

RENDERER_API void vulkan_framebuffer_release_resources(vulkan_framebuffer_t* framebuffer)
{
	memory_allocator_dealloc(framebuffer->renderer->allocator, framebuffer->image_views);
	buf_free(&framebuffer->modification_stack);
	// heap_free(framebuffer);
}

static void vulkan_framebuffer_recreate(vulkan_framebuffer_t* framebuffer)
{
	vulkan_framebuffer_refresh_info_t info = 
	{
		.width = framebuffer->width,
		.height = framebuffer->height,
		.is_update_image_views = false
	};
	vulkan_framebuffer_refresh(framebuffer, &info);
}

RENDERER_API void vulkan_framebuffer_set_supplementary(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view)
{
	if(!framebuffer->is_supplementary_supported) return;

	// for now supplementary attachment count must be one; just for swapchain color attachment
	_debug_assert__(framebuffer->pass->supplementary_attachment_bucket_count == 1);
	
	framebuffer->image_views[0] = view->vo_handle;
	image_view_u32_pair_t pair = { view, 0 };
	buf_push(&framebuffer->modification_stack, &pair);
	vulkan_framebuffer_recreate(framebuffer);
}

static bool comparer(void* view, void* pair)
{
	return DREF_TO(u32, view) == (CAST_TO(image_view_u32_pair_t*, pair)->index);
}

RENDERER_API void vulkan_framebuffer_restore_supplementary(vulkan_framebuffer_t* framebuffer)
{
	if(!framebuffer->is_supplementary_supported) return;
	vulkan_render_pass_t* render_pass = framebuffer->pass;

	// for now supplementary attachment count must be one; just for swapchain color attachment
	_debug_assert__(render_pass->supplementary_attachment_bucket_count == 1);
	
	if(buf_get_element_count(&framebuffer->modification_stack) > 0)
	{
		u32 index = 0;
		IF_DEBUG_MODE (bool result = ) buf_remove(&framebuffer->modification_stack, &index, comparer);
		_debug_assert__(result == true);
	}

	framebuffer->image_views[0] = render_pass->vo_supplementary_attachments[framebuffer->id * render_pass->supplementary_attachment_bucket_depth];
	vulkan_framebuffer_recreate(framebuffer);
}

RENDERER_API void vulkan_framebuffer_set_depth(vulkan_framebuffer_t* framebuffer, vulkan_image_view_t* view)
{
	if(!framebuffer->is_depth_supported) return;

	_debug_assert__(framebuffer->depth_index != U32_MAX);
	framebuffer->image_views[framebuffer->depth_index] = view->vo_handle;
	image_view_u32_pair_t pair = { view, framebuffer->depth_index };
	buf_push(&framebuffer->modification_stack, &pair);
	vulkan_framebuffer_recreate(framebuffer);
}

RENDERER_API void vulkan_framebuffer_restore_depth(vulkan_framebuffer_t* framebuffer)
{
	if(!framebuffer->is_depth_supported) return;

	_debug_assert__(framebuffer->depth_index != U32_MAX);

	u32 index = framebuffer->depth_index;
	_debug_assert__(index >= framebuffer->pass->supplementary_attachment_bucket_count);
	
	if(buf_get_element_count(&framebuffer->modification_stack) > 0)
	{
		IF_DEBUG_MODE (bool result = ) buf_remove(&framebuffer->modification_stack, &index, comparer);
		_debug_assert__(result == true);
	}

	framebuffer->image_views[index] = framebuffer->pass->allocated_attachments[CAST_TO(s32, index) - framebuffer->pass->supplementary_attachment_bucket_count].image_view.vo_handle;
	vulkan_framebuffer_recreate(framebuffer);
}


RENDERER_API void vulkan_framebuffer_refresh(vulkan_framebuffer_t* framebuffer, vulkan_framebuffer_refresh_info_t* info)
{
	if(framebuffer->vo_handle != VK_NULL_HANDLE)
		vulkan_framebuffer_destroy(framebuffer);
	
	AUTO render_pass = framebuffer->pass;

	if(info->is_update_image_views)
	{
		framebuffer->is_supplementary_supported = (render_pass->vo_supplementary_attachments != NULL)
											&& (render_pass->supplementary_attachment_bucket_count != 0)
											&& (render_pass->supplementary_attachment_bucket_depth != 0);
    	framebuffer->depth_index = U32_MAX;
   	 	framebuffer->is_depth_supported = false;

    	// for now we must not touch the depth only passes because they are being sampled in other passes
   	 	// or if(render_pass->type == VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET)
   	

   	 	// TODO: Revert this change after implementing and testing the point light shadows
   		// if(render_pass->required_framebuffer_count == renderer->swapchain->image_count)
   		// {

    		/* NOTE: attachment count for a render pass = allocated attachment count (internal attachments) + suplementary attachment count (external attachments) 
    	 	* since because the suplementary attachments would always be swapchain presentable sRGB (color) attachments, 
    	 	* we only have to check for internal allocated attachments if they are depth attachments. */
			u32 allocated_attachment_count = render_pass->allocated_attachment_count;
#ifdef GLOBAL_DEBUG
			u32 depth_count = 0;
#endif
			for(u32 i = 0; i < allocated_attachment_count; i++)
			{
				if(render_pass->allocated_attachments[i].image.vo_format == VK_FORMAT_D32_SFLOAT)
				{
#ifdef GLOBAL_DEBUG
					depth_count++;
#endif
					framebuffer->is_depth_supported = true;
					framebuffer->depth_index = i + render_pass->supplementary_attachment_bucket_count;
				}
			}

#ifdef GLOBAL_DEBUG
			_debug_assert_wrn__(depth_count <= 1);
#endif
		// }

		/* copy the VkImageView(s) from render pass to the framebuffer */
		_debug_assert__(render_pass->supplementary_attachment_bucket_count <= 1);
		for(u32 i = 0; i < render_pass->supplementary_attachment_bucket_count; i++)
			framebuffer->image_views[i] = render_pass->vo_supplementary_attachments[framebuffer->id + i * render_pass->supplementary_attachment_bucket_depth];

		s32 num_created_image_views = CAST_TO(s32, render_pass->attachment_count) - CAST_TO(s32, render_pass->supplementary_attachment_bucket_count);;
		for(s32 i = 0; i < num_created_image_views; i++)
			framebuffer->image_views[i + render_pass->supplementary_attachment_bucket_count] = render_pass->allocated_attachments[i].image_view.vo_handle;

		/* reapply the modifications */
		u32 count = buf_get_element_count(&framebuffer->modification_stack);
		for(u32 i = 0; i < count; i++)
		{
			AUTO pair = buf_get_ptr_at_typeof(&framebuffer->modification_stack, image_view_u32_pair_t, i);
			framebuffer->image_views[pair->index] = pair->view->vo_handle;
		}
	}

	VkFramebufferCreateInfo framebuffer_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = render_pass->vo_handle,
		.attachmentCount = render_pass->attachment_count,
		.pAttachments = framebuffer->image_views,
		.width = info->width,
		.height = info->height,
		.layers = 1
	};
	vkCall(vkCreateFramebuffer(framebuffer->renderer->logical_device->vo_handle, &framebuffer_create_info, VULKAN_ALLOCATION_CALLBACKS(framebuffer->renderer), &framebuffer->vo_handle));

	/* update the width */
	framebuffer->width = info->width;
	/* update the height */
	framebuffer->height = info->height;
}
