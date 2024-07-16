#pragma once

#include <sge/event.h>

namespace SGE
{
	class Event
	{
	private:
		event_t* m_handle;

		Event(event_t* event) noexcept : m_handle(event) { }

		friend class RenderWindow;

	public:
		typedef event_subscription_create_info_t SubscriptionCreateInfo;
		void publish() noexcept { event_publish(m_handle); }
		void subscribe(void (*callback)(void* publisher, void* handlerData), void* handlerData = NULL) noexcept
		{
			subscribe(SubscriptionCreateInfo { .handler = EVENT_HANDLER(callback), .handler_data = handlerData });
		}
		void subscribe(const SubscriptionCreateInfo& subscriptionCreateInfo) noexcept 
		{ 
			event_subscribe(m_handle, const_cast<SubscriptionCreateInfo*>(&subscriptionCreateInfo));
		}

		template<typename T>
		static T ReinterpretPublisher(void* publisher);
	};
}