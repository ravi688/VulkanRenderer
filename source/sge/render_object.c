/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_object.c is a part of VulkanRenderer

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


#include <sge/render_object.h>
#include <sge/sge.h>
#include <sge/internal/vulkan/vulkan_render_object.h>
#include <sge/alloc.h>

/* constructors and destructors */
SGE_API render_object_t* render_object_new(memory_allocator_t* allocator)
{
	return vulkan_render_object_new(allocator);
}

SGE_API render_object_t* render_object_create(renderer_t* renderer, render_object_create_info_t* create_info)
{
	vulkan_render_object_create_info_t vcreate_info =
	{
		.material = create_info->material,
		.user_data = create_info->user_data,
		.type = REINTERPRET_TO(vulkan_render_object_type_t, create_info->type),
		.draw_handler = create_info->draw_handler
	};
	return vulkan_render_object_create(renderer->vulkan_handle, &vcreate_info);
}

SGE_API void render_object_create_no_alloc(renderer_t* renderer, render_object_create_info_t* create_info, render_object_t OUT object)
{
	vulkan_render_object_create_info_t vcreate_info = 
	{
		.material = create_info->material,
		.user_data = create_info->user_data,
		.type = REINTERPRET_TO(vulkan_render_object_type_t, create_info->type),
		.draw_handler = create_info->draw_handler
	};
	vulkan_render_object_create_no_alloc(renderer->vulkan_handle, &vcreate_info, object);
}

SGE_API void render_object_destroy(render_object_t* object)
{
	vulkan_render_object_destroy(object);
}

SGE_API void render_object_release_resources(render_object_t* object)
{
	vulkan_render_object_release_resources(object);
}


SGE_API void render_object_attach(render_object_t* object, void* user_data)
{
	vulkan_render_object_attach(object, user_data);
}

SGE_API void render_object_draw(render_object_t* object)
{
	vulkan_render_object_draw(object);
}
SGE_API void render_object_set_material(render_object_t* obj, material_t* material)
{
	vulkan_render_object_set_material(obj, material);
}

SGE_API void render_object_set_transform(render_object_t* obj, mat4_t transform)
{
	vulkan_render_object_set_transform(obj, transform);
}

SGE_API mat4_t render_object_get_transform(render_object_t* obj)
{
	return vulkan_render_object_get_transform(obj);
}

SGE_API mat4_t render_object_get_normal(render_object_t* obj)
{
	return vulkan_render_object_get_normal(obj);
}
