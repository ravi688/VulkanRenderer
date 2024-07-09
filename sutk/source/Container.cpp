#include <sutk/Container.hpp>

#include <sutk/assert.h> /* _assert() */

namespace SUTK
{
	Container::Container(SUTK::UIDriver& driver) : m_driver(driver)
	{

	}

	void Container::onAdd(Container* parent)
	{
		_assert(parent != NULL);
		setRect(parent->getRect());
	}

	void Container::onRemove(Container* parent)
	{
		_assert(parent != NULL);
	}

	void Container::add(Container* container)
	{
		m_containers.push_back(container);
		container->onAdd(this);
	}

	void Container::remove(Container* container)
	{
		bool result = com::erase_first_if(m_containers.begin(), m_containers.end(), [container](Container*& _container) { return _container == container; });
		if(result)
			container->onRemove(this);
	}
}