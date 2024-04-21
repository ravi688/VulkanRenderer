/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-43.case1.c is a part of VulkanRenderer

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


#include <renderer/tests/TID-43.case1.h>
#include <renderer/debug.h>

#include <renderer/memory_allocator.h>
#include <renderer/event.h>
#include <renderer/conio.h>


TEST_DATA(TID_43_CASE_1)
{
	memory_allocator_t* allocator;
	event_t* event;
	event_subscription_handle_t handle;
};

SETUP_TEST(TID_43_CASE_1);

static void image_recreate(void* publisher_data, void* handler_data)
{
	debug_log_info("Image recreate success!");
}

static void image_view_recreate(void* publisher_data, void* handler_data)
{
	debug_log_info("Image View recreate success!");
}

static void framebuffer_recreate(void* publisher_data, void* handler_data)
{
	debug_log_info("Framebuffer recreate success!");
}

static void gfx_pipeline_recreate(void* publisher_data, void* handler_data)
{
	debug_log_info("Graphics Pipeline recreate success!");
}

static void swapchain_recreate(void* publisher_data, void* handler_data)
{
	debug_log_info("Swapchain recreate success!");
}

TEST_ON_RENDERER_INITIALIZE(TID_43_CASE_1)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_43_CASE_1",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_43_CASE_1)
{
	memory_allocator_create_info_t create_info =
	{

	};
	this->allocator = memory_allocator_create(&create_info);

	this->event = event_create(this->allocator, this PARAM_IF_DEBUG("Test event"));

	/* 	[VULKAN_IMAGE_RECREATE_FINISH | VULKAN_IMAGE_VIEW_RECREATE_FINISH, NOTHING]
		[VULKAN_IMAGE_RECREATE_FINISH, NOTHING]
		[NOTHING, VULKAN_IMAGE_RECREATE_FINISH]
	 */

	event_subscription_create_info_t info4 =
	{
		.handler = gfx_pipeline_recreate,
		.handler_data = NULL,
		.wait_bits = SIGNAL_ALL_BIT,
		.signal = SIGNAL_NOTHING_BIT
	};

	event_subscribe(this->event, &info4);

	event_subscription_create_info_t info3 =
	{
		.handler = framebuffer_recreate,
		.handler_data = NULL,
		.wait_bits = SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT | SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT,
		.signal = SIGNAL_NOTHING_BIT
	};

	event_subscribe(this->event, &info3);

	event_subscription_create_info_t info2 =
	{
		.handler = image_view_recreate,
		.handler_data = NULL,
		.wait_bits = SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT,
		.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT
	};

	event_subscribe(this->event, &info2);

	event_subscription_create_info_t info =
	{
		.handler = image_recreate,
		.handler_data = NULL,
		.wait_bits = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT
	};

	this->handle = event_subscribe(this->event, &info);

	event_subscription_create_info_t info5 =
	{
		.handler = swapchain_recreate,
		.handler_data = NULL,
		.wait_bits = SIGNAL_NOTHING_BIT,
		.signal = SIGNAL_NOTHING_BIT
	};

	event_subscribe(this->event, &info5);

}

TEST_ON_TERMINATE(TID_43_CASE_1)
{
	event_destroy(this->event);
	event_release_resources(this->event);
	memory_allocator_destroy(this->allocator);
}

TEST_ON_RENDER(TID_43_CASE_1) { }

TEST_ON_UPDATE(TID_43_CASE_1)
{
	if(kbhit())
	{
		char ch = getch();
		if(ch == 'p')
			event_publish(this->event);
		else if(ch == 'd')
			event_unsubscribe(this->event, this->handle);
	}
}
