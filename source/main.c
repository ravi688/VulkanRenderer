/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: main.c is a part of VulkanRenderer

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


#include <renderer/test.h>

#define RENDERER_INCLUDE_TIMING
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>
#include <renderer/legal/legal.h>

#include <conio.h>
#include <signal.h>

static allocate_result_t allocate(u32 size, u32 align, void* user_data)
{
	if((size == 0) || (align == 0))
		return (allocate_result_t) { ALLOCATE_RESULT_INVALID_REQUEST, NULL };

	void* ptr = heap_alloc(size);
	if(ptr == NULL)
		return (allocate_result_t) { ALLOCATE_RESULT_OUT_OF_MEMORY, NULL };

	return (allocate_result_t) { ALLOCATE_RESULT_SUCCESS, ptr };
}

// static allocate_result_t reallocate(void* old_ptr, u32 old_size, u32 old_align, u32 size, u32 align, void* user_data)
// {
// 	void* new_ptr = aligned_alloc(size, align);
	
// 		if(old_ptr == NULL)
// 			return new_ptr;
	
// 		memcopyv(new_ptr, old_ptr, old_align);
	
// 		free(old_ptr);
	
// 		return new_ptr;
// }

static void deallocate(void* ptr, void* user_data)
{
	heap_free(ptr);
}

static void memory_allocation_tree_dump(memory_allocator_t* allocator, const char* file_path)
{
	debug_log_info("Building memory allocation tree...");
	memory_allocation_tree_t* tree = memory_allocator_build_allocation_tree(allocator);
	memory_allocation_tree_serialize_to_file(tree, file_path);
	memory_allocation_tree_destroy(tree);
	debug_log_info("Memory allocation tree built successfully");
}

static memory_allocator_t* allocator;

#define signal_handler(signal, handler) static void signal##_handler(int signal) { handler; }

signal_handler(__SIGINT, memory_allocation_tree_dump(allocator, "__SIGINT.runtime.dump"))
signal_handler(__SIGILL, memory_allocation_tree_dump(allocator, "__SIGILL.runtime.dump"))
signal_handler(__SIGABRT_COMPAT, memory_allocation_tree_dump(allocator, "__SIGABRT_COMPAT.runtime.dump"))
signal_handler(__SIGFPE, memory_allocation_tree_dump(allocator, "__SIGFPE.runtime.dump"))
signal_handler(__SIGSEGV, memory_allocation_tree_dump(allocator, "__SIGSEGV.runtime.dump"))
signal_handler(__SIGTERM, memory_allocation_tree_dump(allocator, "__SIGTERM.runtime.dump"))
signal_handler(__SIGBREAK, memory_allocation_tree_dump(allocator, "__SIGBREAK.runtime.dump"))
signal_handler(__SIGABRT, memory_allocation_tree_dump(allocator, "__SIGABRT.runtime.dump"))
signal_handler(__SIGABRT2, memory_allocation_tree_dump(allocator, "__SIGABRT2.runtime.dump"))

/* TODO: there are extra things have to be done to get memory dump whenever the program crashes,
 * 1. all the assertions must use __builtin_trap() to raise SIGILL.
 * 2. all the fetal errors or just errors must use __builtin_trap() to raise SIGILL. */
static void setup_signal_handlers()
{
	_debug_assert__(signal(SIGINT, __SIGINT_handler) != SIG_ERR);
	_debug_assert__(signal(SIGILL, __SIGILL_handler) != SIG_ERR);
	_debug_assert__(signal(SIGABRT_COMPAT, __SIGABRT_COMPAT_handler) != SIG_ERR);
	_debug_assert__(signal(SIGFPE, __SIGFPE_handler) != SIG_ERR);
	_debug_assert__(signal(SIGSEGV, __SIGSEGV_handler) != SIG_ERR);
	_debug_assert__(signal(SIGTERM, __SIGTERM_handler) != SIG_ERR);
	_debug_assert__(signal(SIGBREAK, __SIGBREAK_handler) != SIG_ERR);
	_debug_assert__(signal(SIGABRT, __SIGABRT_handler) != SIG_ERR);
	_debug_assert__(signal(SIGABRT2, __SIGABRT2_handler) != SIG_ERR);
}

int main(int argc, const char** argv)
{
	legal_show_notice(argv[0]);
	alloc_init(&argv);

	memory_allocator_create_info_t create_info = 
	{
		// .allocate = allocate,
		// .reallocate = reallocate,
		// .deallocate = deallocate
	};
	allocator = memory_allocator_create(&create_info);

	setup_signal_handlers();

	test_t* test = test_create(allocator, (argc > 1) ? argv[1] : "");
	renderer_initialization_data_t data = test->renderer_initialize();
	AUTO driver = renderer_init(allocator, data.gpu_type, data.window_width, data.window_height, data.window_name, data.is_fullscreen, data.is_resizable);

	test->initialize(driver, test->user_data);

	time_handle_t tHandle = time_get_handle();

	while(renderer_is_running(driver))
	{
		if(kbhit())
		{
			if(getch() == 'd')
			{
				/* this should be pushed into another thread to not to halt the main thread! */
				memory_allocation_tree_dump(allocator, "runtime.dump");
			}
		}
		float deltaTime = time_get_delta_time(&tHandle);
		renderer_begin_frame(driver);
		test->render(driver, test->user_data);
		renderer_end_frame(driver);
		renderer_update(driver);
		test->update(driver, deltaTime, test->user_data);
	}

	/* REASON: Takes too much time for complex allocation trees! */
	// memory_allocation_tree_dump(allocator, "terminate.dump");
	test->terminate(driver, test->user_data);
	test_destroy(test);

	renderer_terminate(driver);
	memory_allocator_destroy(allocator);
	alloc_terminate();
	return 0;
}