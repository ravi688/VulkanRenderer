#pragma once

#include <common/Event.hpp>

namespace SUTK
{
	class Activatable
	{
	public:
		typedef com::Event<com::no_publish_ptr_t, bool> OnActiveEvent;

	private:
		bool m_isActive;
		/* by default, it is NULL as not many objects subscribe to this event */
		OnActiveEvent* m_onActiveEvent;

	protected:
		Activatable() noexcept : m_isActive(true), m_onActiveEvent() { }

	public:
		~Activatable() noexcept;

		// mandatory to be called in overriding method
		bool isActive() const noexcept { return m_isActive; }
		virtual void setActive(bool isActive) noexcept;

		// if called for the first time then it allocates memory for OnActiveEvent object
		OnActiveEvent& getOnActiveEvent() noexcept;
	};
}
