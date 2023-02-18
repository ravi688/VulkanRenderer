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
	SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH,
	SIGNAL_VULKAN_IMAGE_RECREATE_FINISH,
	SIGNAL_VULKAN_FRAMEBUFFER_RECREATE,
	SIGNAL_ALL
} signal_t;

typedef enum signal_bits_t
{
	SIGNAL_NOTHING_BIT = BIT32(SIGNAL_NOTHING),
	SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_IMAGE_VIEW_RECREATE_FINISH),
	SIGNAL_VULKAN_IMAGE_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_IMAGE_RECREATE_FINISH),
	SIGNAL_VULKAN_FRAMEBUFFER_RECREATE_FINISH_BIT = BIT32(SIGNAL_VULKAN_FRAMEBUFFER_RECREATE),
	SIGNAL_ALL_BIT = 0xFFFFFFFFULL
} signal_bits_t;

/* publisher_data: pointer to the data owned by the publisher of the event 
 * handler_data: pointer to the same data during the event subscription */
typedef void (*event_handler_t)(void* publisher_data, void* handler_data);

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
	signal_bits_t signal;
} event_subscription_create_info_t;

/* forward declaration */
typedef	struct memory_allocator_t memory_allocator_t;

typedef struct event_t
{
	memory_allocator_t* allocator;
	void* publisher_data;
	u32* signal_table;;
	u32* stage_signal_table;;
	BUFFER subscribers;
	BUFFER unsubscribed_handles;
	event_subscription_handle_t handle_counter;
} event_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
RENDERER_API event_t* event_new(memory_allocator_t* allocator);
RENDERER_API event_t* event_create(memory_allocator_t* allocator, void* publisher_data);
RENDERER_API void event_create_no_alloc(memory_allocator_t* allocator, void* publisher_data, event_t OUT event);
RENDERER_API void event_destroy(event_t* event);
RENDERER_API void event_release_resources(event_t* event);

/* calls all the event handlers subscribed to this event */
RENDERER_API void event_publish(event_t* event);
/* adds a subscription to the event subscription list */
RENDERER_API event_subscription_handle_t event_subscribe(event_t* event, event_subscription_create_info_t* create_info);
/* removes a subscription from the event subscription list */
RENDERER_API void event_unsubscribe(event_t* event, event_subscription_handle_t handle);

END_CPP_COMPATIBLE
