/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: shader_library.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

#include <renderer/shader_library.h>
#include <renderer/renderer.h>
#include <renderer/internal/vulkan/vulkan_shader_library.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set.h>
#include <renderer/internal/vulkan/vulkan_descriptor_set_layout.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description_builder.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description_builder.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description_builder.h>
#include <renderer/internal/vulkan/vulkan_shader_create_info_builder.h>
#include <renderer/internal/vulkan/vulkan_renderer.h>
#include <renderer/internal/vulkan/vulkan_mesh.h>
#include <renderer/alloc.h>

#include <disk_manager/file_reader.h>
#include <string.h> 		// strcpy

/* constructors & destructors */
RENDERER_API shader_library_t* shader_library_new(memory_allocator_t* allocator)
{
	return vulkan_shader_library_new(allocator);
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

static void shr_res_build_create(vulkan_shader_resource_description_builder_t* builder, u32* bind_counter)
{
	vulkan_shader_resource_description_builder_add(builder, 1);
	vulkan_shader_resource_description_builder_bind(builder, *bind_counter);
	(*bind_counter)++;
}

static CAN_BE_UNUSED_FUNCTION void shr_res_build_create_vertex_attribute(vulkan_shader_resource_description_builder_t* builder, u32* bind_counter, const char* name, glsl_type_t type, u32 location_number, u32 binding_number)
{
	shr_res_build_create(builder, bind_counter);
	vulkan_shader_resource_description_builder_create_vertex_attribute(builder, name, type, location_number, binding_number);
}
static void shr_res_build_create_opaque(vulkan_shader_resource_description_builder_t* builder, u32* bind_counter, const char* name, glsl_type_t type, u32 set_number, u32 binding_number)
{
	shr_res_build_create(builder, bind_counter);
	vulkan_shader_resource_description_builder_create_opaque(builder, name, type, set_number, binding_number);
}
static struct_descriptor_t* shr_res_build_create_uniform(vulkan_shader_resource_description_builder_t* builder, u32* bind_counter, const char* name, u32 set_number, u32 binding_number)
{
	shr_res_build_create(builder, bind_counter);
	return vulkan_shader_resource_description_builder_create_uniform(builder, name, set_number, binding_number);
}
static INLINE_IF_RELEASE_MODE void shr_res_build_end_uniform(vulkan_shader_resource_description_builder_t* builder)
{
	vulkan_shader_resource_description_builder_end_uniform(builder);
}


#define UNSUPPORTED_PRESET(preset) LOG_FETAL_ERR("Unsupported shader preset type %u\n", preset)

static vulkan_shader_resource_description_builder_t* create_material_set_binding(memory_allocator_t* allocator, shader_library_shader_preset_t preset)
{
	vulkan_shader_resource_description_builder_t* builder = vulkan_shader_resource_description_builder_create(allocator);
	struct_descriptor_t* parameters;
	u32 bind_counter = 0;
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
		{
			{
				struct_descriptor_t* Color = struct_descriptor_create(allocator);
					struct_descriptor_begin(allocator, Color, "Color", GLSL_TYPE_MAX_NON_OPAQUE);
						struct_descriptor_add_field(Color, "r", GLSL_TYPE_FLOAT);
						struct_descriptor_add_field(Color, "g", GLSL_TYPE_FLOAT);
						struct_descriptor_add_field(Color, "b", GLSL_TYPE_FLOAT);
					struct_descriptor_end(allocator, Color);
				parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
					if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
						struct_descriptor_add_field(parameters, "tex_size", GLSL_TYPE_UVEC2);
					struct_descriptor_add_field2(parameters, "color", Color);
					struct_descriptor_add_field(parameters, "space_type", GLSL_TYPE_INT);
					struct_descriptor_add_field(parameters, "surface_type", GLSL_TYPE_INT);
				shr_res_build_end_uniform(builder);
			}
			if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
			{
				shr_res_build_create_opaque(builder, &bind_counter, "bga", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				{
					struct_descriptor_t* GTC = struct_descriptor_create(allocator);
						struct_descriptor_begin(allocator, GTC, "GTC", GLSL_TYPE_MAX_NON_OPAQUE);
							struct_descriptor_add_field(GTC, "tltc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "trtc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "brtc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "bltc", GLSL_TYPE_VEC2);
						struct_descriptor_end(allocator, GTC);

						AUTO gtc_buffer = shr_res_build_create_uniform(builder, &bind_counter, "GTCBuffer", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE1);
							struct_descriptor_add_field_array2(gtc_buffer, "texcoords", GTC, U32_MAX);
						shr_res_build_end_uniform(builder);
				}
			}
			{
				AUTO tst_buffer = shr_res_build_create_uniform(builder, &bind_counter, "TSTBuffer", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE2);
					struct_descriptor_add_field_array(tst_buffer, "transforms", GLSL_TYPE_MAT4, U32_MAX);
				shr_res_build_end_uniform(builder);
			}
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		{
			parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			shr_res_build_end_uniform(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
		{
			parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			shr_res_build_end_uniform(builder);
			shr_res_build_create_opaque(builder, &bind_counter, "albedo", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
		{
			parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			shr_res_build_end_uniform(builder);
			shr_res_build_create_opaque(builder, &bind_counter, "shadowMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
		{
			parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			shr_res_build_end_uniform(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		{
			shr_res_build_create_opaque(builder, &bind_counter, "albedo", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		{
			parameters = shr_res_build_create_uniform(builder, &bind_counter, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
				struct_descriptor_add_field(parameters, "reflectance", GLSL_TYPE_FLOAT);
			shr_res_build_end_uniform(builder);
			shr_res_build_create_opaque(builder, &bind_counter, "reflectionMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
		{
			break;
		}
		default:
		{
			UNSUPPORTED_PRESET(preset);
			break;
		}
	};

	return builder;
}

static INLINE_IF_RELEASE_MODE void vbl_build_begin(vulkan_vertex_buffer_layout_description_builder_t* builder, u32* bind_counter, u32 stride, VkVertexInputRate input_rate, u32 binding_number)
{
	vulkan_vertex_buffer_layout_description_builder_add(builder, 1);
	vulkan_vertex_buffer_layout_description_builder_bind(builder, *bind_counter);
	vulkan_vertex_buffer_layout_description_builder_begin(builder, stride, input_rate, binding_number);
	(*bind_counter)++;
}
static INLINE_IF_RELEASE_MODE void vbl_build_add_attribute(vulkan_vertex_buffer_layout_description_builder_t* builder, u32 location, VkFormat format, u32 offset)
{
	vulkan_vertex_buffer_layout_description_builder_add_attribute(builder, location, format, offset);
}
static INLINE_IF_RELEASE_MODE void vbl_build_end(vulkan_vertex_buffer_layout_description_builder_t* builder)
{
	vulkan_vertex_buffer_layout_description_builder_end(builder);
}

static vulkan_vertex_buffer_layout_description_builder_t* create_vertex_info(memory_allocator_t* allocator, shader_library_shader_preset_t preset)
{
	vulkan_vertex_buffer_layout_description_builder_t* builder = vulkan_vertex_buffer_layout_description_builder_create(allocator);
	u32 bind_counter = 0;
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
		{
			vbl_build_begin(builder, &bind_counter, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
			{
				/* bindings 0, 1, 2, 3, and 4 are reserved for basic vertex data buffers (they are not interleaved), hence we must start from 5 */
				vbl_build_begin(builder, &bind_counter, 48, VK_VERTEX_INPUT_RATE_INSTANCE, 5);
					/* locations 0, 1, 2, 3, and 4 are reserved for basic vertex data, hence we must start from 5 */
					vbl_build_add_attribute(builder, 5, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
					vbl_build_add_attribute(builder, 6, VK_FORMAT_R32G32B32A32_SFLOAT, 16);
					vbl_build_add_attribute(builder, 7, VK_FORMAT_R32G32B32_SFLOAT, 32);
				vbl_build_end(builder);
			}
			else /* if preset == SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH */
			{
				/* bindings 0, 1, 2, 3, and 4 are reserved for basic vertex data buffers (they are not interleaved), hence we must start from 5 */
				vbl_build_begin(builder, &bind_counter, 16, VK_VERTEX_INPUT_RATE_INSTANCE, 5);
					/* locations 0, 1, 2, 3, and 4 are reserved for basic vertex data, hence we must start from 5 */
					vbl_build_add_attribute(builder, 5, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
				vbl_build_end(builder);
			}
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
		{
			vbl_build_begin(builder, &bind_counter, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
		{
			vbl_build_begin(builder, &bind_counter, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			vbl_build_begin(builder, &bind_counter, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
			vbl_build_end(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
		{
			vbl_build_begin(builder, &bind_counter, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			vbl_build_begin(builder, &bind_counter, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
		{
			vbl_build_begin(builder, &bind_counter, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			vbl_build_begin(builder, &bind_counter, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			vbl_build_end(builder);
			vbl_build_begin(builder, &bind_counter, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				vbl_build_add_attribute(builder, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
			vbl_build_end(builder);
			break;
		}
		default:
		{
			UNSUPPORTED_PRESET(preset);
			break;
		}
	}
	return builder;
}

static INLINE_IF_RELEASE_MODE void rpd_build_begin_pass(vulkan_render_pass_description_builder_t* builder, u32 bind_index, vulkan_render_pass_type_t type)
{
	vulkan_render_pass_description_builder_add(builder, 1);
	vulkan_render_pass_description_builder_bind(builder, bind_index);
	vulkan_render_pass_description_builder_begin_pass(builder, type);
}
static INLINE_IF_RELEASE_MODE void rpd_build_add_input_sampler2d(vulkan_render_pass_description_builder_t* builder, u32 index, u32 binding)
{
	vulkan_render_pass_description_builder_add_input(builder, GLSL_TYPE_SAMPLER_2D, index, binding);
}
static INLINE_IF_RELEASE_MODE void rpd_build_add_attachment(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_type_t type)
{
	vulkan_render_pass_description_builder_add_attachment(builder, type);
}
static INLINE_IF_RELEASE_MODE void rpd_build_begin_subpass(vulkan_render_pass_description_builder_t* builder, u32 pipeline_index)
{
	vulkan_render_pass_description_builder_begin_subpass(builder, pipeline_index);
}
static INLINE_IF_RELEASE_MODE void rpd_build_add_attachment_reference(vulkan_render_pass_description_builder_t* builder, vulkan_attachment_reference_type_t type, u32 reference, u32 binding)
{
	vulkan_render_pass_description_builder_add_attachment_reference(builder, type, reference, binding);
}
static INLINE_IF_RELEASE_MODE void rpd_build_end_subpass(vulkan_render_pass_description_builder_t* builder)
{
	vulkan_render_pass_description_builder_end_subpass(builder);
}
static INLINE_IF_RELEASE_MODE void rpd_build_add_subpass_dependency(vulkan_render_pass_description_builder_t* builder, VkSubpassDependency* dependency)
{
	vulkan_render_pass_description_builder_add_subpass_dependency(builder, dependency);
}
static INLINE_IF_RELEASE_MODE void rpd_build_end_pass(vulkan_render_pass_description_builder_t* builder)
{
	vulkan_render_pass_description_builder_end_pass(builder);
}


static vulkan_render_pass_description_builder_t* create_render_pass_description(memory_allocator_t* allocator, shader_library_shader_preset_t preset)
{
	vulkan_render_pass_description_builder_t* builder = vulkan_render_pass_description_builder_create(allocator);

	VkSubpassDependency dependency = { .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT };
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR);
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
				rpd_build_end_subpass(builder);
			rpd_build_end_pass(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR); // swapchain image
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH); // depth buffer of the camera
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR); // swapchain image
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH); // depth buffer of the camera
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);
				
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);
				
				dependency.srcSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH); // local depth buffer
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);

			rpd_build_begin_pass(builder, 1, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				rpd_build_add_input_sampler2d(builder, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR); // swapchain image
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH); // depth buffer of the camera
				rpd_build_begin_subpass(builder, 1);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH); // local depth buffer 
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dependency.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
		{
			rpd_build_begin_pass(builder, 0, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH);
				rpd_build_begin_subpass(builder, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				// let the write happen first and then allow read
				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			rpd_build_begin_pass(builder, 1, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				rpd_build_add_input_sampler2d(builder, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 1 target [swapchain image]
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 0 target
				rpd_build_add_attachment(builder, VULKAN_ATTACHMENT_TYPE_DEPTH);
				rpd_build_begin_subpass(builder, 1);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 1, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);


				rpd_build_begin_subpass(builder, 2);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT, 1, VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					rpd_build_add_attachment_reference(builder, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				rpd_build_end_subpass(builder);

				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 1;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = 1;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = 1;
				dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

				dependency.srcSubpass = 1;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				rpd_build_add_subpass_dependency(builder, &dependency);

			rpd_build_end_pass(builder);
			break;
		}
		default:
		{
			UNSUPPORTED_PRESET(preset);
			break;
		}
	};
	return builder;
}

static void gfx_pipe_build_begin_pipeline(vulkan_graphics_pipeline_description_builder_t* builder, u32 bind_index)
{
	vulkan_graphics_pipeline_description_builder_add(builder, 1);
	vulkan_graphics_pipeline_description_builder_bind(builder, bind_index);
	vulkan_graphics_pipeline_description_builder_begin_pipeline(builder);
}

static INLINE_IF_RELEASE_MODE void gfx_pipe_build_add_color_blend_state(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_blend_enable)
{
	vulkan_graphics_pipeline_description_builder_add_color_blend_state(builder, is_blend_enable);
}

static INLINE_IF_RELEASE_MODE void gfx_pipe_build_add_shader(vulkan_graphics_pipeline_description_builder_t* builder, const char* file_path, vulkan_shader_type_t shader_type)
{
	vulkan_graphics_pipeline_description_builder_add_shader(builder, file_path, shader_type);
}

static INLINE_IF_RELEASE_MODE void gfx_pipe_build_set_depth_stencil(vulkan_graphics_pipeline_description_builder_t* builder, VkBool32 is_depth_write, VkBool32 is_depth_test)
{
	vulkan_graphics_pipeline_description_builder_set_depth_stencil(builder, is_depth_write, is_depth_test);
}

static INLINE_IF_RELEASE_MODE void gfx_pipe_build_end_pipeline(vulkan_graphics_pipeline_description_builder_t* builder)
{
	vulkan_graphics_pipeline_description_builder_end_pipeline(builder);
}

static vulkan_graphics_pipeline_description_builder_t* create_pipeline_descriptions(memory_allocator_t* allocator, shader_library_shader_preset_t preset)
{
	vulkan_graphics_pipeline_description_builder_t* builder = vulkan_graphics_pipeline_description_builder_create(allocator);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				//get_rasterization(&pipelines)->polygonMode = VK_POLYGON_MODE_LINE;
				if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
				{
					gfx_pipe_build_add_shader(builder, "shaders/presets/bitmap_text/bitmap_text.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
					gfx_pipe_build_add_shader(builder, "shaders/presets/bitmap_text/bitmap_text.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
				}
				else
				{
					gfx_pipe_build_add_shader(builder, "shaders/presets/text_mesh/text_mesh.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
					gfx_pipe_build_add_shader(builder, "shaders/presets/text_mesh/text_mesh.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
				}
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/ui/ui.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/ui/ui.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/ui/ui2.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/ui/ui2.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/unlit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/skybox/skybox.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/skybox/skybox.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.depth.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/reflection.depth.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 1);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/color.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/color.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 1);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/diffuse.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/diffuse.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 2);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 1);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/diffuse.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/diffuse.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 2);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.spot.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.spot.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			gfx_pipe_build_begin_pipeline(builder, 1);
				gfx_pipe_build_add_color_blend_state(builder, VK_FALSE);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/spot.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/spot.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
		{
			gfx_pipe_build_begin_pipeline(builder, 0);
				gfx_pipe_build_set_depth_stencil(builder, VK_TRUE, VK_TRUE);
				gfx_pipe_build_add_shader(builder, "shaders/presets/lit/color/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				gfx_pipe_build_add_shader(builder, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			gfx_pipe_build_end_pipeline(builder);
			break;
		}
		default:
		{
			UNSUPPORTED_PRESET(preset);
			break;
		}
	}
	return builder;
}

static vulkan_shader_create_info_builder_t* get_create_info_from_preset(memory_allocator_t* allocator, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_builder_t* builder = vulkan_shader_create_info_builder_create(allocator);
	vulkan_shader_create_info_builder_add(builder, 1);
	vulkan_shader_create_info_builder_bind(builder, 0);
	
	/* create material set bindings */
	AUTO msb_builder = create_material_set_binding(allocator, preset);
	vulkan_shader_create_info_builder_set_material_set_bindings_builder(builder, msb_builder, true);

	/* create vertex buffer layout descriptions */
	AUTO vbld_builder = create_vertex_info(allocator, preset);
	vulkan_shader_create_info_builder_set_vertex_buffer_layout_descriptions_builder(builder, vbld_builder, true);

	/* create render pass descriptions */
	AUTO rpds_builder = create_render_pass_description(allocator, preset);
	vulkan_shader_create_info_builder_set_render_pass_descriptions_builder(builder, rpds_builder, true);
	
	/* create graphics pipeline descriptions */
	AUTO gfx_pipes_builder = create_pipeline_descriptions(allocator, preset);
	vulkan_shader_create_info_builder_set_graphics_pipeline_descriptions_builder(builder, gfx_pipes_builder, true);
	return builder;
}

RENDERER_API shader_handle_t shader_library_create_shader_from_preset(shader_library_t* library, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_builder_t* builder = get_create_info_from_preset(library->renderer->allocator, preset);
	AUTO handle = vulkan_shader_library_create_shader(library, vulkan_shader_create_info_builder_get(builder), "Built-In");
	vulkan_shader_create_info_builder_destroy(builder);
	return handle;
}

RENDERER_API shader_handle_t shader_library_create_shader(shader_library_t* library, shader_create_info_t* create_info, const char* shader_name)
{
	return vulkan_shader_library_create_shader(library, CAST_TO(vulkan_shader_create_info_t*, create_info), shader_name);
}

RENDERER_API shader_handle_t shader_library_load_shader(shader_library_t* library, const char* file_path)
{
	vulkan_shader_load_info_t load_info =
	{
		.path = file_path,
		.is_vertex_attrib_from_file = true
	};
	return vulkan_shader_library_load_shader(library, &load_info, "Untitled");
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
	return SHADER(vulkan_shader_library_getH(library, handle));
}

RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name)
{
	return SHADER(vulkan_shader_library_get(library, shader_name));
}
