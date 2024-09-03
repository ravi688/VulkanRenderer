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
		m_isActive = isActive; 
		if(m_onActiveEvent != NULL)
			m_onActiveEvent->publish(isActive);
	}

	Activatable::OnActiveEvent& Activatable::getOnActiveEvent() noexcept
	{
		if(m_onActiveEvent == NULL)
			m_onActiveEvent = new OnActiveEvent(this);
		return *m_onActiveEvent;
	}
}
