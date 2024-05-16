/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_shader_resource_description_builder.c is a part of VulkanRenderer

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

#include <renderer/internal/vulkan/vulkan_shader_resource_description_builder.h>
#include <renderer/internal/vulkan/vulkan_shader_resource_description.h>
#include <renderer/memory_allocator.h>
#include <renderer/assert.h>

RENDERER_API vulkan_shader_resource_description_builder_t* vulkan_shader_resource_description_builder_create(memory_allocator_t* allocator)
{
	vulkan_shader_resource_description_builder_t* builder = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_SHADER_RESOURCE_DESCRIPTION_BUILDER, vulkan_shader_resource_description_builder_t);
	memzero(builder, vulkan_shader_resource_description_builder_t);
	builder->allocator = allocator;
	builder->description_array = memory_allocator_buf_new(allocator, vulkan_shader_resource_description_t);
	builder->bind_index = U32_MAX;
	return builder;
}

RENDERER_API vulkan_shader_resource_description_builder_t* vulkan_shader_resource_description_builder_create_inverse(memory_allocator_t* allocator, vulkan_shader_resource_description_t* descriptions, u32 description_count)
{
	vulkan_shader_resource_description_builder_t* builder = vulkan_shader_resource_description_builder_create(allocator);
	vulkan_shader_resource_description_builder_add(builder, description_count);
	for(u32 i = 0; i < description_count; i++)
	{
		vulkan_shader_resource_description_builder_bind(builder, i);
		vulkan_shader_resource_description_t* description = &descriptions[i];
		glsl_type_t glsl_type = CAST_TO(glsl_type_t, struct_descriptor_get_type(&description->handle));
		debug_assert__(glsl_type != GLSL_TYPE_PUSH_CONSTANT, "We haven't tested push constants, so it might not work");
		debug_assert__(glsl_type != GLSL_TYPE_STORAGE_BUFFER, "We haven't tested storaged buffers, so it might not work");
		switch(glsl_type)
		{
			case GLSL_TYPE_UNIFORM_BUFFER:
			{
				vulkan_shader_resource_description_builder_create_uniform(builder, struct_descriptor_get_name(&description->handle), description->set_number, description->binding_number);
				break;
			}
			case GLSL_TYPE_SAMPLER_CUBE:
			case GLSL_TYPE_SAMPLER_2D:
			case GLSL_TYPE_SUBPASS_INPUT:
			{
				vulkan_shader_resource_description_builder_create_opaque(builder, struct_descriptor_get_name(&description->handle), struct_descriptor_get_type(&description->handle), description->set_number, description->binding_number);
				break;
			}
			default:
			{
				if(vulkan_shader_resource_description_is_attribute(description))
				{
					vulkan_shader_resource_description_builder_create_vertex_attribute(builder, struct_descriptor_get_name(&description->handle), struct_descriptor_get_type(&description->handle), description->vertex_attrib_location_number, description->vertex_attrib_binding_number);
					break;
				}
				DEBUG_LOG_FETAL_ERROR("Untested and perhaps unsupported glsl_type_t: %u", glsl_type);
				break;
			}
		}
	}
	return builder;
}

RENDERER_API void vulkan_shader_resource_description_builder_destroy(vulkan_shader_resource_description_builder_t* builder)
{
	u32 description_count = buf_get_element_count(&builder->description_array);
	for(u32 i = 0; i < description_count; i++)
	{
		AUTO description = buf_get_ptr_at_typeof(&builder->description_array, vulkan_shader_resource_description_t, i);
		struct_descriptor_destroy(builder->allocator, &description->handle);
	}
	buf_free(&builder->description_array);
	memory_allocator_dealloc(builder->allocator, builder);
}


RENDERER_API void vulkan_shader_resource_description_builder_add(vulkan_shader_resource_description_builder_t* builder, u32 count)
{
	buf_push_pseudo(&builder->description_array, count);
}

RENDERER_API void vulkan_shader_resource_description_builder_bind(vulkan_shader_resource_description_builder_t* builder, u32 index)
{
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	builder->bind_index = index;
}


RENDERER_API vulkan_shader_resource_description_t* vulkan_shader_resource_description_builder_get(vulkan_shader_resource_description_builder_t* builder)
{
	if(buf_get_element_count(&builder->description_array) == 0)
		return NULL;
	return CAST_TO(vulkan_shader_resource_description_t*, buf_get_ptr(&builder->description_array));
}

RENDERER_API u32 vulkan_shader_resource_description_builder_get_count(vulkan_shader_resource_description_builder_t* builder)
{
	return CAST_TO(u32, buf_get_element_count(&builder->description_array));
}


static vulkan_shader_resource_description_t* get_description(vulkan_shader_resource_description_builder_t* builder, u32 index)
{
	_debug_assert__(index != U32_MAX);
	_debug_assert__(index < buf_get_element_count(&builder->description_array));
	return buf_get_ptr_at_typeof(&builder->description_array, vulkan_shader_resource_description_t, index);
}

static INLINE_IF_RELEASE_MODE vulkan_shader_resource_description_t* get_bound_description(vulkan_shader_resource_description_builder_t* builder)
{
	return get_description(builder, builder->bind_index);
}

RENDERER_API void vulkan_shader_resource_description_builder_create_vertex_attribute(vulkan_shader_resource_description_builder_t* builder, const char* name, glsl_type_t type, u32 location_number, u32 binding_number)
{
	vulkan_shader_resource_description_create_opaque(builder->allocator, get_bound_description(builder), name, type, location_number, binding_number);
}

RENDERER_API void vulkan_shader_resource_description_builder_create_opaque(vulkan_shader_resource_description_builder_t* builder, const char* name, glsl_type_t type, u32 set_number, u32 binding_number)
{
	vulkan_shader_resource_description_create_opaque(builder->allocator, get_bound_description(builder), name, type, set_number, binding_number);
}

RENDERER_API struct_descriptor_t* vulkan_shader_resource_description_builder_create_uniform(vulkan_shader_resource_description_builder_t* builder, const char* name, u32 set_number, u32 binding_number)
{
	return vulkan_shader_resource_description_create_uniform(builder->allocator, get_bound_description(builder), name, set_number, binding_number);
}

RENDERER_API void vulkan_shader_resource_description_builder_end_uniform(vulkan_shader_resource_description_builder_t* builder)
{
	vulkan_shader_resource_description_end_uniform(builder->allocator, get_bound_description(builder));
}

