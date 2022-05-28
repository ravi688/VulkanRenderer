
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_defines.h>  					// vkCall
#include <renderer/internal/vulkan/vulkan_types.h>	 					// vulkan_shader_type_t
#include <shader_compiler/compiler.h>	 								// shader compiler types
#include <renderer/memory_allocator.h>


RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_new()
{
	vulkan_descriptor_set_layout_t* layout = heap_new(vulkan_descriptor_set_layout_t);
	memset(layout, 0, sizeof(vulkan_descriptor_set_layout_t));
	return layout;
}

RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count)
{
	vulkan_descriptor_set_layout_t* layout = vulkan_descriptor_set_layout_new();
	vulkan_descriptor_set_layout_create_no_alloc(renderer, bindings, binding_count, layout);
	return layout;
}

RENDERER_API void vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_resource_descriptor_t* descriptors, u32 descriptor_count, vulkan_descriptor_set_layout_t OUT layout)
{
	if(descriptor_count == 0)
	{
		layout->vo_handle = VK_NULL_HANDLE;
		layout->renderer = renderer;
		return;
	}
	// allocate memory
	VkDescriptorSetLayoutBinding* bindings = heap_newv(VkDescriptorSetLayoutBinding, descriptor_count);
	memset(bindings, 0, sizeof(VkDescriptorSetLayoutBinding) * descriptor_count);

	u32 binding_count = 0;
	for(u32 i = 0; i < descriptor_count; i++)
	{
		vulkan_shader_resource_descriptor_t* descriptor = &descriptors[i];

		// if this is a push constant range or a vertex attribute then skip it
		if(descriptor->is_push_constant || descriptor->is_attribute)
			continue;

		VkDescriptorSetLayoutBinding* binding = &bindings[binding_count];
		binding->binding = descriptor->binding_number;
		binding->descriptorCount = 1;	//for now we are not using arrays
		binding->stageFlags = 0;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
			binding->stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
			binding->stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
			binding->stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if(descriptor->stage_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
			binding->stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		switch(descriptor->handle.type)
		{
			case SHADER_COMPILER_BLOCK:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
			
			case SHADER_COMPILER_SAMPLER_2D:
			case SHADER_COMPILER_SAMPLER_3D:
			case SHADER_COMPILER_SAMPLER_CUBE:
				binding->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
			
			default:
				LOG_FETAL_ERR("Cannot create set layout binding for the type \"%u\", because it is not recognized\n", descriptor->handle.type);
			break;
		}
		binding_count++;
	}
	vulkan_descriptor_set_layout_create_no_alloc(renderer, bindings, binding_count, layout);
	heap_free(bindings);	
}

RENDERER_API vulkan_descriptor_set_layout_t* vulkan_descriptor_set_layout_create_from_resource_descriptors(vulkan_renderer_t* renderer, vulkan_shader_resource_descriptor_t* bindings, u32 binding_count)
{
	vulkan_descriptor_set_layout_t* layout = vulkan_descriptor_set_layout_new();
	vulkan_descriptor_set_layout_create_from_resource_descriptors_no_alloc(renderer, bindings, binding_count, layout);
	return layout;
}

RENDERER_API void vulkan_descriptor_set_layout_create_no_alloc(vulkan_renderer_t* renderer, VkDescriptorSetLayoutBinding* bindings, u32 binding_count, vulkan_descriptor_set_layout_t OUT layout)
{
	if(binding_count == 0)
	{
		layout->vo_handle = VK_NULL_HANDLE;
		layout->renderer = renderer;
		return;
	}

	VkDescriptorSetLayoutCreateInfo layout_create_info =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = binding_count,
		.pBindings = bindings
	};
	VkDescriptorSetLayout set_layout;
	vkCall(vkCreateDescriptorSetLayout(renderer->logical_device->vo_handle, &layout_create_info, NULL, &set_layout));

	layout->renderer = renderer;
	layout->vo_handle = set_layout;
}

RENDERER_API void vulkan_descriptor_set_layout_destroy(vulkan_descriptor_set_layout_t* layout)
{
	if(layout->vo_handle != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(layout->renderer->logical_device->vo_handle, layout->vo_handle, NULL);
		layout->vo_handle = VK_NULL_HANDLE;
	}
}

RENDERER_API void vulkan_descriptor_set_layout_release_resources(vulkan_descriptor_set_layout_t* layout)
{
	heap_free(layout);
}

