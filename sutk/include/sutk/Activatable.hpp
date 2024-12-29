#pragma once

#include <common/Event.hpp>
#include <common/Bool.hpp>

namespace SUTK
{
	class Activatable
	{
	public:
		typedef com::Event<com::no_publish_ptr_t, com::Bool> OnActiveEvent;

	private:
		com::Bool m_isActive;
		/* by default, it is NULL as not many objects subscribe to this event */
		OnActiveEvent* m_onActiveEvent;

	protected:
		Activatable() noexcept : m_isActive(com::Bool::True()), m_onActiveEvent() { }

	public:
		~Activatable() noexcept;

		com::Bool isActive() const noexcept { return m_isActive; }
		// mandatory to be called in overriding method
		virtual void setActive(com::Bool isActive) noexcept;

		// if called for the first time then it allocates memory for OnActiveEvent object
		OnActiveEvent& getOnActiveEvent() noexcept;
	};
}
