/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: event.c is a part of VulkanRenderer

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

#include <renderer/event.h>
#include <renderer/memory_allocator.h>
#include <renderer/string_builder.h>
#ifdef GLOBAL_DEBUG
#	include <string.h>
#endif /* GLOBAL_DEBUG */

typedef event_subscription_create_info_t invocation_data_t;

/* structure to store source level debug information for the subscription */
typedef struct debug_info_t
{
	/* line number in the file */
	u32 line;
	/* name of the function in the file */
	const char* function_str;
	/* name of the file */
	const char* file_str;
} debug_info_t;

/* structure to store all the data related to subscription */
typedef struct subscription_t
{
	/* nothing but the event_subscription_create_info_t */
	invocation_data_t invocation_data;
	/* handle to be able to search this subscription into a list of subscribers */
	event_subscription_handle_t handle;
	/* source level debug info */
	debug_info_t debug_info;
	/* true if this subscription is active, otherwise false */
	bool is_active;
} subscription_t;

SGE_API event_t* event_new(memory_allocator_t* allocator)
{
	AUTO event = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_EVENT, event_t);
	memzero(event, event_t);
	OBJECT_INIT(event, OBJECT_TYPE_EVENT, OBJECT_NATIONALITY_INTERNAL);
	event->allocator = allocator;
	return event;
}

SGE_API event_t* event_create(memory_allocator_t* allocator, void* publisher_data PARAM_IF_DEBUG(const char* name))
{
	AUTO event = event_new(allocator);
	event_create_no_alloc(allocator, publisher_data PARAM_IF_DEBUG(name), event);
	return event;
}

SGE_API void event_create_no_alloc(memory_allocator_t* allocator, void* publisher_data PARAM_IF_DEBUG(const char* name), event_t OUT event)
{
	OBJECT_MEMZERO(event, event_t);
	event->allocator = allocator;
	event->string_builder = string_builder_create(allocator, 512);
	IF_DEBUG(strcpy(event->name, name));
	event->publisher_data = publisher_data;
	event->signal_table = memory_allocator_alloc_obj_array(event->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, SIGNAL_COUNT);
	event->stage_signal_table = memory_allocator_alloc_obj_array(event->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, SIGNAL_COUNT);
	event->subscribers = memory_allocator_buf_new(allocator, subscription_t);
	event->stage_subscribers = memory_allocator_buf_new(allocator, u32);
	event->stage_subscribers_swap = memory_allocator_buf_new(allocator, u32);
	event->unsubscribed_handles = memory_allocator_buf_new(allocator, event_subscription_handle_t);
	event->is_dump_only = false;

	/* initially all the signals would be down */
	for(u32 i = SIGNAL_NOTHING; i < SIGNAL_MAX; i++)
		event->signal_table[i] = U32_MAX;
	/* but the nothing signal would always be up */
	event->signal_table[SIGNAL_NOTHING] = 0;
}

SGE_API void event_create_no_alloc_ext(memory_allocator_t* allocator, void* publisher_data PARAM_IF_DEBUG(const char* name), event_t OUT event)
{
	OBJECT_INIT(event, OBJECT_TYPE_EVENT, OBJECT_NATIONALITY_EXTERNAL);
	event_create_no_alloc(allocator, publisher_data PARAM_IF_DEBUG(name), event);
}

SGE_API void event_destroy(event_t* event)
{
	for(u32 i = SIGNAL_NOTHING; i < SIGNAL_MAX; i++)
		event->signal_table[i] = U32_MAX;
	event->publisher_data = NULL;
	buf_clear(&event->stage_subscribers, NULL);
	buf_clear(&event->stage_subscribers_swap, NULL);
	buf_clear(&event->subscribers, NULL);
	buf_clear(&event->unsubscribed_handles, NULL);
}

SGE_API void event_release_resources(event_t* event)
{
	string_builder_destroy(event->string_builder);
	memory_allocator_dealloc(event->allocator, event->signal_table);
	memory_allocator_dealloc(event->allocator, event->stage_signal_table);
	buf_free(&event->stage_subscribers);
	buf_free(&event->stage_subscribers_swap);
	buf_free(&event->subscribers);
	buf_free(&event->unsubscribed_handles);
	if(OBJECT_IS_INTERNAL(event))
		memory_allocator_dealloc(event->allocator, event);
}

/* checks if all the wait signals have been raised or not, if yes returns true, otherwise false */
static bool signal_waits_done(event_t* event, signal_bits_t wait_bits)
{
	for(u32 i = SIGNAL_NOTHING; i < SIGNAL_MAX; i++)
		if((wait_bits & BIT32(i)) && (event->stage_signal_table[i] != U32_MAX) && (event->stage_signal_table[i] > 0))
			return false;
	return true;
}

#ifdef GLOBAL_DEBUG
# 	define GET_HANDLER(subscription_info) (&(subscription_info))->handler.handler
#else
#	define GET_HANDLER(subscription_info) (&(subscription_info))->handler
#endif /* GLOBAL_DEBUG */

/* dumps the subscription data in the string builder */
static void subscription_dump(memory_allocator_t* allocator, subscription_t* subscription, string_builder_t* builder);

SGE_API void event_publish(event_t* event)
{
	DBG_EVENT_PUBLISH( debug_log_info("***--event-publish-begin-%s--***", event->name) );

	/* create a copy of the signal table because it will modified during the invocations
	 * also, signal table is used to determine how many subscribers/invocations can raise a particular signal */
	memcopyv(event->stage_signal_table, event->signal_table, u32, SIGNAL_COUNT);

	/* copy sequential indicies into the stage subscribers list */
	buf_clear(&event->stage_subscribers, NULL);
	u32 count = buf_get_element_count(&event->subscribers);
	for(u32 i = 0; i < count; i++)
		buf_push(&event->stage_subscribers, &i);

	/* we will be mainting two buffers and swap them whenever the number of invocations yet to be invoked is greater than zero */
	BUFFER* swap_buffer = &event->stage_subscribers_swap;
	BUFFER* stage_buffer = &event->stage_subscribers;

	/* debuggin purpose only */
	NOT_DBG_EVENT_PUBLISH( if(event->is_dump_only) )
	{
		string_builder_clear(event->string_builder);
		string_builder_append(event->string_builder, "\n");
	}

	do
	{
		/* clear the swap buffer as this will be used to collect waiting handlers */
		buf_clear(swap_buffer, NULL);

		/* iterate over the yet to be invoked handlers */
		for(u32 i = 0; i < count; i++)
		{
			/* get the index of any one subscription */
			u32 index;
			buf_get_at(stage_buffer, i, &index);

			/* get pointer to the subscription with the help of the index we just got */
			AUTO subscription = buf_get_ptr_at_typeof(&event->subscribers, subscription_t, index);

			/* if all the signals have been raised to which this subscription/invocation was waiting upon */
			if(signal_waits_done(event, subscription->invocation_data.wait_for))
			{
				AUTO handler = GET_HANDLER(subscription->invocation_data);

				/* if is_dump_only is true then just dump the invocation order but don't make a call to the handlers */
				if(!event->is_dump_only && (handler != NULL) && subscription->is_active)
					handler(event->publisher_data, subscription->invocation_data.handler_data);
				NOT_DBG_EVENT_PUBLISH( else if(event->is_dump_only) )
					subscription_dump(event->allocator, subscription, event->string_builder);

				/* raise the signals which are requested to be raised by this invocation
				 * NOTE: no handler can raise SIGNAL_NOTHING_BIT as it is already raised */
				for(u32 i = SIGNAL_NOTHING + 1; i < SIGNAL_MAX; i++)
				{
					if((subscription->invocation_data.signal_bits & BIT32(i)) == BIT32(i))
					{
						_debug_assert__(event->stage_signal_table[i] != U32_MAX);
						_debug_assert__(event->stage_signal_table[i] > 0);
						event->stage_signal_table[i]--;
					}
				}
			}
			/* otherwise put this into waiting buffer, because it is still waiting upon some other signals to be raised by some other invocations */
			else
				buf_push(swap_buffer, &index);
		}

		/* swap the buffers */
		BUFFER* temp = swap_buffer;
		swap_buffer = stage_buffer;
		stage_buffer = temp;

		/* if there are handlers yet to be invoked, then iterate over again with leftover handlers */
	} while((count = buf_get_element_count(stage_buffer)) > 0);

	/* debugging purpose only */
	NOT_DBG_EVENT_PUBLISH( if(event->is_dump_only) )
	{
		string_builder_append_null(event->string_builder);
		debug_log_info(string_builder_get_str(event->string_builder));
	}
	DBG_EVENT_PUBLISH( debug_log_info("***--event-publish-end--***") );
}

SGE_API event_subscription_handle_t __event_subscribe(event_t* event, event_subscription_create_info_t* create_info, u32 line, const char* const function, const char* const file)
{
	subscription_t subscription = { .debug_info = { line, function, file }, .is_active = true };

	/* copy the invocation data */
	memcopy(&subscription.invocation_data, create_info, invocation_data_t);

	/* generate a unique handle */
	event_subscription_handle_t handle = EVENT_SUBSCRIPTION_HANDLE_INVALID;

	/* if there exist unclaimed handle then just use it */
	if(buf_get_element_count(&event->unsubscribed_handles) > 0)
		buf_pop(&event->unsubscribed_handles, &handle);
	/* otherwise create a new handle by incrementing the counter */
	else
	{
		handle = event->handle_counter;
		event->handle_counter++;
	}
	subscription.handle = handle;

	/* if the subscribed handler doesn't wait for SIGNAL_ALL_BIT then it must raise SIGNAL_ALL_BIT
	 * NOTE: all hander must raise SIGNAL_ALL_BIT except the ones which are waiting upon SIGNAL_ALL_BIT */
	if((subscription.invocation_data.wait_bits & SIGNAL_ALL_BIT) != SIGNAL_ALL_BIT)
		subscription.invocation_data.signal_bits |= SIGNAL_ALL_BIT;
	/* if the subscribed handler waits for SIGNAL_ALL_BIT and raises SIGNAL_ALL_BIT as well */
	else if((subscription.invocation_data.signal_bits & SIGNAL_ALL_BIT) == SIGNAL_ALL_BIT)
		subscription.invocation_data.signal_bits = ~SIGNAL_ALL_BIT;

	buf_push(&event->subscribers, &subscription);

	for(u32 i = SIGNAL_NOTHING + 1; i < SIGNAL_MAX; i++)
	{
		if((subscription.invocation_data.signal_bits & BIT32(i)) == BIT32(i))
		{
			/* increment the number of handlers that will raise the signal */
			if(event->signal_table[i] == U32_MAX)
				event->signal_table[i] = 1;
			else
				event->signal_table[i]++;
		}
	}

	return handle;
}

static bool handle_comparer(void* handle, void* subscription)
{
	return CAST_TO(subscription_t*, subscription)->handle == DREF_TO(event_subscription_handle_t, handle);
}

SGE_API void event_unsubscribe(event_t* event, event_subscription_handle_t handle)
{
	/* remove the subscriber, and if success then add the just unclaimed handle to the list of unsubscribed handles to be later used */
	buf_ucount_t index = buf_find_index_of(&event->subscribers, &handle, handle_comparer);
	if(index != BUF_INVALID_INDEX)
	{
		subscription_t subscription;
		CAN_BE_UNUSED_VARIABLE bool result = buf_remove_at(&event->subscribers, index, &subscription);
		_debug_assert__(result == true);
		buf_push(&event->unsubscribed_handles, &handle);
		for(u32 i = SIGNAL_NOTHING + 1; i < SIGNAL_MAX; i++)
		{
			if((subscription.invocation_data.signal_bits & BIT32(i)) == BIT32(i))
			{
				_debug_assert__(event->signal_table[i] != U32_MAX);

				if(event->signal_table[i] > 0)
					event->signal_table[i]--;
			}
		}
	}
	/* otherwise it is an error */
	else
		debug_log_error("event subscription handle %llu has already been unsubscribed or it is a corrupted handle", handle);
}


static const char* signal_to_string(signal_bits_t bit)
{
	switch(bit)
	{
		case SIGNAL_NOTHING_BIT: return "SIGNAL_NOTHING_BIT";
		case SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT: return "SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT";
		case SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT: return "SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT";
		case SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT: return "SIGNAL_VULKAN_IMAGE_VIEW_TRANSFER_FINISH_BIT";
		case SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT: return "SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT";
		case SIGNAL_ALL_BIT: return "SIGNAL_ALL_BIT";
		default: return "<INVALID SIGNAL BIT>";
	}
}

CAN_BE_UNUSED_FUNCTION static string_builder_t* signal_bits_to_string(memory_allocator_t* allocator, signal_bits_t bits)
{
	string_builder_t* builder = string_builder_create(allocator, 128);

	for(u32 i = SIGNAL_NOTHING; i < SIGNAL_MAX; i++)
	{
		if((bits & BIT32(i)) == BIT32(i))
		{
			string_builder_append(builder, "%s", signal_to_string(bits & BIT32(i)));
			if((bits >> (i + 1)) > 0)
				string_builder_append(builder, " | ");
		}
	}
	return builder;
}

static void subscription_dump(memory_allocator_t* allocator, subscription_t* subscription, string_builder_t* builder)
{
#ifdef GLOBAL_DEBUG
	string_builder_append(builder, "{\n");
	string_builder_increment_indentation(builder);
		string_builder_append(builder, ".location = { %lu, %s, %s }\n", subscription->debug_info.line, subscription->debug_info.function_str, subscription->debug_info.file_str);
		string_builder_append(builder, ".handler = %s (%p)\n", subscription->invocation_data.handler.handler_str, subscription->invocation_data.handler.handler);
		string_builder_append(builder, ".handler_data = %p\n", subscription->invocation_data.handler_data);
		string_builder_append(builder, ".wait_bits = ");
		string_builder_stitch_builder_destroy(builder, signal_bits_to_string(allocator, subscription->invocation_data.wait_bits));
		string_builder_stitch_newline(builder);
		string_builder_append(builder, ".signal_bits = ");
		string_builder_stitch_builder_destroy(builder, signal_bits_to_string(allocator, subscription->invocation_data.signal_bits));
		string_builder_stitch_newline(builder);
		string_builder_append(builder, ".handle = %llu\n", subscription->handle);
	string_builder_decrement_indentation(builder);
	string_builder_append(builder, "}\n");
#endif /* GLOBAL_DEBUG */
}

SGE_API void event_dump(event_t* event)
{
#ifdef GLOBAL_DEBUG
	string_builder_t* builder = string_builder_create(event->allocator, 512);

	u32 count = buf_get_element_count(&event->subscribers);
	if(count > 0)
		string_builder_append(builder, "\n");
	for(u32 i = 0; i < count; i++)
		subscription_dump(event->allocator, buf_get_ptr_at_typeof(&event->subscribers, subscription_t, i), builder);

	string_builder_append_null(builder);
	debug_log_info(string_builder_get_str(builder));
	string_builder_destroy(builder);
#endif /* GLOBAL_DEBUG */
}

SGE_API void event_set_subscription_active(event_t* event, event_subscription_handle_t handle, bool is_active)
{
	/* remove the subscriber, and if success then add the just unclaimed handle to the list of unsubscribed handles to be later used */
	buf_ucount_t index = buf_find_index_of(&event->subscribers, &handle, handle_comparer);
	if(index != BUF_INVALID_INDEX)
	{
		AUTO subscription = CAST_TO(subscription_t*,  buf_get_ptr_at(&event->subscribers, index));
		subscription->is_active = is_active;
	}
	/* otherwise it is an error */
	else
		debug_log_error("no subscription with event subscription handle %llu has been found, is that a corrupt handle?", handle);
}
