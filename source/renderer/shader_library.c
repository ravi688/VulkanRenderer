#include <renderer/shader_library.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_shader.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/memory_allocator.h>

#include <disk_manager/file_reader.h>
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

static void add_opaque(BUFFER* list, const char* name, u32 type, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_description_add_opaque(create_element(list), name, type, set_number, binding_number);
}

static struct_descriptor_t* begin_uniform(BUFFER* list, const char* name, u32 set_number, u32 binding_number)
{
	return vulkan_shader_resource_description_begin_uniform(create_element(list), name, set_number, binding_number);
}

static void end_uniform(BUFFER* list)
{
	vulkan_shader_resource_description_end_uniform(buf_peek_ptr(list));
}

static vulkan_shader_resource_description_t* create_material_set_binding(shader_library_shader_preset_t preset, u32 OUT binding_count)
{
	BUFFER bindings = buf_new(vulkan_shader_resource_description_t);
	struct_descriptor_t* parameters;
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(&bindings);	
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(&bindings);	
			add_opaque(&bindings, "albedo", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(&bindings);	
			add_opaque(&bindings, "shadowMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(&bindings);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(&bindings);
			add_opaque(&bindings, "albedo", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			add_opaque(&bindings, "albedo", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
			parameters = begin_uniform(&bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
				struct_descriptor_add_field(parameters, "reflectance", GLSL_TYPE_FLOAT);
			end_uniform(&bindings);	
			add_opaque(&bindings, "reflectionMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
		break;
		default:
			UNSUPPORTED_PRESET(preset);
	};

	OUT binding_count = buf_get_element_count(&bindings);
	if((OUT binding_count) != 0)
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
	vulkan_vertex_buffer_layout_description_begin(create_element(list), stride, input_rate, binding_number);
}

static void add_vertex_attribute(BUFFER* list, u16 location, VkFormat format, u32 offset)
{
	vulkan_vertex_buffer_layout_description_add_attribute(buf_peek_ptr(list), location, format, offset);
}

static void end_vertex_binding(BUFFER* list)
{
	vulkan_vertex_buffer_layout_description_end(buf_peek_ptr(list));
}

static vulkan_vertex_buffer_layout_description_t* create_vertex_info(shader_library_shader_preset_t preset, u32 OUT vertex_info_count)
{
	BUFFER attributes = buf_new(vulkan_vertex_buffer_layout_description_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			begin_vertex_binding(&attributes, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
			end_vertex_binding(&attributes);
		break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
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
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			begin_vertex_binding(&attributes, 12, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(&attributes);
			begin_vertex_binding(&attributes, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
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
	vulkan_render_pass_description_begin(create_element(list), type);
}

static void add_input(BUFFER* list, u32 index, u32 binding)
{
	vulkan_render_pass_description_add_input(buf_peek_ptr(list), GLSL_TYPE_SAMPLER_2D, index, binding);
}

static void add_attachment(BUFFER* list, vulkan_attachment_type_t type)
{
	vulkan_render_pass_description_add_attachment(buf_peek_ptr(list), type);
}

static void add_dependency(BUFFER* list, VkSubpassDependency* dependency)
{
	vulkan_render_pass_description_add_subpass_dependency(buf_peek_ptr(list), dependency);
}

static void begin_subpass(BUFFER* list, u32 pipeline_index)
{
	vulkan_render_pass_description_begin_subpass(buf_peek_ptr(list), pipeline_index);
}
static void add_attachment_reference(BUFFER* list, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	vulkan_render_pass_description_add_attachment_reference(buf_peek_ptr(list), type, reference, binding);
}

static void end_subpass(BUFFER* list)
{
	vulkan_render_pass_description_end_subpass(buf_peek_ptr(list));
}

static void end_pass(BUFFER* list)
{
	vulkan_render_pass_description_end(buf_peek_ptr(list));
}

static vulkan_render_pass_description_t* create_render_pass_description(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset, u32 OUT pass_count)
{
	BUFFER passes = buf_create(sizeof(vulkan_render_pass_description_t), 1, 0);

	VkSubpassDependency dependency = { .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT };
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
				end_subpass(&passes);
			end_pass(&passes);
		break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(&passes);

				// let the clear happen first
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				add_dependency(&passes, &dependency);

				// let the layout transition happen
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = 0;
				add_dependency(&passes, &dependency);

			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(&passes);
			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(&passes);
			end_pass(&passes);
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_input(&passes, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 1);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(&passes);
			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(&passes);
				
				// let the clear happen first
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				add_dependency(&passes, &dependency);

				// let the layout transition happen
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = 0;
				add_dependency(&passes, &dependency);

				// let the write happen first and then allow read
				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				add_dependency(&passes, &dependency);

			end_pass(&passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(&passes);
				
				// let the clear happen first
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				add_dependency(&passes, &dependency);

				// let the layout transition happen
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = 0;
				add_dependency(&passes, &dependency);

				// let the write happen first and then allow read
				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				add_dependency(&passes, &dependency);

			end_pass(&passes);
			begin_pass(&passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_input(&passes, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 1 target [swapchain image]
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 0 target
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(&passes, 1);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 1, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				end_subpass(&passes);

				// let the clear happen first for the depth stencil attachment
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				add_dependency(&passes, &dependency);

				// let the clear happen first for the color attachment
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				add_dependency(&passes, &dependency);


				// let the layout transition happen
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = 0;
				add_dependency(&passes, &dependency);

				begin_subpass(&passes, 2);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT, 1, VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(&passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				end_subpass(&passes);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = 1;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				add_dependency(&passes, &dependency);

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
	vulkan_graphics_pipeline_description_begin(renderer, create_element(list));
}

static void add_color_blend_state(BUFFER* list, VkBool32 blendEnable)
{
	vulkan_graphics_pipeline_description_add_color_blend_state(buf_peek_ptr(list), blendEnable);
}

static void set_depth_stencil(BUFFER* list, VkBool32 depthWrite, VkBool32 depthTest)
{
	vulkan_graphics_pipeline_description_set_depth_stencil(buf_peek_ptr(list), depthWrite, depthTest);
}

static void set_depth_bias(BUFFER* list, float factor, float clamp, float slope_factor)
{
	vulkan_graphics_pipeline_description_set_depth_bias(buf_peek_ptr(list), factor, clamp, slope_factor);
}

static void add_shader(BUFFER* list, const char* file_path, vulkan_shader_type_t type)
{
	vulkan_graphics_pipeline_description_add_shader(buf_peek_ptr(list), file_path, type);
}

static void end_pipeline(BUFFER* list)
{
	vulkan_graphics_pipeline_description_end(buf_peek_ptr(list));
}

static vulkan_graphics_pipeline_description_t* create_pipeline_descriptions(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	BUFFER pipelines = buf_new(vulkan_graphics_pipeline_description_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui2.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui2.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/skybox/skybox.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/skybox/skybox.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
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
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.depth.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.depth.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
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
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(&pipelines);
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
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
