/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects
	such as toolchains written by the same author) is a software to render
	2D & 3D geometries by writing C/C++ code and shaders.

	File: event.h is a part of VulkanRenderer

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

#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>

/* upon calling event_publish() function, the event iterates through all the handlers subscribed
 * and calls one-by-one.
 * But there might be some handlers which depend on the other handlers within the same iteration (publish).
 * To solve this issue, we can subdivide the iteration with signals, and the handlers may have an option
 * to wait on other handlers to finish their execution and raise their own signals to let other dependent handlers
 * to execute further. */
typedef enum signal_t
{
	SIGNAL_NOTHING = 0,
	SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH,
	SIGNAL_VULKAN_IMAGE_RECREATE_FINISH,
	SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH,
	SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH,
	SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH,
	SIGNAL_ALL,

	SIGNAL_MAX,
	SIGNAL_COUNT = SIGNAL_MAX
} signal_t;

typedef enum signal_bits_t
{
	SIGNAL_NOTHING_BIT = BIT32(SIGNAL_NOTHING),
	SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_CAMERA_RENDER_AREA_RECALCULATE_FINISH),
	SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_IMAGE_RECREATE_FINISH),
	SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH),
	SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT = BIT32(SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH),
	SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH),
	SIGNAL_ALL_BIT = BIT32(SIGNAL_ALL)
} signal_bits_t;

/* publisher_data: pointer to the data owned by the publisher of the event
 * handler_data: pointer to the same data during the event subscription */
typedef void (*__event_handler_t)(void* publisher_data, void* handler_data);

#ifdef GLOBAL_DEBUG
	typedef struct event_handler_debug_t
	{
		__event_handler_t handler;
		const char* handler_str;
	} event_handler_debug_t;
	typedef event_handler_debug_t event_handler_t;
# 	define EVENT_HANDLER(handler) (event_handler_debug_t) { handler, #handler }
#else
	typedef __event_handler_t event_handler_t;
# 	define EVENT_HANDLER(handler) CAST_TO(event_handler_t, handler)
#endif /* GLOBAL_DEBUG */

/* unique handle to each subscription */
typedef u32 event_subscription_handle_t;
#define EVENT_SUBSCRIPTION_HANDLE_INVALID U32_MAX

/* structure storing the details of subscriber */
typedef struct event_subscription_create_info_t
{
	/* pointer to the function to be called upon publish of the event */
	event_handler_t handler;
	/* pointer to the generic data to be passed on to the event handler as the second argument 'handler_data' */
	void* handler_data;
	/* signals to wait for before calling the event handler */
	union
	{
		signal_bits_t wait_for;
		signal_bits_t wait_bits;
	};
	/* signal to raise after calling the event handler */
	union
	{
		signal_bits_t signal;
		signal_bits_t signal_bits;
	};
} event_subscription_create_info_t;

/* forward declaration */
typedef	struct memory_allocator_t memory_allocator_t;
typedef struct string_builder_t string_builder_t;

typedef struct event_t
{
	/* pointer to the memory allocator */
	memory_allocator_t* allocator;
	/* pointer to the string builder (created internally) */
	string_builder_t* string_builder;
	/* name of the event_t */
	IF_DEBUG( char name[256]; )
	/* pointer to the data of the publisher (which calls event_publish()),
	 * passed as the first parameter in each handler invocation */
	void* publisher_data;
	/* pointer to the signal table (list of u32 of size SIGNAL_ALL), (created internally) */
	u32* signal_table;;
	/* pointer to the temporary signal table (list of u32 of size SIGNAL_ALL), (created internally)
	 * it is used during the call to event_publish() to execute the invocations in ordered manner based
	 * on the waiting and signalling signals. */
	u32* stage_signal_table;
	/* list of subscriptions (created internally) */
	BUFFER subscribers;
	/* list of temprary indicies (u32), (created internally)
	 * it is used during the call to event_publish() to execute the invocations in ordered manner based
	 * on the waiting and signalling signals. */
	BUFFER stage_subscribers;
	/* list of temprary indicies (u32), (created internally)
	 * it is used during the call to event_publish() to execute the invocations in ordered manner based
	 * on the waiting and signalling signals. */
	BUFFER stage_subscribers_swap;
	/* list of event_subscription_handle_t(s) which have been unclaimed/released by the unsubscribed handlers (created internally).*/
	BUFFER unsubscribed_handles;
	/* internall counter to assign unique handlers to each subscriptions (created internally) */
	event_subscription_handle_t handle_counter;
	/* set this true if you just want to see the execution order of the handlers but not the call */
	bool is_dump_only;
} event_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API event_t* event_new(memory_allocator_t* allocator);
RENDERER_API event_t* event_create(memory_allocator_t* allocator, void* publisher_data PARAM_IF_DEBUG(const char* name));
RENDERER_API void event_create_no_alloc(memory_allocator_t* allocator, void* publisher_data PARAM_IF_DEBUG(const char* name), event_t OUT event);
RENDERER_API void event_destroy(event_t* event);
RENDERER_API void event_release_resources(event_t* event);

/* calls all the event handlers subscribed to this event */
RENDERER_API void event_publish(event_t* event);
/* adds a subscription to the event subscription list */
#define event_subscribe(event, create_info) __event_subscribe(event, create_info, __LINE__, __FUNCTION__, __FILE__)
RENDERER_API event_subscription_handle_t __event_subscribe(event_t* event, event_subscription_create_info_t* create_info, u32 line, const char* const function, const char* const file);
/* removes a subscription from the event subscription list */
RENDERER_API void event_unsubscribe(event_t* event, event_subscription_handle_t handle);
/* dumps the subscribers to this event onto stdout */
RENDERER_API void event_dump(event_t* event);
/* sets a subscription identified by 'handle' as active or inactive
 * if a subscrpition becomes inactive then it's handler will not be executed but it would still be considered as executed
 * meaning if some subscriptions were waiting on that subscription to be executed, all those subscriptoins would be executed after that. */
RENDERER_API void event_set_subscription_active(event_t* event, event_subscription_handle_t handle, bool is_active);

END_CPP_COMPATIBLE
