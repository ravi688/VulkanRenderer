#include <sutk/Container.hpp>

#include <sutk/assert.h> /* _assert() */
#include <sutk/RenderRectContainer.hpp>
#include <sutk/RenderRect.hpp>

namespace SUTK
{
	Container::Container(SUTK::UIDriver& driver, Container* parent) : UIDriverObject(driver), m_rect({0, 0, 100, 100}), m_parent(NULL), m_renderRectCont(NULL), m_renderRect(NULL), m_isDebug(false)
	{
		setParent(parent);
	}

	void Container::setParent(Container* parent) noexcept
	{
		// recalculate this container's rect into the local space of new parent container
		Vec2D<DisplaySizeType> screenCoords = getLocalCoordsToScreenCoords({ 0u, 0u });
		Vec2D<DisplaySizeType> localCoords = (parent != NULL) ? parent->getScreenCoordsToLocalCoords(screenCoords) : screenCoords;
		setPosition(localCoords);

		// previous parent is non-null, then remove this container from the parent
		if(m_parent != NULL)
		{
			// remove this container from the list of "childs" of the (old) parent container
			bool result = com::erase_first_if(m_parent->m_containers.begin(), m_parent->m_containers.end(), [this](Container*& _container) { return _container == this; });
			_assert(result == true);
			m_parent = NULL;
			// invoke onRemove callback for this container as it has now been removed from the old (parent) container
			onRemove(this);
		}

		// if this (new) parent is non-null, then add this container into the parent
		if(parent != NULL)
		{
			// add this container into the list of "childs" of the (new) parent container
			parent->m_containers.push_back(this);
			m_parent = parent;
			// invoke onAdd callback function for this container as it has now been added into another (parent) container
			onAdd(this);
		}
	}

	Vec2D<DisplaySizeType> Container::getScreenCoordsToLocalCoords(Vec2D<DisplaySizeType> screenCoords) const
	{
		return screenCoords - getLocalCoordsToScreenCoords({ 0u, 0u });
	}

	Vec2D<DisplaySizeType> Container::getLocalCoordsToScreenCoords(Vec2D<DisplaySizeType> localCoords) const
	{
		if(getParent() != NULL)
			return getParent()->getLocalCoordsToScreenCoords(getRect().getPosition()) + localCoords;
		return localCoords;
	}

	void Container::onAdd(Container* parent)
	{
		_assert(parent != NULL);
	}

	void Container::onRemove(Container* parent)
	{
		_assert(parent != NULL);
	}

	void Container::add(Container* container)
	{
		_assert(container != NULL);
		container->setParent(this);
	}

	void Container::remove(Container* container)
	{
		_assert(container != NULL);
		container->setParent(NULL);
	}

	void Container::onResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// if this container is resized and update the renderRect's size as well
		if(m_renderRectCont != NULL)
		{
			_assert(m_isDebug);
			m_renderRectCont->setRect({ 0, 0, getRect().width, getRect().height });
		}
	}

	void Container::enableDebug(bool isEnable) noexcept
	{
		// only create SUTK::RenderRectContainer and SUTK::RenderRect once in the life-time of this Container
		if(m_renderRectCont == NULL)
		{
			// create SUTK::RenderRectContainer and setup its rect
			m_renderRectCont = getUIDriver().createContainer<RenderRectContainer>(this);
			m_renderRectCont->setRect({ 0, 0, getRect().width, getRect().height });
			
			// create SUTKU::RenderRect and establish parent-child link with SUTK::RenderRectContainer just created
			m_renderRect = getUIDriver().createRenderable<RenderRect>(m_renderRectCont);
			m_renderRectCont->setRenderRect(m_renderRect);
		}
		m_isDebug = true;
	}
}