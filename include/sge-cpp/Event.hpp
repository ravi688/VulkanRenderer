#pragma once

#include <sge/event.h>
#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference

namespace SGE
{
	class Event : public PtrReference<event_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Event);
		
	public:
		typedef event_subscription_handle_t SubscriptionHandle;
		typedef void (*Handler)(void* publisher, void* handlerData);

		friend class RenderWindow;

	public:
		typedef event_subscription_create_info_t SubscriptionCreateInfo;
		void publish() noexcept { event_publish(m_handle); }
		void publish(void* arg) noexcept { event_publish_arg(m_handle, arg); }
		SubscriptionHandle subscribe(Handler handler, void* handlerData = NULL) noexcept
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