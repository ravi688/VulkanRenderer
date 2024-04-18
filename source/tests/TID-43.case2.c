/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: TID-43.case2.c is a part of VulkanRenderer

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


#include <renderer/tests/TID-43.case2.h>
#include <renderer/debug.h>

#include <renderer/memory_allocator.h>
#include <renderer/event.h>

/* TODO: Write a cross platform input library { Keyboard, and Mouse } 
 * For now, we can just bypass kbhit() and getch() function calls. */
#ifdef PLATFORM_WINDOWS
#	include <conio.h>
#elif defined(PLATFORM_LINUX)
#	define kbhit() false
#	define getch() 0
#endif

TEST_DATA(TID_43_CASE_2)
{
	memory_allocator_t* allocator;
	event_t* event;
	event_subscription_handle_t handle;
};

SETUP_TEST(TID_43_CASE_2);

static void recreate_allocated_attachments(void* publisher_data, void* handler_data)
{
	debug_log_info("Allocated attachments recreate success!");
}

static void recreate_framebuffers(void* publisher_data, void* handler_data)
{
	debug_log_info("Framebuffer recreate success!");
}

static void recreate_graphics_pipelines(void* publisher_data, void* handler_data)
{
	debug_log_info("Graphics Pipelines recreate success!");
}

static void recreate_swapchain(void* publisher_data, void* handler_data)
{
	debug_log_info("Swapchain recreate success!");
}

static void recreate_projection(void* publisher_data, void* handler_data)
{
	debug_log_info("Projection recreate success");
}

static void recreate_screen_projection(void* publisher_data, void* handler_data)
{
	debug_log_info("Screen projection recreate success");
}

static void refresh_default_render_pass(void* publisher_data, void* handler_data)
{
	debug_log_info("Default render pass refresh success");
}

static void refresh_render_passes(void* publisher_data, void* handler_data)
{
	debug_log_info("Render passes refresh success");
}

TEST_ON_RENDERER_INITIALIZE(TID_32_CASE_2)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "TID_32_CASE_2",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

TEST_ON_INITIALIZE(TID_43_CASE_2)
{
	memory_allocator_create_info_t create_info =
	{

	};
	this->allocator = memory_allocator_create(&create_info);

	this->event = event_create(this->allocator, this PARAM_IF_DEBUG("Test Event"));

	/* 	[VULKAN_IMAGE_RECREATE_FINISH | VULKAN_IMAGE_VIEW_RECREATE_FINISH, NOTHING]
		[VULKAN_IMAGE_RECREATE_FINISH, NOTHING]
		[NOTHING, VULKAN_IMAGE_RECREATE_FINISH]
	 */

	event_subscription_create_info_t subscription = { };

	subscription.handler = EVENT_HANDLER(recreate_swapchain);
	subscription.wait_for = SIGNAL_NOTHING_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(NULL);
	subscription.wait_for = SIGNAL_ALL_BIT;
	subscription.signal = SIGNAL_NOTHING_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(recreate_allocated_attachments);
	subscription.wait_for = SIGNAL_NOTHING_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	event_subscribe(this->event, &subscription);
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(recreate_projection);
	subscription.wait_for = SIGNAL_NOTHING_BIT;
	subscription.signal = SIGNAL_NOTHING_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(recreate_screen_projection);
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(recreate_graphics_pipelines);
	subscription.wait_for = SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT;
	subscription.signal = SIGNAL_NOTHING_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(refresh_render_passes);
	subscription.wait_for = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(recreate_framebuffers);
	subscription.wait_for = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT | SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(refresh_default_render_pass);
	subscription.wait_for = SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT;
	subscription.signal = SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT;
	event_subscribe(this->event, &subscription);

	subscription.handler = EVENT_HANDLER(NULL);
	subscription.wait_for = SIGNAL_ALL_BIT;
	subscription.signal = SIGNAL_NOTHING_BIT;
	event_subscribe(this->event, &subscription);

	this->event->is_dump_only = true;

	event_dump(this->event);;
}

TEST_ON_TERMINATE(TID_43_CASE_2)
{
	event_destroy(this->event);
	event_release_resources(this->event);
	memory_allocator_destroy(this->allocator);
}

TEST_ON_RENDER(TID_43_CASE_2) { }

TEST_ON_UPDATE(TID_43_CASE_2)
{
	if(kbhit())
	{
		char ch = getch();
		if(ch == 'p')
			event_publish(this->event);
		else if(ch == 'd')
			event_unsubscribe(this->event, this->handle);
		else if(ch == 'e')
			event_dump(this->event);
	}
}
