/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: shader_library.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
#include <renderer/internal/vulkan/vulkan_vertex_buffer_layout_description.h>
#include <renderer/internal/vulkan/vulkan_render_pass_description.h>
#include <renderer/internal/vulkan/vulkan_graphics_pipeline_description.h>
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

#define UNSUPPORTED_PRESET(preset) LOG_FETAL_ERR("Unsupported shader preset type %u\n", preset)

static vulkan_shader_resource_description_t* create_material_set_binding(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset, u32 OUT binding_count)
{
	BUFFER bindings = buf_new(vulkan_shader_resource_description_t);
	struct_descriptor_t* parameters;
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
			{
				struct_descriptor_t* Color = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_STRUCT_DESCRIPTOR, struct_descriptor_t);
					struct_descriptor_begin(renderer->allocator, Color, "Color", GLSL_TYPE_MAX_NON_OPAQUE);
						struct_descriptor_add_field(Color, "r", GLSL_TYPE_FLOAT);
						struct_descriptor_add_field(Color, "g", GLSL_TYPE_FLOAT);
						struct_descriptor_add_field(Color, "b", GLSL_TYPE_FLOAT);
					struct_descriptor_end(renderer->allocator, Color);
				parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
					if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
						struct_descriptor_add_field(parameters, "tex_size", GLSL_TYPE_UVEC2);
					struct_descriptor_add_field2(parameters, "color", Color);
					struct_descriptor_add_field(parameters, "space_type", GLSL_TYPE_INT);
					struct_descriptor_add_field(parameters, "surface_type", GLSL_TYPE_INT);
				end_uniform(renderer, &bindings);
			}
			if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
			{
				add_opaque(renderer, &bindings, "bga", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				{
					struct_descriptor_t* GTC = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_STRUCT_DESCRIPTOR, struct_descriptor_t);
						struct_descriptor_begin(renderer->allocator, GTC, "GTC", GLSL_TYPE_MAX_NON_OPAQUE);
							struct_descriptor_add_field(GTC, "tltc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "trtc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "brtc", GLSL_TYPE_VEC2);
							struct_descriptor_add_field(GTC, "bltc", GLSL_TYPE_VEC2);
						struct_descriptor_end(renderer->allocator, GTC);

						AUTO gtc_buffer = begin_uniform(renderer, &bindings, "GTCBuffer", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE1);
							struct_descriptor_add_field_array2(gtc_buffer, "texcoords", GTC, U32_MAX);
						end_uniform(renderer, &bindings);
				}
			}
			{
				AUTO tst_buffer = begin_uniform(renderer, &bindings, "TSTBuffer", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE2);
					struct_descriptor_add_field_array(tst_buffer, "transforms", GLSL_TYPE_MAT4, U32_MAX);
				end_uniform(renderer, &bindings);
			}
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
			parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(renderer, &bindings);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(renderer, &bindings);
			add_opaque(renderer, &bindings, "albedo", GLSL_TYPE_SAMPLER_2D, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(renderer, &bindings);
			add_opaque(renderer, &bindings, "shadowMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
			end_uniform(renderer, &bindings);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			add_opaque(renderer, &bindings, "albedo", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
			parameters = begin_uniform(renderer, &bindings, "parameters", VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_MATERIAL_PROPERTIES);
				struct_descriptor_add_field(parameters, "color", GLSL_TYPE_VEC4);
				struct_descriptor_add_field(parameters, "reflectance", GLSL_TYPE_FLOAT);
			end_uniform(renderer, &bindings);
			add_opaque(renderer, &bindings, "reflectionMap", GLSL_TYPE_SAMPLER_CUBE, VULKAN_DESCRIPTOR_SET_MATERIAL, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
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

static vulkan_vertex_buffer_layout_description_t* create_vertex_info(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset, u32 OUT vertex_info_count)
{
	BUFFER attributes = buf_new(vulkan_vertex_buffer_layout_description_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
			begin_vertex_binding(renderer, &attributes, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
			{
				/* bindings 0, 1, 2, 3, and 4 are reserved for basic vertex data buffers (they are not interleaved), hence we must start from 5 */
				begin_vertex_binding(renderer, &attributes, 48, VK_VERTEX_INPUT_RATE_INSTANCE, 5);
					/* locations 0, 1, 2, 3, and 4 are reserved for basic vertex data, hence we must start from 5 */
					add_vertex_attribute(&attributes, 5, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
					add_vertex_attribute(&attributes, 6, VK_FORMAT_R32G32B32A32_SFLOAT, 16);
					add_vertex_attribute(&attributes, 7, VK_FORMAT_R32G32B32_SFLOAT, 32);
				end_vertex_binding(renderer, &attributes);
			}
			else /* if preset == SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH */
			{
				/* bindings 0, 1, 2, 3, and 4 are reserved for basic vertex data buffers (they are not interleaved), hence we must start from 5 */
				begin_vertex_binding(renderer, &attributes, 16, VK_VERTEX_INPUT_RATE_INSTANCE, 5);
					/* locations 0, 1, 2, 3, and 4 are reserved for basic vertex data, hence we must start from 5 */
					add_vertex_attribute(&attributes, 5, VK_FORMAT_R32G32B32A32_SFLOAT, 0);
				end_vertex_binding(renderer, &attributes);
			}
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_vertex_binding(renderer, &attributes, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_vertex_binding(renderer, &attributes, 16, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			begin_vertex_binding(renderer, &attributes, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
		break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_vertex_binding(renderer, &attributes, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			begin_vertex_binding(renderer, &attributes, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_vertex_binding(renderer, &attributes, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_POSITION_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			begin_vertex_binding(renderer, &attributes, 16 /* 12 + 1 padding */, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_NORMAL_LOCATION, VK_FORMAT_R32G32B32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			begin_vertex_binding(renderer, &attributes, 8, VK_VERTEX_INPUT_RATE_VERTEX, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_BINDING);
				add_vertex_attribute(&attributes, VULKAN_MESH_VERTEX_ATTRIBUTE_TEXCOORD_LOCATION, VK_FORMAT_R32G32_SFLOAT, 0);
			end_vertex_binding(renderer, &attributes);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	}
	OUT vertex_info_count = buf_get_element_count(&attributes);
	buf_fit(&attributes);
	return buf_get_ptr(&attributes);
}

static vulkan_render_pass_description_t* create_render_pass_description(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset, u32 OUT pass_count)
{
	BUFFER passes = buf_create(sizeof(vulkan_render_pass_description_t), 1, 0);

	VkSubpassDependency dependency = { .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT };
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
				end_subpass(renderer, &passes);
			end_pass(renderer, &passes);
		break;
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(renderer, &passes);

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

			end_pass(renderer, &passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(renderer, &passes);
			end_pass(renderer, &passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(renderer, &passes);
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
			end_pass(renderer, &passes);
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_input(renderer, &passes, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 1);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 1, 0);
				end_subpass(renderer, &passes);
			end_pass(renderer, &passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(renderer, &passes);

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

			end_pass(renderer, &passes);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SINGLE_FRAMEBUFFER);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 0, 0);
				end_subpass(renderer, &passes);

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

			end_pass(renderer, &passes);
			begin_pass(renderer, &passes, VULKAN_RENDER_PASS_TYPE_SWAPCHAIN_TARGET);
				add_input(renderer, &passes, 0, VULKAN_DESCRIPTOR_BINDING_TEXTURE0);
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 1 target [swapchain image]
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_COLOR); 	// subpass 0 target
				add_attachment(&passes, VULKAN_ATTACHMENT_TYPE_DEPTH);
				begin_subpass(renderer, &passes, 1);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 1, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				end_subpass(renderer, &passes);

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

				begin_subpass(renderer, &passes, 2);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_INPUT, 1, VULKAN_DESCRIPTOR_BINDING_INPUT_ATTACHMENT0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_COLOR, 0, 0);
					add_attachment_reference(renderer, &passes, VULKAN_ATTACHMENT_REFERENCE_TYPE_DEPTH_STENCIL, 2, 0);
				end_subpass(renderer, &passes);

				dependency.srcSubpass = 0;
				dependency.dstSubpass = 1;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				add_dependency(&passes, &dependency);

			end_pass(renderer, &passes);
			break;
		default:
			UNSUPPORTED_PRESET(preset);
	};

	OUT pass_count = buf_get_element_count(&passes);
	buf_fit(&passes);
	return buf_get_ptr(&passes);
}

static vulkan_graphics_pipeline_description_t* create_pipeline_descriptions(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	BUFFER pipelines = buf_new(vulkan_graphics_pipeline_description_t);
	switch(preset)
	{
		case SHADER_LIBRARY_SHADER_PRESET_TEXT_MESH:
		case SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				//get_rasterization(&pipelines)->polygonMode = VK_POLYGON_MODE_LINE;
				if(preset == SHADER_LIBRARY_SHADER_PRESET_BITMAP_TEXT)
				{
					add_shader(&pipelines, "shaders/presets/bitmap_text/bitmap_text.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
					add_shader(&pipelines, "shaders/presets/bitmap_text/bitmap_text.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
				}
				else
				{
					add_shader(&pipelines, "shaders/presets/text_mesh/text_mesh.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
					add_shader(&pipelines, "shaders/presets/text_mesh/text_mesh.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
				}
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_UI2:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui2.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/ui/ui2.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_UNLIT_COLOR:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/unlit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SKYBOX:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/skybox/skybox.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/skybox/skybox.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_COLOR:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/color.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/color.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.depth.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_REFLECTION_DEPTH_POINT:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/reflection.depth.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
		break;
		case SHADER_LIBRARY_SHADER_PRESET_LIT_SHADOW_COLOR:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/color.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/color.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_TEST:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.pass.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_DIFFUSE_POINT:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/diffuse.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/overlay.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_POINT_LIGHT:
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SPOT_LIGHT:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.spot.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.spot.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			begin_pipeline(renderer, &pipelines);
				add_color_blend_state(&pipelines, VK_FALSE);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/spot.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/lit/color/spot.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
			break;
		case SHADER_LIBRARY_SHADER_PRESET_SHADOW_MAP:
			begin_pipeline(renderer, &pipelines);
				set_depth_stencil(&pipelines, VK_TRUE, VK_TRUE);
				add_shader(&pipelines, "shaders/presets/lit/color/shadow.point.vert.spv", VULKAN_SHADER_TYPE_VERTEX);
				add_shader(&pipelines, "shaders/presets/diffuse/shadow.point.frag.spv", VULKAN_SHADER_TYPE_FRAGMENT);
			end_pipeline(renderer, &pipelines);
		break;
		default:
			UNSUPPORTED_PRESET(preset);
	}
	return buf_get_ptr(&pipelines);
}

static vulkan_shader_create_info_t* get_create_info_from_preset(vulkan_renderer_t* renderer, shader_library_shader_preset_t preset)
{
	vulkan_shader_create_info_t* create_info = memory_allocator_alloc_obj(renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_CREATE_INFO, vulkan_shader_create_info_t);
	create_info->material_set_bindings = create_material_set_binding(renderer, preset, &create_info->material_set_binding_count);
	create_info->vertex_infos = create_vertex_info(renderer, preset, &create_info->vertex_info_count);
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
	return CAST_TO(shader_t*, vulkan_shader_library_getH(library, handle));
}

RENDERER_API shader_t* shader_library_get(shader_library_t* library, const char* shader_name)
{
	return CAST_TO(shader_t*, vulkan_shader_library_get(library, shader_name));
}
