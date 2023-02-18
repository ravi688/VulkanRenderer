#include <renderer/event.h>
#include <renderer/memory_allocator.h>

typedef event_subscription_create_info_t invocation_data_t;

typedef struct subscription_t
{
	/* nothing but the event_subscription_create_info_t */
	invocation_data_t invocation_data;
	/* handle to be able to search this subscription into a list of subscribers */
	event_subscription_handle_t handle;
} subscription_t;

RENDERER_API event_t* event_new(memory_allocator_t* allocator)
{
	AUTO event = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_EVENT, event_t);
	memzero(event, event_t);
	event->allocator = allocator;
	return event;
}

RENDERER_API event_t* event_create(memory_allocator_t* allocator, void* publisher_data)
{
	AUTO event = event_new(allocator);
	event_create_no_alloc(allocator, publisher_data, event);
	return event;
}

RENDERER_API void event_create_no_alloc(memory_allocator_t* allocator, void* publisher_data, event_t OUT event)
{
	memzero(event, event_t);
	event->allocator = allocator;
	event->publisher_data = publisher_data;
	event->signal_table = memory_allocator_alloc_obj_array(event->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, SIGNAL_ALL + 1);
	event->stage_signal_table = memory_allocator_alloc_obj_array(event->allocator, MEMORY_ALLOCATION_TYPE_OBJ_U32_ARRAY, u32, SIGNAL_ALL + 1);
	event->subscribers = buf_new(subscription_t);
	event->unsubscribed_handles = buf_new(event_subscription_handle_t);
	for(u32 i = SIGNAL_NOTHING; i <= SIGNAL_ALL; i++)
		event->signal_table[i] = U32_MAX;
	event->signal_table[SIGNAL_NOTHING] = 0;
}

RENDERER_API void event_destroy(event_t* event)
{
	for(u32 i = SIGNAL_NOTHING; i <= SIGNAL_ALL; i++)
		event->signal_table[i] = U32_MAX;
	event->publisher_data = NULL;
	buf_clear(&event->subscribers, NULL);
	buf_clear(&event->unsubscribed_handles, NULL);
}

RENDERER_API void event_release_resources(event_t* event)
{
	memory_allocator_dealloc(event->allocator, event->signal_table);
	memory_allocator_dealloc(event->allocator, event->stage_signal_table);
	buf_free(&event->subscribers);
	buf_free(&event->unsubscribed_handles);
}

static bool signal_waits_done(event_t* event, signal_bits_t wait_bits)
{
	for(u32 i = SIGNAL_NOTHING; i <= SIGNAL_ALL; i++)
		if((wait_bits & BIT32(i)) && (event->stage_signal_table[i] != U32_MAX) && (event->stage_signal_table[i] > 0))
			return false;
	return true;
}

static u32 get_bit_pos(u32 bit)
{
	if((bit & SIGNAL_ALL_BIT) == SIGNAL_ALL_BIT)
		return SIGNAL_ALL;

	for(u32 i = SIGNAL_NOTHING; i < SIGNAL_ALL; i++)
		if((bit & BIT32(i)) == BIT32(i))
			return i;
	return U32_MAX;
}

RENDERER_API void event_publish(event_t* event)
{
	memcopyv(event->stage_signal_table, event->signal_table, u32, SIGNAL_ALL + 1);

	u32 count = buf_get_element_count(&event->subscribers);
	for(u32 i = 0; i < count; i++)
	{
		AUTO subscription = buf_get_ptr_at_typeof(&event->subscribers, subscription_t, i);
		if(signal_waits_done(event, subscription->invocation_data.wait_for))
		{
			subscription->invocation_data.handler(event->publisher_data, subscription->invocation_data.handler_data);
			u32 bit_pos = get_bit_pos(subscription->invocation_data.signal);
			if(event->stage_signal_table[bit_pos] > 0)
				event->stage_signal_table[bit_pos]--;
		}
	}
}

RENDERER_API event_subscription_handle_t event_subscribe(event_t* event, event_subscription_create_info_t* create_info)
{
	subscription_t subscription;
	
	/* copy the invocation data */
	memcopy(&subscription.invocation_data, create_info, invocation_data_t);

	/* generate a unique handle */
	event_subscription_handle_t handle = EVENT_SUBSCRIPTION_HANDLE_INVALID;

	/* if there is exist unclaimed handle then just use it */
	if(buf_get_element_count(&event->unsubscribed_handles) > 0)
		buf_pop(&event->unsubscribed_handles, &handle);
	/* otherwise create a new handle by incrementing the counter */
	else
	{
		event->handle_counter++;
		handle = event->handle_counter;
	}
	subscription.handle = handle;

	u32 count = buf_get_element_count(&event->subscribers);
	signal_bits_t raised_signals = SIGNAL_NOTHING_BIT;
	u32 index = 0;
	for(u32 i = 0; i < count; i++)
	{
		AUTO _subscription = buf_get_ptr_at_typeof(&event->subscribers, subscription_t, i);
		raised_signals |= _subscription->invocation_data.signal;
		if(((raised_signals & subscription.invocation_data.wait_bits) == subscription.invocation_data.wait_bits)
			|| ((_subscription->invocation_data.wait_bits & subscription.invocation_data.signal) == subscription.invocation_data.signal))
			break;
		index++;
	}
	buf_insert_at(&event->subscribers, index, &subscription);

	u32 bit_pos = get_bit_pos(subscription.invocation_data.signal);
	if(bit_pos != SIGNAL_NOTHING)
	{
		/* increment the number of handlers that will raise the signal */
		if(event->signal_table[bit_pos] == U32_MAX)
			event->signal_table[bit_pos] = 1;
		else
			event->signal_table[bit_pos]++;
	}

	return handle;
}

static bool handle_comparer(void* handle, void* subscription)
{
	return CAST_TO(subscription_t*, subscription)->handle == DREF_TO(event_subscription_handle_t, handle);
}

RENDERER_API void event_unsubscribe(event_t* event, event_subscription_handle_t handle)
{
	/* remove the subscriber, and if success then add the just unclaimed handle to the list of unsubscribed handles to be later used */

	buf_ucount_t index = buf_find_index_of(&event->subscribers, &handle, handle_comparer);
	if(index != BUF_INVALID_INDEX)
	{
		subscription_t subscription;
		bool result = buf_remove_at(&event->subscribers, index, &subscription);
		_debug_assert__(result == true);
		buf_push(&event->unsubscribed_handles, &handle);
		u32 bit_pos = get_bit_pos(subscription.invocation_data.signal);
		_debug_assert__(event->signal_table[bit_pos] != U32_MAX);
		if((bit_pos != SIGNAL_NOTHING) && (event->signal_table[bit_pos] > 0))
			event->signal_table[bit_pos]--;
	}
	/* otherwise it is an error */
	else
		debug_log_error("event subscription handle %llu has already been unsubscribed or it is a corrupted handle", handle);
}
