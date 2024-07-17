#pragma once

#include <sge/event.h>

namespace SGE
{
	class Event
	{
	public:
		typedef event_subscription_handle_t SubscriptionHandle;
	private:
		event_t* m_handle;

		Event(event_t* event) noexcept : m_handle(event) { }

		friend class RenderWindow;

	public:
		typedef event_subscription_create_info_t SubscriptionCreateInfo;
		void publish() noexcept { event_publish(m_handle); }
		SubscriptionHandle subscribe(void (*handler)(void* publisher, void* handlerData), void* handlerData = NULL) noexcept
		{
			return subscribe(SubscriptionCreateInfo { .handler = EVENT_HANDLER(handler), .handler_data = handlerData });
		}
		SubscriptionHandle subscribe(const SubscriptionCreateInfo& subscriptionCreateInfo) noexcept 
		{ 
			return event_subscribe(m_handle, const_cast<SubscriptionCreateInfo*>(&subscriptionCreateInfo));
		}
		void unsubscribe(SubscriptionHandle handle) noexcept
		{
			event_unsubscribe(m_handle, handle);
		}

		template<typename T>
		static T ReinterpretPublisher(void* publisher);
	};
}