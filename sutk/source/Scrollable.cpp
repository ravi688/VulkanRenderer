#include <sutk/Scrollable.hpp>
#include <sutk/Container.hpp>
#include <sutk/ScrollContainer.hpp> // otherwise dynamic_cast to this target won't work!

namespace SUTK
{
	Scrollable::Scrollable(Container* container) noexcept
	{
		m_scrollContainer = dynamic_cast<ScrollContainer*>(container->getParent());
		_COM_ASSERT(m_scrollContainer);
	}
}