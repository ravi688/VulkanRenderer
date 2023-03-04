#include <renderer/internal/vulkan/vulkan_render_pass_instance.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_render_pass_pool.h>
#include <renderer/memory_allocator.h>

RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_new(memory_allocator_t* allocator)
{
	vulkan_render_pass_instance_t* instance = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_INSTANCE, vulkan_render_pass_instance_t);
	memzero(instance, vulkan_render_pass_instance_t);
	return instance;
}

RENDERER_API vulkan_render_pass_instance_t* vulkan_render_pass_instance_create(vulkan_renderer_t* renderer, vulkan_render_pass_instance_create_info_t* create_info)
{
	vulkan_render_pass_instance_t* instance = vulkan_render_pass_instance_new(renderer->allocator);
	vulkan_render_pass_instance_create_no_alloc(renderer, create_info, instance);
	return instance;
}

RENDERER_API void vulkan_render_pass_instance_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_instance_create_info_t* create_info, vulkan_render_pass_instance_t OUT instance)
{
	memzero(instance, vulkan_render_pass_instance_t);
	instance->renderer = renderer;
	instance->render_pass = vulkan_render_pass_pool_getH(renderer->render_pass_pool, create_info->render_pass_handle);
	vulkan_descriptor_set_create_info_t set_create_info = 
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &instance->render_pass->render_set_layout
	};
	vulkan_descriptor_set_create_no_alloc(renderer, &set_create_info, &instance->render_set);
	instance->render_pass->instance_count++;
}

RENDERER_API void vulkan_render_pass_instance_destroy(vulkan_render_pass_instance_t* instance)
{
	vulkan_descriptor_set_destroy(&instance->render_set);
	_debug_assert__(instance->render_pass->instance_count > 0);
	instance->render_pass->instance_count--;
}

RENDERER_API void vulkan_render_pass_instance_release_resources(vulkan_render_pass_instance_t* instance)
{

}


RENDERER_API void vulkan_render_pass_instance_set_clear_indirect(vulkan_render_pass_instance_t* instance, color_t color, float depth, VkClearValue* indirect_buffer)
{
	vulkan_render_pass_set_clear_indirect(instance->render_pass, color, depth, indirect_buffer);
}

RENDERER_API void vulkan_render_pass_instance_set_clear(vulkan_render_pass_instance_t* instance, color_t color, float depth)
{
	vulkan_render_pass_set_clear(instance->render_pass, color, depth);
}

RENDERER_API void vulkan_render_pass_instance_begin(vulkan_render_pass_instance_t* instance, u32 framebuffer_index, vulkan_camera_t* camera)
{
	vulkan_render_pass_begin(instance->render_pass, framebuffer_index, camera);
}

RENDERER_API void vulkan_render_pass_instance_end(vulkan_render_pass_instance_t* instance)
{
	vulkan_render_pass_end(instance->render_pass);
}

RENDERER_API void vulkan_render_pass_instance_next(vulkan_render_pass_instance_t* instance)
{
	vulkan_render_pass_next(instance->render_pass);
}

RENDERER_API void vulkan_render_pass_instance_refresh(vulkan_render_pass_instance_t* instance, vulkan_render_pass_instance_refresh_info_t* info)
{
	vulkan_render_pass_refresh(instance->render_pass, info);
}
