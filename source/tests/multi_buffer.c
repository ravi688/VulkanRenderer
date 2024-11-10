/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: multi_buffer.c is a part of VulkanRenderer

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


/*
	test id: MULTI_BUFFER
	Tests render to depth texture and present that to screen.
	while depth texture's dimensions is rectangular rather than square
	and different from the swapchain's dimensions
 */


#include <sge/tests/multi_buffer.h>

#include <sge/multi_buffer.h>

TEST_DATA(MULTI_BUFFER)
{
	u8 _;
};

SETUP_TEST(MULTI_BUFFER);

TEST_ON_SGE_INITIALIZE(MULTI_BUFFER)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "MULTI_BUFFER",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

static void test_assert(bool condition)
{
	com_assert(COM_DESCRIPTION(condition), "Test Failed");
}

static void run_test(u32 capacity, u32 sub_capacity)
{
	buffer_t buf = buf_create(sizeof(int), 3, 0);
	test_assert(buf_get_capacity(&buf) == 3);
	buf_ensure_capacity(&buf, 10);
	test_assert(buf_get_capacity(&buf) == 12);
	buf_resize(&buf, 23);
	test_assert(buf_get_capacity(&buf) == 23);
	buf_push_pseudo(&buf, 23);
	test_assert(buf_get_capacity(&buf) == 23);
	test_assert(buf_get_element_count(&buf) == 23);
	buf_insert_pseudo(&buf, 13, 10);
	test_assert(buf_get_element_count(&buf) == 33);
	test_assert(buf_get_capacity(&buf) == 46);
	buf_remove_pseudo(&buf, 13, 10);
	test_assert(buf_get_capacity(&buf) == 46);
	test_assert(buf_get_element_count(&buf) == 23);
	buf_insert_pseudo(&buf, 13, 10);
	test_assert(buf_get_element_count(&buf) == 33);
	test_assert(buf_get_capacity(&buf) == 46);
	buf_free(&buf);

	multi_buffer_t mbuf;
	multi_buffer_create(sizeof(int), capacity, &mbuf);

	test_assert(multi_buffer_get_count(&mbuf) == 0);
	test_assert(multi_buffer_get_capacity(&mbuf) == capacity);
	test_assert(multi_buffer_get_combined_sub_buffers_count(&mbuf) == 0);
	test_assert(multi_buffer_get_sub_buffer_count(&mbuf) == 0);

	sub_buffer_handle_t handles[4];
	for(u32 i = 1; i <= 4; ++i)
	{
		for(u32 j = 0; j < i; ++j)
		{
			u32 _sub_capacity = (j%2) ? (sub_capacity * j) : (sub_capacity + j);
			AUTO handle = multi_buffer_sub_buffer_create(&mbuf, _sub_capacity);
			debug_log_info("created sub buffer handle: %llu", handle);
			handles[j] = handle;

			test_assert(multi_buffer_sub_buffer_get_count(&mbuf, handle) == 0);
			test_assert(multi_buffer_sub_buffer_get_capacity(&mbuf, handle) >= _sub_capacity);

			multi_buffer_sub_buffer_clear(&mbuf, handle);
			test_assert(multi_buffer_sub_buffer_get_count(&mbuf, handle) == 0);

			int value = 0;
			for(u32 k = 0; k < j; ++k, ++value)
			{
				multi_buffer_sub_buffer_push(&mbuf, handle, &value);
				test_assert(multi_buffer_sub_buffer_get_count(&mbuf, handle) == (value + 1));
			}
			value = 0;
			for(u32 k = 0; k < j; ++k, ++value)
			{
				int out_value;
				multi_buffer_sub_buffer_get_at(&mbuf, handle, k, &out_value);
				test_assert(out_value == value);
			}
			value = 0;
			for(u32 k = 0; k < j; ++k, value += 2)
				multi_buffer_sub_buffer_set_at(&mbuf, handle, k, &value);
			value = 0;
			for(u32 k = 0; k < j; ++k, value += 2)
			{
				int out_value;
				multi_buffer_sub_buffer_get_at(&mbuf, handle, k, &out_value);
				test_assert(out_value == value);
			}
		}
		for(u32 j = 0; j < u32_min(i, 10); ++j)
			multi_buffer_sub_buffer_destroy(&mbuf, handles[j]);
		for(u32 j = 10; j < i; ++j)
		{
			AUTO handle = handles[j];
			u32 count = multi_buffer_sub_buffer_get_count(&mbuf, handle);
			int value = j;
			multi_buffer_sub_buffer_push(&mbuf, handle, &value);
			test_assert(multi_buffer_sub_buffer_get_count(&mbuf, handle) == (count + 1));
			multi_buffer_sub_buffer_destroy(&mbuf, handle);
		}
		test_assert(multi_buffer_get_sub_buffer_count(&mbuf) == 0);
		debug_log_info("Passed iteration: %u", i);
	}

	multi_buffer_clear(&mbuf);
	test_assert(multi_buffer_get_sub_buffer_count(&mbuf) == 0);
	test_assert(multi_buffer_get_count(&mbuf) == 0);

	multi_buffer_free(&mbuf);
	debug_log_info("Passed");
}

TEST_ON_INITIALIZE(MULTI_BUFFER)
{
	run_test(0, 0);
	run_test(1, 0);
	run_test(1, 4);
	run_test(4, 5);
	debug_log_info("All runs passed the test");
}

TEST_ON_TERMINATE(MULTI_BUFFER)
{

}

TEST_ON_UPDATE(MULTI_BUFFER)
{

}

TEST_ON_RENDER(MULTI_BUFFER)
{

}

