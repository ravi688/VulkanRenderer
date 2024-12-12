/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_material.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_material.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_texture.h>
#include <sge/internal/vulkan/vulkan_shader.h>
#include <sge/internal/vulkan/vulkan_pipeline_layout.h>
#include <sge/internal/vulkan/vulkan_types.h>
#include <sge/internal/vulkan/vulkan_shader_resource_description.h>
#include <sge/assert.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>

SGE_API vulkan_material_t* vulkan_material_new(memory_allocator_t* allocator)
{
	vulkan_material_t* material = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_MATERIAL, vulkan_material_t);
	memzero(material, vulkan_material_t);
	VULKAN_OBJECT_INIT(material, VULKAN_OBJECT_TYPE_MATERIAL, VULKAN_OBJECT_NATIONALITY_INTERNAL);
	return material;
}

static bool is_sampler_type(glsl_type_t type)
{
	switch(type)
	{
		case GLSL_TYPE_SAMPLER_1D:
		case GLSL_TYPE_SAMPLER_2D:
		case GLSL_TYPE_SAMPLER_3D:
		case GLSL_TYPE_SAMPLER_CUBE:
			return true;
		default:
			return false;
	}
}

static u16 calculate_uniform_resource_count(vulkan_shader_t* shader)
{
	_debug_assert__(shader != NULL);
	if(shader->material_set_bindings == NULL) return 0;
	u16 count = shader->material_set_binding_count;
	u16 uniform_count = 0;
	for(u16 i = 0; i < count; i++)
	{
		AUTO type = shader->material_set_bindings[i].handle.type;
		if((type == GLSL_TYPE_UNIFORM_BUFFER) || is_sampler_type(type))
			++uniform_count;
	}
	return uniform_count;
}

/* types of bindings which define a material, 
 * for example, buffer objects hold data about the material's variables
 * 	and textures hold data about the materials' appearance
 * With that concept, we do not classify subpassInput bindings as material resource.
 * A material resource is something which can be set by calling one of the material setters. 

 * There can be other such types but for now we only support uniform samplers, push constants, storage buffers, and uniform buffers */
static bool is_material_resource(vulkan_shader_resource_description_t* binding)
{
	switch(binding->handle.type)
	{
		case GLSL_TYPE_SAMPLER_1D:
		case GLSL_TYPE_SAMPLER_2D:
		case GLSL_TYPE_SAMPLER_3D:
		case GLSL_TYPE_SAMPLER_CUBE:
		case GLSL_TYPE_PUSH_CONSTANT:
		case GLSL_TYPE_UNIFORM_BUFFER:
		case GLSL_TYPE_STORAGE_BUFFER:
			return true;
		default:
			return false;
	}
}

static void setup_material_resources(vulkan_material_t* material)
{
	_debug_assert__(material->shader != NULL);
	u16 count = calculate_uniform_resource_count(material->shader);
	if(count == 0) return;

	vulkan_shader_resource_description_t* bindings = material->shader->material_set_bindings;
	vulkan_uniform_resource_t* uniform_resources = memory_allocator_alloc_obj_array(material->renderer->allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_UNIFORM_RESOURCE_ARRAY, vulkan_uniform_resource_t, count);
	safe_memzerov(uniform_resources, vulkan_uniform_resource_t, count);
	for(u16 i = 0, j = 0; i < material->shader->material_set_binding_count; i++)
	{
		vulkan_shader_resource_description_t* binding = &bindings[i];
		if(is_material_resource(binding))
		{
			_debug_assert__(j < count);

			vulkan_uniform_resource_t* resource = &uniform_resources[j];
			resource->index = i;
			if(binding->handle.type == GLSL_TYPE_UNIFORM_BUFFER)
			{
				/* if this uniform interface block has a fixed size then create vulkan uniform buffer for it */
				if(!struct_descriptor_is_variable_sized(&binding->handle))
				{
					u32 size = struct_descriptor_sizeof(&binding->handle);
					vulkan_buffer_create_info_t create_info =
					{
						.size = size,
						.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					};
					resource->buffer = vulkan_buffer_create(material->renderer, &create_info);
					vulkan_descriptor_set_write_uniform_buffer(&material->material_set, binding->binding_number, resource->buffer);
				}
				/* otherwise don't create vulkan uniform buffer for it */
			}
			else
				resource->index = 0xFFFF;
			j++;
		}
	}
	material->uniform_resources = uniform_resources;
	material->uniform_resource_count = count;
}

SGE_API void vulkan_material_create_no_alloc(vulkan_renderer_t* renderer, vulkan_shader_t* shader, vulkan_material_t* material)
{
	VULKAN_OBJECT_MEMZERO(material, vulkan_material_t);

	material->renderer = renderer;
	material->handle = VULKAN_MATERIAL_HANDLE_INVALID;
	material->shader = shader;

	// create MATERIAL_SET
	vulkan_descriptor_set_create_info_t create_info =
	{
		.vo_pool = renderer->vo_descriptor_pool,
		.layout = &shader->material_set_layout
	};
	vulkan_descriptor_set_create_no_alloc_ext(renderer, &create_info, &material->material_set);
	setup_material_resources(material);
}

SGE_API vulkan_material_t* vulkan_material_create(vulkan_renderer_t* renderer, vulkan_shader_t* shader)
{
	// allocate memory and intitalize
	vulkan_material_t* material = vulkan_material_new(renderer->allocator);

	// create material
	vulkan_material_create_no_alloc(renderer, shader, material);
	return material;
}

SGE_API void vulkan_material_destroy(vulkan_material_t* material)
{
	vulkan_descriptor_set_destroy(&material->material_set);
	for(u16 i = 0; i < material->uniform_resource_count; i++)
		if(material->uniform_resources[i].buffer)
			vulkan_buffer_destroy(material->uniform_resources[i].buffer);
}

SGE_API void vulkan_material_release_resources(vulkan_material_t* material)
{
	vulkan_descriptor_set_release_resources(&material->material_set);
	for(u16 i = 0; i < material->uniform_resource_count; i++)
		if(material->uniform_resources[i].buffer)
			vulkan_buffer_release_resources(material->uniform_resources[i].buffer);
	if(material->uniform_resource_count != 0)
		memory_allocator_dealloc(material->renderer->allocator, material->uniform_resources);
	if(VULKAN_OBJECT_IS_INTERNAL(material))
		memory_allocator_dealloc(material->renderer->allocator, material);
}

static VkShaderStageFlagBits get_vulkan_shader_flags(u8 _flags);

static void set_push_constants(vulkan_material_t* material, vulkan_shader_resource_description_t* descriptor, vulkan_pipeline_layout_t* pipeline_layout)
{
	vulkan_pipeline_layout_push_constants(pipeline_layout,
											get_vulkan_shader_flags(descriptor->stage_flags),
											descriptor->push_constant_range_offset,
											struct_descriptor_sizeof(&descriptor->handle),
											descriptor->handle.ptr);
}

SGE_API void vulkan_material_push_constants(vulkan_material_t* material, vulkan_pipeline_layout_t* pipeline_layout)
{
	u32 binding_count = material->shader->material_set_binding_count;
	for(u32 i = 0; i < binding_count; i++)
		if(material->shader->material_set_bindings[i].is_push_constant)
			set_push_constants(material, &material->shader->material_set_bindings[i], pipeline_layout);
}

/* ------------------------------------------ BEGIN: PUSH CONSTANTS -------------------------------------------*/

// TODO: fix this branching
static VkShaderStageFlagBits get_vulkan_shader_flags(u8 _flags)
{
	VkShaderStageFlagBits flags = 0;
	if(_flags & (1 << VULKAN_SHADER_TYPE_VERTEX))
		flags |= VK_SHADER_STAGE_VERTEX_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_FRAGMENT))
		flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_GEOMETRY))
		flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
	if(_flags & (1 << VULKAN_SHADER_TYPE_TESSELLATION))
		flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	return flags;
}

// setters
/* functions accepting handles */
#define set_push_value(material, handle, setter, in_value) __set_push_value(material, handle, (void (*)(struct_descriptor_t*, struct_field_handle_t, const void* const))(setter), in_value);
static void __set_push_value(vulkan_material_t* material, vulkan_material_field_handle_t handle, void (*setter)(struct_descriptor_t*, struct_field_handle_t, const void* const), const void* const in)
{
	vulkan_shader_resource_description_t* descriptor = &(material->shader->material_set_bindings[handle.index]);
	setter(&descriptor->handle, handle.field_handle, in);
	// set_push_constants(material, descriptor);
}

SGE_API void vulkan_material_set_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value)
{
	set_push_value(material, handle, struct_descriptor_set_float, &value);
}

SGE_API void vulkan_material_set_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value)
{
	set_push_value(material, handle, struct_descriptor_set_int, &value);
}

SGE_API void vulkan_material_set_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value)
{
	set_push_value(material, handle, struct_descriptor_set_uint, &value);
}

SGE_API void vulkan_material_set_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec2, &value);
}

SGE_API void vulkan_material_set_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec3, &value);
}

SGE_API void vulkan_material_set_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t value)
{
	set_push_value(material, handle, struct_descriptor_set_vec4, &value);
}

SGE_API void vulkan_material_set_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t value)
{
	set_push_value(material, handle, struct_descriptor_set_mat2, &value);
}

SGE_API void vulkan_material_set_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t value)
{
	set_push_value(material, handle, struct_descriptor_set_mat4, &value);
}

// getters
#define get_push_value(material, handle, getter, out_value) __get_push_value(material, handle, (void (*)(struct_descriptor_t*, struct_field_handle_t, void* const))(getter), out_value);
static void __get_push_value(vulkan_material_t* material, vulkan_material_field_handle_t handle, void (*getter)(struct_descriptor_t*, struct_field_handle_t, void* const), void* const out)
{
	vulkan_shader_resource_description_t* descriptor = &(material->shader->material_set_bindings[handle.index]);
	getter(&descriptor->handle, handle.field_handle, out);
}

SGE_API float vulkan_material_get_push_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	float value;
	get_push_value(material, handle, struct_descriptor_get_float, &value);
	return value;
}

SGE_API int vulkan_material_get_push_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	int value;
	get_push_value(material, handle, struct_descriptor_get_int, &value);
	return value;
}

SGE_API uint vulkan_material_get_push_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	uint value;
	get_push_value(material, handle, struct_descriptor_get_uint, &value);
	return value;
}

SGE_API vec2_t vulkan_material_get_push_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec2_t value;
	get_push_value(material, handle, struct_descriptor_get_vec2, &value);
	return value;
}

SGE_API vec3_t vulkan_material_get_push_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec3_t value;
	get_push_value(material, handle, struct_descriptor_get_vec3, &value);
	return value;
}

SGE_API vec4_t vulkan_material_get_push_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec4_t value;
	get_push_value(material, handle, struct_descriptor_get_vec4, &value);
	return value;
}

SGE_API mat2_t vulkan_material_get_push_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat2_t value;
	get_push_value(material, handle, struct_descriptor_get_mat2, &value);
	return value;
}

SGE_API mat4_t vulkan_material_get_push_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat4_t value;
	get_push_value(material, handle, struct_descriptor_get_mat4, &value);
	return value;
}

/* functions accepting strings */
SGE_API void vulkan_material_set_push_float(vulkan_material_t* material, const char* name, float value)
{
	vulkan_material_set_push_floatH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_push_int(vulkan_material_t* material, const char* name, int value)
{
	vulkan_material_set_push_intH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_push_uint(vulkan_material_t* material, const char* name, uint value)
{
	vulkan_material_set_push_uintH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_push_vec2(vulkan_material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_push_vec2H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_push_vec3(vulkan_material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_push_vec3H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_push_vec4(vulkan_material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_push_vec4H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_push_mat2(vulkan_material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_push_mat2H(material, vulkan_material_get_field_handle(material, name), m);
}

SGE_API void vulkan_material_set_push_mat4(vulkan_material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_push_mat4H(material, vulkan_material_get_field_handle(material, name), m);
}

SGE_API float vulkan_material_get_push_float(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_floatH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API int vulkan_material_get_push_int(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_intH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API uint vulkan_material_get_push_uint(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_uintH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec2_t vulkan_material_get_push_vec2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec2H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec3_t vulkan_material_get_push_vec3(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec3H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec4_t vulkan_material_get_push_vec4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_vec4H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API mat2_t vulkan_material_get_push_mat2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_mat2H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API mat4_t vulkan_material_get_push_mat4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_push_mat4H(material, vulkan_material_get_field_handle(material, name));
}

/* ------------------------------------------ END: PUSH CONSTANTS -------------------------------------------*/

static void unmap_descriptor(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	struct_descriptor_t* descriptor = &(material->shader->material_set_bindings[handle.index].handle);
	vulkan_buffer_t* buffer = material->uniform_resources[handle.uniform_index].buffer;
	if(buffer)
	{
		struct_descriptor_unmap(descriptor);
		vulkan_buffer_unmap(buffer);
	}
	else
		debug_log_error("Failed to unmap struct descriptor as the field handle doesn't point to a valid to a valid vulkan buffer");
}

static struct_descriptor_t* map_descriptor(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	struct_descriptor_t* descriptor = &(material->shader->material_set_bindings[handle.index].handle);
	vulkan_buffer_t* buffer = material->uniform_resources[handle.uniform_index].buffer;
	if(buffer)
		struct_descriptor_map(descriptor, vulkan_buffer_map(buffer));
	else
		debug_log_error("Failed to map struct descriptor to vulkan buffer as the field handle doesn't point to valid vulkan buffer");
	return descriptor;
}

SGE_API void vulkan_material_set_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle, float value)
{
	struct_descriptor_set_float(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle, int value)
{
	struct_descriptor_set_int(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle, uint value)
{
	struct_descriptor_set_uint(map_descriptor(material, handle), handle.field_handle, &value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec2_t value)
{
	struct_descriptor_set_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec3_t value)
{
	struct_descriptor_set_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, vec4_t value)
{
	struct_descriptor_set_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_uvec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec2_t value)
{
	struct_descriptor_set_uvec2(map_descriptor(material, handle), handle.field_handle, (uint*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_uvec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec3_t value)
{
	struct_descriptor_set_uvec3(map_descriptor(material, handle), handle.field_handle, (uint*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_uvec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, uvec4_t value)
{
	struct_descriptor_set_uvec4(map_descriptor(material, handle), handle.field_handle, (uint*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat2_t value)
{
	struct_descriptor_set_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle, mat4_t value)
{
	struct_descriptor_set_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	unmap_descriptor(material, handle);
}

SGE_API void vulkan_material_set_textureH(vulkan_material_t* material, vulkan_material_field_handle_t handle, vulkan_texture_t* texture)
{
	_debug_assert__(handle.index < material->shader->material_set_binding_count);
	vulkan_shader_resource_description_t descriptor = material->shader->material_set_bindings[handle.index];
	AUTO type = descriptor.handle.type;
	if((type != GLSL_TYPE_SAMPLER_2D) && (type != GLSL_TYPE_SAMPLER_3D) && (type != GLSL_TYPE_SAMPLER_CUBE))
	{
		debug_log_error("You're trying to write a texture to a non-texture type descriptor");
		return;
	}
	else
	{
		switch(type)
		{
			case GLSL_TYPE_SAMPLER_CUBE:
				if(!vulkan_texture_is_cube(texture))
				{
					debug_log_error("You're trying to write a non-cube texture to a descriptor of type sampler cube");
					return;
				}
				break;
			case GLSL_TYPE_SAMPLER_3D:
				if(!vulkan_texture_is_3d(texture))
				{
					debug_log_error("You're trying to write a non-3d texture to a descriptor of type sampler 3D");
					return;
				}
				break;
			case GLSL_TYPE_SAMPLER_2D:
				if(!vulkan_texture_is_2d(texture))
				{
					debug_log_error("You're trying to write a non-2d texture to a descriptor of type sampler 2D");
					return;
				}
				break;
			case GLSL_TYPE_SAMPLER_1D:
				if(!vulkan_texture_is_1d(texture))
				{
					debug_log_fetal_error("You're tyring to write an invalid texture to a descriptor of type sampler 1D as none of the samplerCube, sampler3D, sampler2D, or sampler1D match");
					return;
				}
				break;
			default: break;
		}
	}
	// _debug_assert__(descriptor.set_number < 1); 	//for now we are just using one descriptor set and multiple bindings
	vulkan_descriptor_set_write_texture(&material->material_set, descriptor.binding_number, texture);
	material->uniform_resources[handle.uniform_index].texture = texture;
}

SGE_API float vulkan_material_get_floatH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	float value;
	struct_descriptor_get_float(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

SGE_API int vulkan_material_get_intH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	int value;
	struct_descriptor_get_int(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

SGE_API uint vulkan_material_get_uintH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	uint value;
	struct_descriptor_get_uint(map_descriptor(material, handle), handle.field_handle, &value);
	return value;
}

SGE_API vec2_t vulkan_material_get_vec2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec2_t value;
	struct_descriptor_get_vec2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

SGE_API vec3_t vulkan_material_get_vec3H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec3_t value;
	struct_descriptor_get_vec3(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

SGE_API vec4_t vulkan_material_get_vec4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	vec4_t value;
	struct_descriptor_get_vec4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

SGE_API mat2_t vulkan_material_get_mat2H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat2_t value;
	struct_descriptor_get_mat2(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

SGE_API mat4_t vulkan_material_get_mat4H(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	mat4_t value;
	struct_descriptor_get_mat4(map_descriptor(material, handle), handle.field_handle, (float*)&value);
	return value;
}

SGE_API vulkan_texture_t* vulkan_material_get_textureH(vulkan_material_t* material, vulkan_material_field_handle_t handle)
{
	_debug_assert__(handle.index < material->shader->material_set_binding_count);
	_com_assert(handle.uniform_index < material->uniform_resource_count);
	return material->uniform_resources[handle.uniform_index].texture;
}

/* functions accepting strings */
SGE_API void vulkan_material_set_float(vulkan_material_t* material, const char* name, float value)
{
	vulkan_material_set_floatH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_int(vulkan_material_t* material, const char* name, int value)
{
	vulkan_material_set_intH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_uint(vulkan_material_t* material, const char* name, uint value)
{
	vulkan_material_set_uintH(material, vulkan_material_get_field_handle(material, name), value);
}

SGE_API void vulkan_material_set_vec2(vulkan_material_t* material, const char* name, vec2_t v)
{
	vulkan_material_set_vec2H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_vec3(vulkan_material_t* material, const char* name, vec3_t v)
{
	vulkan_material_set_vec3H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_vec4(vulkan_material_t* material, const char* name, vec4_t v)
{
	vulkan_material_set_vec4H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_uvec2(vulkan_material_t* material, const char* name, uvec2_t v)
{
	vulkan_material_set_uvec2H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_uvec3(vulkan_material_t* material, const char* name, uvec3_t v)
{
 	vulkan_material_set_uvec3H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_uvec4(vulkan_material_t* material, const char* name, uvec4_t v)
{
	vulkan_material_set_uvec4H(material, vulkan_material_get_field_handle(material, name), v);
}

SGE_API void vulkan_material_set_mat2(vulkan_material_t* material, const char* name, mat2_t m)
{
	vulkan_material_set_mat2H(material, vulkan_material_get_field_handle(material, name), m);
}

SGE_API void vulkan_material_set_mat4(vulkan_material_t* material, const char* name, mat4_t m)
{
	vulkan_material_set_mat4H(material, vulkan_material_get_field_handle(material, name), m);
}

SGE_API void vulkan_material_set_texture(vulkan_material_t* material, const char* name, vulkan_texture_t* texture)
{
	vulkan_material_set_textureH(material, vulkan_material_get_field_handle(material, name), texture);
}

SGE_API float vulkan_material_get_float(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_floatH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API int vulkan_material_get_int(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_intH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API uint vulkan_material_get_uint(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_uintH(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec2_t vulkan_material_get_vec2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec2H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec3_t vulkan_material_get_vec3(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec3H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vec4_t vulkan_material_get_vec4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_vec4H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API mat2_t vulkan_material_get_mat2(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_mat2H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API mat4_t vulkan_material_get_mat4(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_mat4H(material, vulkan_material_get_field_handle(material, name));
}

SGE_API vulkan_texture_t* vulkan_material_get_texture(vulkan_material_t* material, const char* name)
{
	return vulkan_material_get_textureH(material, vulkan_material_get_field_handle(material, name));
}

/* returns true if 'name' has anything after '.', called field, otherwise returns false */
static bool get_record_and_field_name(const char* const full_name, char out_struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE], char out_field_name[STRUCT_FIELD_MAX_NAME_SIZE])
{
	u32 len = strlen(full_name);
	_debug_assert__(len != 0);
	const char* ptr = strchr(full_name, '.');
	if(out_field_name != NULL)
		memset(out_field_name, 0, STRUCT_FIELD_MAX_NAME_SIZE);
	memset(out_struct_name, 0, STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
	if(ptr == NULL)
	{
		memcopyv(out_struct_name, full_name, u8, len);
		if(out_field_name != NULL)
			strcpy(out_field_name, "<UndefinedField>");
		return false;
	}
	u16 struct_name_len = (u16)(ptr - full_name);
	u16 field_name_len = (u16)(len - struct_name_len - 1);
	_debug_assert__(struct_name_len < STRUCT_DESCRIPTOR_MAX_NAME_SIZE);
	_debug_assert__(field_name_len < STRUCT_FIELD_MAX_NAME_SIZE);
	memcopyv(out_struct_name, full_name, u8, struct_name_len);
	if(out_field_name != NULL)
		memcopyv(out_field_name, ptr + 1, u8, field_name_len);
	return true;
}

SGE_API vulkan_material_field_handle_t vulkan_material_get_field_handle(vulkan_material_t* material, const char* name)
{
	_debug_assert__(material->shader != NULL);

	if(material->shader->material_set_binding_count == 0)
		LOG_WRN("Couldn't get field handle to \"%s\", reason: material->shader->material_set_binding_count == 0\n", name);

	char struct_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE];
	char field_name[STRUCT_FIELD_MAX_NAME_SIZE];
	bool is_field_requested = get_record_and_field_name(name, struct_name, field_name);

	u16 binding_count = material->shader->material_set_binding_count;
	vulkan_shader_resource_description_t* bindings = material->shader->material_set_bindings;
	u16 index = 0xFFFF;
	u16 uniform_index = 0xFFFF;
	struct_field_handle_t field_handle = STRUCT_FIELD_INVALID_HANDLE;
	for(u16 i = 0, j = 0; i < binding_count; i++)
	{
		vulkan_shader_resource_description_t* binding = &bindings[i];
		if(vulkan_shader_resource_description_is_attribute(binding))
			continue;
		if(strcmp(binding->handle.name, struct_name) == 0)
		{
			field_handle = struct_descriptor_get_field_handle(&binding->handle, field_name);
			index = i;
			uniform_index = j;
			if(field_handle == STRUCT_FIELD_INVALID_HANDLE)
			{
				/* if a field is requested in an interface block, but the field handle is not found in the actual shader interface block
				 * then throw an error. */
				if(is_field_requested)
					LOG_FETAL_ERR("Symbol \"%s\" isn't found in the shader resource bindings\n", name);
				continue;
			}
			break;
		}
		j++;
	}
	if((index != 0xFFFF) && (uniform_index != 0xFFFF))
		return (vulkan_material_field_handle_t) { .index = index, .uniform_index = uniform_index, .field_handle = field_handle };
	LOG_FETAL_ERR("Symbol \"%s\" isn't found in the shader resource bindings\n", name);
	return (vulkan_material_field_handle_t) { .index = 0xFFFF, .uniform_index = 0xFFFF, .field_handle = STRUCT_FIELD_INVALID_HANDLE };
}

typedef_pair_t(vulkan_uniform_resource_ptr_t, vulkan_shader_resource_description_ptr_t);

static pair_t(vulkan_uniform_resource_ptr_t, vulkan_shader_resource_description_ptr_t) get_uniform_resource_descriptor(vulkan_material_t* material, const char* block_name)
{
	for(u16 i = 0; i < material->uniform_resource_count; i++)
	{
		vulkan_uniform_resource_t* uniform = &material->uniform_resources[i];
		vulkan_shader_resource_description_t* binding = &material->shader->material_set_bindings[uniform->index];
		if(strcmp(binding->handle.name, block_name) == 0)
			return make_pair_t(vulkan_uniform_resource_ptr_t, vulkan_shader_resource_description_ptr_t) { uniform, binding };
	}
	return make_pair_t(vulkan_uniform_resource_ptr_t, vulkan_shader_resource_description_ptr_t) { NULL, NULL };
}

SGE_API void vulkan_material_set_array_size(vulkan_material_t* material, const char* name, u32 size)
{
	/* split the name into block_name and field_name */
	char block_name[STRUCT_DESCRIPTOR_MAX_NAME_SIZE];
	char field_name[STRUCT_FIELD_MAX_NAME_SIZE];
	get_record_and_field_name(name, block_name, field_name);
	release_assert__(strcmp(field_name, "<UndefinedField>") != 0, "\"%s\" is not a fully qualified array name", name);

	AUTO resource_descriptor = get_uniform_resource_descriptor(material, block_name);

	_debug_assert__(resource_descriptor.second != NULL);

	/* get the interface block descriptor and set the array size */
	struct_descriptor_t* descriptor = &resource_descriptor.second->handle;
	release_assert__(descriptor != NULL, "Unable to find block name \"%s\" in the material", block_name);
	struct_descriptor_set_array_size(descriptor, field_name, size);

	_debug_assert__(!struct_descriptor_is_variable_sized(descriptor));
}

SGE_API void vulkan_material_set_buffer(vulkan_material_t* material, const char* block_name, vulkan_buffer_t* buffer)
{
	AUTO resource_descriptor = get_uniform_resource_descriptor(material, block_name);
	release_assert__(resource_descriptor.first != NULL, "Unable to find block name \"%s\" in the material", block_name);

	AUTO binding = resource_descriptor.second;

	/* if the buffer is NULL then create a new vulkan buffer */
	if(buffer == NULL)
	{
		AUTO uniform = resource_descriptor.first;
		vulkan_buffer_create_info_t create_info =
		{
			.size = struct_descriptor_sizeof(&binding->handle),
			.vo_usage_flags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			.vo_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		uniform->buffer = vulkan_buffer_create(material->renderer, &create_info);
		buffer = uniform->buffer;
	}

	/* write the descriptor */
	vulkan_descriptor_set_write_uniform_buffer(&material->material_set, binding->binding_number, buffer);
}

SGE_API const struct_descriptor_t* vulkan_material_get_interface(vulkan_material_t* material, const char* block_name)
{
	AUTO resource_descriptor = get_uniform_resource_descriptor(material, block_name);
	_debug_assert__(resource_descriptor.second != NULL);
	return &resource_descriptor.second->handle;
}
