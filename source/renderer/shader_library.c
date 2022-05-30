#include <renderer/shader_library.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_types.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/memory_allocator.h>

#include <disk_manager/file_reader.h>
#include <shader_compiler/compiler.h>

#include <string.h> 		// strcpy

/* constructors & destructors */
RENDERER_API shader_library_t* shader_library_new()
{
	return vulkan_shader_library_new();
}

RENDERER_API shader_library_t* shader_library_create(renderer_t* renderer)
{
	return vulkan_shader_library_create(renderer->vulkan_handle);
}

RENDERER_API shader_library_t* shader_library_load_folder(renderer_t* renderer, const char* folder_path)
{
	return vulkan_shader_library_load_folder(renderer->vulkan_handle, folder_path);
}

RENDERER_API void shader_library_destroy(shader_library_t* library)
{
	vulkan_shader_library_destroy(library);
}

RENDERER_API void shader_library_release_resources(shader_library_t* library)
{
	vulkan_shader_library_release_resources(library);
}


/* logic functions */

static vulkan_shader_resource_descriptor_t* create_material_set_binding()
{
	vulkan_shader_resource_descriptor_t* binding = heap_new(vulkan_shader_resource_descriptor_t);
	memzero(binding, vulkan_shader_resource_descriptor_t);
	struct_descriptor_t* descriptor = &binding->handle;
	strcpy(descriptor->name, "parameters");
	descriptor->type = SHADER_COMPILER_BLOCK;
	descriptor->field_count = 1;
	AUTO field = heap_new(struct_field_t);
	strcpy(field->name, "color");
	field->type = SHADER_COMPILER_VEC4;
	field->size = sizeof(float) * 4;
	field->alignment = 16;
	descriptor->fields = field;
	struct_descriptor_recalculate(descriptor);

	binding->is_uniform = true;
	binding->set_number = VULKAN_DESCRIPTOR_SET_MATERIAL;
	binding->binding_number = VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES;
	binding->stage_flags = (1 << VULKAN_SHADER_TYPE_VERTEX) | (1 << VULKAN_SHADER_TYPE_FRAGMENT);
	return binding;
}

static vulkan_vertex_info_t* create_vertex_info()
{
	vulkan_vertex_info_t* info = heap_new(vulkan_vertex_info_t);
	memzero(info, vulkan_vertex_info_t);
	info->binding = VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING;
	info->input_rate = VK_VERTEX_INPUT_RATE_VERTEX;
	info->size = sizeof(float) * 3;
	info->attribute_count = 1;
	info->attribute_locations = heap_new(u16);
	info->attribute_locations[0] = VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION;
	info->attribute_formats = heap_new(VkFormat);
	info->attribute_formats[0] = VK_FORMAT_R32G32B32_SFLOAT;
	info->attribute_offsets = heap_new(u32);
	info->attribute_offsets[0] = 0;
	return info;
}

static GraphicsPipelineSettings* create_pipeline_settings(vulkan_renderer_t* renderer)
{
	GraphicsPipelineSettings* settings = heap_new(GraphicsPipelineSettings);
	memzero(settings, GraphicsPipelineSettings);
	
	VkPipelineColorBlendAttachmentState* color_attachment = heap_new(VkPipelineColorBlendAttachmentState);
	color_attachment[0] = (VkPipelineColorBlendAttachmentState)
	{
		.blendEnable = VK_FALSE,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT	
	};

	settings->colorblend = (VkPipelineColorBlendStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments = color_attachment
	};

	settings->inputassembly = (VkPipelineInputAssemblyStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	/* Viewport configuration */
	VkViewport* viewport = heap_new(VkViewport);
	viewport[0] = (VkViewport)
	{
		.x = 0,
		.y = 0,
		.width = renderer->swapchain->vo_image_extent.width,
		.height = renderer->swapchain->vo_image_extent.height,
		.minDepth = 0,
		.maxDepth = 1.0f
	};
	VkRect2D* scissor = heap_new(VkRect2D);
	scissor[0] = (VkRect2D)
	{
		.offset = { 0, 0 },
		.extent = renderer->swapchain->vo_image_extent
	};

	settings->viewport = (VkPipelineViewportStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = viewport,
		.scissorCount = 1,
		.pScissors = scissor
	};

	/* Rasterization configuration */
	settings->rasterization = (VkPipelineRasterizationStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		.lineWidth = 1.0f
	};

	/* Multisampling */
	settings->multisample = (VkPipelineMultisampleStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};

	/* Depth stencil configuration */
	settings->depthstencil = (VkPipelineDepthStencilStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_FALSE,
		.depthWriteEnable = VK_FALSE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
		.stencilTestEnable = VK_FALSE,
		.depthBoundsTestEnable = VK_FALSE
	};

	settings->dynamic = (VkPipelineDynamicStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO
	};
	return settings;
}

static vulkan_render_pass_description_t* create_render_pass_description(vulkan_renderer_t* renderer)
{
	vulkan_render_pass_description_t* render_pass = heap_new(vulkan_render_pass_description_t);
	memzero(render_pass, vulkan_render_pass_description_t);

	render_pass->attachment_count = 1;
	render_pass->attachments = heap_new(vulkan_attachment_type_t);
	render_pass->attachments[0] = VULKAN_ATTACHMENT_TYPE_COLOR;
	
	render_pass->subpass_count = 1;
	vulkan_subpass_description_t* subpass = render_pass->subpass_descriptions = heap_new(vulkan_subpass_description_t);
	memzero(subpass, vulkan_subpass_description_t);
	
	vulkan_graphics_pipeline_description_t* pipeline = &subpass->pipeline_description;
	pipeline->settings = create_pipeline_settings(renderer);

	pipeline->spirv_code_count = 2;
	vulkan_spirv_code_t* codes = pipeline->spirv_codes = heap_newv(vulkan_spirv_code_t, 2);
	BUFFER* vertex_shader = load_binary_from_file("shaders/presets/unlit/color/color.vert.spv");
	BUFFER* fragment_shader = load_binary_from_file("shaders/presets/unlit/color/color.frag.spv");
	codes[0].spirv = buf_get_ptr(vertex_shader);
	codes[0].length = buf_get_element_count(vertex_shader);
	codes[0].type = VULKAN_SHADER_TYPE_VERTEX;
	codes[1].spirv = buf_get_ptr(fragment_shader);
	codes[1].length = buf_get_element_count(fragment_shader);
	codes[1].type = VULKAN_SHADER_TYPE_FRAGMENT;

	// TODO:
	// buf_free(vertex_shader);
	// buf_free(fragment_shader);

	subpass->color_attachment_count = 1;
	subpass->color_attachments = heap_new(u32);
	subpass->color_attachments[0] = 0;

	subpass->depth_stencil_attachment = U32_MAX; // invalid
	
	render_pass->type = VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET;
	return render_pass;
}

static vulkan_shader_create_info_t* get_create_info_from_preset(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_t* create_info = heap_new(vulkan_shader_create_info_t);
	create_info->material_set_binding_count = 1;
	create_info->material_set_bindings = create_material_set_binding();
	create_info->vertex_info_count = 1;
	create_info->vertex_infos = create_vertex_info();
	create_info->render_pass_description_count = 1;
	create_info->render_pass_descriptions = create_render_pass_description(renderer);
	return create_info;
}

RENDERER_API shader_handle_t shader_library_create_shader_from_preset(shader_library_t* library, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_t* create_info = get_create_info_from_preset(library->renderer, preset);
	return vulkan_shader_library_create_shader(library, create_info, "Built-In");
}

RENDERER_API shader_handle_t shader_library_create_shader(shader_library_t* library, shader_create_info_t* create_info, const char* shader_name)
{
	return vulkan_shader_library_create_shader(library, CAST_TO(vulkan_shader_create_info_t*, create_info), shader_name);
}

RENDERER_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path)
{
	return vulkan_shader_library_load_shader(library, file_path);
}

RENDERER_API bool shader_library_destroy_shader(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_destroy_shader(library, shader_name);
}

RENDERER_API bool shader_library_destroy_shaderH(shader_library_t* library, shader_handle_t handle)
{
	return vulkan_shader_library_destroy_shaderH(library, handle);
}

/* getters */

RENDERER_API shader_handle_t shader_library_get_handle(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_get_handle(library, shader_name);
}

RENDERER_API const char* shader_library_get_nameH(shader_library_t* library, shader_handle_t handle)
{
	return vulkan_shader_library_get_nameH(library, handle);
}

RENDERER_API shader_t* shader_library_getH(shader_library_t* library, shader_handle_t handle)
{
	return vulkan_shader_library_getH(library, handle);
}

RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name)
{
	return vulkan_shader_library_get(library, shader_name);
}
