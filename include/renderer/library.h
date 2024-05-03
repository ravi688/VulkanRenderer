/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: library.h is a part of VulkanRenderer

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


#include <renderer/defines.h>

#include <bufferlib/buffer.h>

typedef buf_ucount_t library_slot_handle_t;
#define LIBRARY_SLOT_HANDLE_INVALID (~0ULL)

typedef struct library_slot_t
{
	void* object;						// satellite data
	library_slot_handle_t handle; 		// handle to this slot
} library_slot_t;

typedef struct library_t
{
	memory_allocator_t* allocator;
	BUFFER slots;
	BUFFER relocation_table;
} library_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API library_t* library_new(memory_allocator_t* allocator);
RENDERER_API library_t* library_create(memory_allocator_t* allocator);
RENDERER_API void library_create_no_alloc(memory_allocator_t* allocator, library_t OUT lib);
RENDERER_API void library_destroy(library_t* lib);
RENDERER_API void library_release_resources(library_t* lib);

/* getters */
static FORCE_INLINE u32 library_get_count(library_t* lib) 
{
	return buf_get_element_count(&lib->slots);
}
static FORCE_INLINE void* library_get_at(library_t* lib, buf_ucount_t index)
{
	return CAST_TO(library_slot_t*, buf_get_ptr_at(&lib->slots, index))->object;
}
RENDERER_API void* library_getH(library_t* lib, library_slot_handle_t handle);

RENDERER_API library_slot_handle_t library_create_slot(library_t* lib, void* object);
RENDERER_API void library_destroy_slotH(library_t* lib, library_slot_handle_t handle);

END_CPP_COMPATIBLE

