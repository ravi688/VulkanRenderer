#include <sutk/Activatable.hpp>

namespace SUTK
{
	Activatable::~Activatable() noexcept
	{
		if(m_onActiveEvent != NULL)
			delete m_onActiveEvent;
	}

	void Activatable::setActive(bool isActive) noexcept 
	{
		bool isPublish = m_isActive ^ isActive;
		m_isActive = isActive; 
		// Only publish the event if there is a change, otherwise don't
		if(m_onActiveEvent != NULL && isPublish)
			m_onActiveEvent->publish(isActive);
	}

	Activatable::OnActiveEvent& Activatable::getOnActiveEvent() noexcept
	{
		if(m_onActiveEvent == NULL)
			m_onActiveEvent = new OnActiveEvent();
		return *m_onActiveEvent;
	}
}
