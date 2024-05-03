/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: library.c is a part of VulkanRenderer

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


#include <renderer/library.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>

RENDERER_API library_t* library_new(memory_allocator_t* allocator)
{
	library_t* lib = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_LIBRARY, library_t);
	memzero(lib, library_t);
	return lib;
}

RENDERER_API library_t* library_create(memory_allocator_t* allocator)
{
	library_t* lib = library_new(allocator);
	library_create_no_alloc(allocator, lib);
	return lib;
}

RENDERER_API void library_create_no_alloc(memory_allocator_t* allocator, library_t OUT lib)
{
	memzero(lib, library_t);
	lib->slots = buf_create(sizeof(library_slot_t), 1, 0);
	lib->relocation_table = buf_create(sizeof(library_slot_handle_t), 1, 0);
	lib->allocator = allocator;
}

RENDERER_API void library_destroy(library_t* lib)
{
	buf_clear(&lib->slots, NULL);
	buf_clear(&lib->relocation_table, NULL);
}

RENDERER_API void library_release_resources(library_t* lib)
{
	buf_free(&lib->slots);
	buf_free(&lib->relocation_table);

	// TODO
	// heap_free(lib);
}

RENDERER_API library_slot_handle_t library_create_slot(library_t* lib, void* object)
{
	buf_push_auto(&lib->relocation_table, buf_get_element_count(&lib->slots));

	library_slot_t slot = 
	{
		.object = object,
		.handle = buf_get_element_count(&lib->relocation_table) - 1
	};
	buf_push(&lib->slots, &slot);
	return slot.handle;
}

RENDERER_API void* library_getH(library_t* lib, library_slot_handle_t handle)
{
	_debug_assert__(handle != LIBRARY_SLOT_HANDLE_INVALID);
	buf_ucount_t index;
	buf_get_at(&lib->relocation_table, handle, &index);
	return CAST_TO(library_slot_t*, buf_get_ptr_at(&lib->slots, index))->object;
}

RENDERER_API void library_destroy_slotH(library_t* lib, library_slot_handle_t handle)
{
	_debug_assert__(handle != LIBRARY_SLOT_HANDLE_INVALID);
	buf_ucount_t index;
	buf_get_at(&lib->relocation_table, handle, &index);
	if(!buf_remove_at(&lib->slots, index, NULL))
		LOG_ERR("Unable to remove requested library slot handle: %ull\n, resolved index is: %ull\n", handle, index);
	else
	{
		buf_ucount_t count = buf_get_element_count(&lib->slots);
		for(buf_ucount_t i = index; i < count; i++)
		{
			library_slot_t* slot = CAST_TO(library_slot_t*, buf_get_ptr_at(&lib->slots, i));
			buf_ucount_t* _index = buf_get_ptr_at(&lib->relocation_table, slot->handle);
			if(*_index == LIBRARY_SLOT_HANDLE_INVALID)
				continue;
			(*_index) -= 1;
		}
		index = LIBRARY_SLOT_HANDLE_INVALID;
		buf_set_at(&lib->relocation_table, handle, &index);
	}
}

