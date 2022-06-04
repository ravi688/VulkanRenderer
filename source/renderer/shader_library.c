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

#define UNSUPPORTED_PRESET(preset) LOG_FETAL_ERR("Unsupported shader preset type %u\n", preset)

static void* create_element(BUFFER* list)
{
	buf_push_pseudo(list, 1);
	return buf_peek_ptr(list);	
}

static void add_sampler2D(BUFFER* list, const char* name, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_descriptor_t* binding = create_element(list);
	binding->is_opaque = true;
	binding->set_number = set_number;
	binding->binding_number = binding_number;
	binding->stage_flags = (1 << VULKAN_SHADER_TYPE_VERTEX) | (1 << VULKAN_SHADER_TYPE_FRAGMENT);

	struct_descriptor_t* descriptor = &binding->handle;
	strcpy(descriptor->name, name);
	descriptor->type = SHADER_COMPILER_SAMPLER_2D;	
}

static void begin_uniform(BUFFER* list, const char* name, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_descriptor_t* binding = create_element(list);
	binding->is_uniform = true;
	binding->set_number = set_number;
	binding->binding_number = binding_number;
	binding->stage_flags = (1 << VULKAN_SHADER_TYPE_VERTEX) | (1 << VULKAN_SHADER_TYPE_FRAGMENT);

	struct_descriptor_t* descriptor = &binding->handle;
	strcpy(descriptor->name, name);
	descriptor->type = SHADER_COMPILER_BLOCK;
	BUFFER* fields = heap_new(BUFFER);
	*fields = buf_create(sizeof(struct_field_t), 0, 0);
	descriptor->fields = CAST_TO(struct_field_t*, fields);
}

static void end_uniform(BUFFER* list)
{
	vulkan_shader_resource_descriptor_t* binding = buf_peek_ptr(list);
	struct_descriptor_t* descriptor = &binding->handle;
	BUFFER* fields = CAST_TO(BUFFER*, descriptor->fields);
	descriptor->fields = buf_get_ptr(fields);
	descriptor->field_count = buf_get_element_count(fields);
	heap_free(fields);
	struct_descriptor_recalculate(descriptor);
}

static struct_field_t* create_field(BUFFER* list)
{
	vulkan_shader_resource_descriptor_t* binding = buf_peek_ptr(list);
	struct_descriptor_t* descriptor = &binding->handle;
	BUFFER* fields = CAST_TO(BUFFER*, descriptor->fields);
	buf_push_pseudo(fields, 1);
	return buf_peek_ptr(fields);
}

static void add_color_field(BUFFER* list, const char* name)
{
	struct_field_t* field = create_field(list);
	strcpy(field->name, name);
	field->type = SHADER_COMPILER_VEC4;
	field->size = sizeof(float) * 4;
	field->alignment = 16;
}

static vulkan_shader_resource_descriptor_t* create_material_set_binding(shader_library_shader_preset_t preset, u32 OUT binding_count)
{
	BUFFER bindings = buf_new(vulkan_shader_resource_descriptor_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				add_color_field(&bindings, "color");
			end_uniform(&bindings);	
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
			begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				add_color_field(&bindings, "color");
			end_uniform(&bindings);	
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				add_color_field(&bindings, "color");
			end_uniform(&bindings);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	};

	OUT binding_count = buf_get_element_count(&bindings);
	buf_fit(&bindings);
	return buf_get_ptr(&bindings);
}

#define create_buffer(type) __create_buffer(sizeof(type))
static BUFFER* __create_buffer(u32 size)
{
	BUFFER* buffer = heap_new(BUFFER);
	*buffer = buf_create(size, 1, 0);
	return buffer;
}

static void begin_vertex_binding(BUFFER* list, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	vulkan_vertex_info_t* info = create_element(list);
	info->binding = binding_number;
	info->input_rate = input_rate;
	info->size = stride;

	info->attribute_locations = CAST_TO(u16*, create_buffer(u16));
	info->attribute_formats = CAST_TO(VkFormat*, create_buffer(VkFormat));
	info->attribute_offsets = CAST_TO(u32*, create_buffer(u32));
}

static void add_vertex_attribute(BUFFER* list, u16 location, VkFormat format, u32 offset)
{
	vulkan_vertex_info_t* info = buf_peek_ptr(list);
	buf_push_auto(CAST_TO(BUFFER*, info->attribute_locations), location);
	buf_push_auto(CAST_TO(BUFFER*, info->attribute_formats), format);
	buf_push_auto(CAST_TO(BUFFER*, info->attribute_offsets), offset);
}

static void end_vertex_binding(BUFFER* list)
{
	vulkan_vertex_info_t* info = buf_peek_ptr(list);
	BUFFER* buffer = CAST_TO(BUFFER*, info->attribute_locations);
	info->attribute_count = buf_get_element_count(buffer);
	info->attribute_locations = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, info->attribute_formats);
	info->attribute_formats = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, info->attribute_offsets);
	info->attribute_offsets = buf_get_ptr(buffer);
	heap_free(buffer);
}

static vulkan_vertex_info_t* create_vertex_info(shader_library_shader_preset_t preset, u32 OUT vertex_info_count)
{
	BUFFER attributes = buf_new(vulkan_vertex_info_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	}
	OUT vertex_info_count = buf_get_element_count(&attributes);
	buf_fit(&attributes);
	return buf_get_ptr(&attributes);
}

static void begin_pass(BUFFER* list, vulkan_render_pass_type_t type)
{
	vulkan_render_pass_description_t* pass = create_element(list);
	pass->type = type;
	pass->attachments = CAST_TO(vulkan_attachment_type_t*, create_buffer(vulkan_attachment_type_t));
	pass->input_attachments = CAST_TO(u32*, create_buffer(u32));
	pass->subpass_descriptions = CAST_TO(vulkan_subpass_description_t*, create_buffer(vulkan_subpass_description_t));
	pass->render_set_bindings = CAST_TO(vulkan_shader_resource_descriptor_t*, create_buffer(vulkan_shader_resource_descriptor_t));
}

static void add_input_from_previous(BUFFER* list, u32 index, u32 binding)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	buf_push_auto(CAST_TO(BUFFER*, pass->input_attachments), index);
	add_sampler2D(CAST_TO(BUFFER*, pass->render_set_bindings), "internal", VULKAN_DESCRIPTOR_SET_RENDER, binding);
}

static void add_attachment(BUFFER* list, vulkan_attachment_type_t type)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	buf_push_auto(CAST_TO(BUFFER*, pass->attachments), type);
}

static void begin_subpass(BUFFER* list, u32 pipeline_index)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	vulkan_subpass_description_t* subpass = create_element(CAST_TO(BUFFER*, pass->subpass_descriptions));
	subpass->pipeline_description_index = pipeline_index;
	subpass->depth_stencil_attachment = U32_MAX;
	subpass->color_attachments = CAST_TO(u32*, create_buffer(u32));
	subpass->input_attachments = CAST_TO(u32*, create_buffer(u32));
	subpass->preserve_attachments = CAST_TO(u32*, create_buffer(u32));
}

typedef enum vulkan_attachment_reference_type_t
{
	VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR = 1,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT,
	VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL,
	VUKLAN_ATTACHMENT_REFERENCE_TYPE_PRESERVE
} vulkan_attachment_reference_type_t;

static void add_attachment_reference(BUFFER* list, vulkan_attachment_reference_type_t type, u32 reference)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, pass->subpass_descriptions));
	switch(type)
	{
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR:
			buf_push_auto(CAST_TO(BUFFER*, subpass->color_attachments), reference);
			break;
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT:
			buf_push_auto(CAST_TO(BUFFER*, subpass->input_attachments), reference);
			break;
		case VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL:
			subpass->depth_stencil_attachment = reference;
			break;
		case VUKLAN_ATTACHMENT_REFERENCE_TYPE_PRESERVE:
			buf_push_auto(CAST_TO(BUFFER*, subpass->preserve_attachments), reference);		
			break;
		default:
			LOG_FETAL_ERR("Unsupported vulkan attachment reference type: %u\n", type);
	}
}

static void end_subpass(BUFFER* list)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	vulkan_subpass_description_t* subpass = buf_peek_ptr(CAST_TO(BUFFER*, pass->subpass_descriptions));
	BUFFER* buffer;
	if(subpass->color_attachments != NULL)
	{
		buffer = CAST_TO(BUFFER*, subpass->color_attachments);
		subpass->color_attachment_count = buf_get_element_count(buffer);
		subpass->color_attachments = buf_get_ptr(buffer);
		heap_free(buffer);
	}
	if(subpass->input_attachments != NULL)
	{
		buffer = CAST_TO(BUFFER*, subpass->input_attachments);
		subpass->input_attachment_count = buf_get_element_count(buffer);
		subpass->input_attachments = buf_get_ptr(buffer);
		heap_free(buffer);
	}
	if(subpass->preserve_attachments != NULL)
	{
		buffer = CAST_TO(BUFFER*, subpass->preserve_attachments);
		subpass->preserve_attachment_count = buf_get_element_count(buffer);
		subpass->preserve_attachments = buf_get_ptr(buffer);
		heap_free(buffer);
	}
}

static void end_pass(BUFFER* list)
{
	vulkan_render_pass_description_t* pass = buf_peek_ptr(list);
	BUFFER* buffer = CAST_TO(BUFFER*, pass->attachments);
	pass->attachment_count = buf_get_element_count(buffer);
	pass->attachments = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, pass->input_attachments);
	pass->input_attachment_count = buf_get_element_count(buffer);
	pass->input_attachments = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, pass->subpass_descriptions);
	pass->subpass_count = buf_get_element_count(buffer);
	pass->subpass_descriptions = buf_get_ptr(buffer);
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, pass->render_set_bindings);
	pass->render_set_bindings = buf_get_ptr(buffer);
	pass->render_set_binding_count = buf_get_element_count(buffer);
	heap_free(buffer);
}

static vulkan_render_pass_description_t* create_render_pass_description(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset, u32 OUT pass_count)
{
	BUFFER passes = buf_create(sizeof(vulkan_render_pass_description_t), 1, 0);

	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1);
				end_subpass(&passes);
			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1);
				end_subpass(&passes);
			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0);
				end_subpass(&passes);
			end_pass(&passes);
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_input_from_previous(&passes, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 1);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1);
				end_subpass(&passes);
			end_pass(&passes);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	};

	OUT pass_count = buf_get_element_count(&passes);
	buf_fit(&passes);
	return buf_get_ptr(&passes);
}

static void begin_pipeline(vulkan_renderer_t* renderer, BUFFER* list)
{
	vulkan_graphics_pipeline_description_t* pipeline = create_element(list);
	GraphicsPipelineSettings* settings = heap_new(GraphicsPipelineSettings);
	memzero(settings, GraphicsPipelineSettings);
	pipeline->settings = settings;

	pipeline->spirv_codes = CAST_TO(vulkan_spirv_code_t*, create_buffer(vulkan_spirv_code_t));

	settings->colorblend = (VkPipelineColorBlendStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.pAttachments = CAST_TO(VkPipelineColorBlendAttachmentState*, create_buffer(VkPipelineColorBlendAttachmentState))
	};

	settings->inputassembly = (VkPipelineInputAssemblyStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
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
	settings->rasterization = (VkPipelineRasterizationStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth = 1.0f
	};
	settings->multisample = (VkPipelineMultisampleStateCreateInfo)
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};
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
}

static void add_color_blend_state(BUFFER* list, VkBool32 blendEnable)
{
	VkPipelineColorBlendAttachmentState state =
	{
		.blendEnable = blendEnable,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT	
	};
	VkPipelineColorBlendStateCreateInfo* info = &CAST_TO(vulkan_graphics_pipeline_description_t*, buf_peek_ptr(list))->settings->colorblend;
	buf_push(CAST_TO(BUFFER*, info->pAttachments), &state);
}

static void set_depth_stencil(BUFFER* list, VkBool32 depthWrite, VkBool32 depthTest)
{
	VkPipelineDepthStencilStateCreateInfo* info = &CAST_TO(vulkan_graphics_pipeline_description_t*, buf_peek_ptr(list))->settings->depthstencil;
	info->depthWriteEnable = depthWrite;
	info->depthTestEnable = depthTest;
}

static void add_shader(BUFFER* list, const char* file_path, vulkan_shader_type_t type)
{
	vulkan_graphics_pipeline_description_t* pipeline = CAST_TO(vulkan_graphics_pipeline_description_t*, buf_peek_ptr(list));
	BUFFER* data = load_binary_from_file(file_path);
	vulkan_spirv_code_t code = 
	{
		.type = type,
		.spirv = data->bytes,
		.length = data->element_count
	};
	buf_push(CAST_TO(BUFFER*, pipeline->spirv_codes), &code);
}

static void end_pipeline(BUFFER* list)
{
	vulkan_graphics_pipeline_description_t* pipeline = CAST_TO(vulkan_graphics_pipeline_description_t*, buf_peek_ptr(list));
	VkPipelineColorBlendStateCreateInfo* info = &pipeline->settings->colorblend;
	BUFFER* buffer = CAST_TO(BUFFER*, info->pAttachments);
	info->attachmentCount = buf_get_element_count(buffer);
	info->pAttachments = CAST_TO(VkPipelineColorBlendAttachmentState*, buf_get_ptr(buffer));
	heap_free(buffer);
	buffer = CAST_TO(BUFFER*, pipeline->spirv_codes);
	pipeline->spirv_code_count = buf_get_element_count(buffer);
	pipeline->spirv_codes = buf_get_ptr(buffer);
	heap_free(buffer);
}

static vulkan_graphics_pipeline_description_t* create_pipeline_descriptions(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	BUFFER pipelines = buf_new(vulkan_graphics_pipeline_description_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/color.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/color.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	}
	return buf_get_ptr(&pipelines);
}

static vulkan_shader_create_info_t* get_create_info_from_preset(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_t* create_info = heap_new(vulkan_shader_create_info_t);
	create_info->material_set_bindings = create_material_set_binding(preset, &create_info->material_set_binding_count);
	create_info->vertex_infos = create_vertex_info(preset, &create_info->vertex_info_count);
	create_info->render_pass_descriptions = create_render_pass_description(renderer, preset, &create_info->render_pass_description_count);
	create_info->pipeline_descriptions = create_pipeline_descriptions(renderer, preset);
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
