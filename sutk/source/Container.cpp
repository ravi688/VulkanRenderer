#include <sutk/Container.hpp>

#include <sutk/assert.h> /* _assert() */
#include <sutk/RenderableContainer.hpp>
#include <sutk/RenderRect.hpp>

namespace SUTK
{
	Container::Container(SUTK::UIDriver& driver, Container* parent) : 
															UIDriverObject(driver), 
															m_rect({0, 0, 5.0f, 5.0f}),
															m_anchorRect(NULL),
															m_parent(NULL), 
															m_renderRectCont(NULL), 
															m_renderRect(NULL), 
															m_isDebug(false)
	{
		setParent(parent);
		if(parent != NULL)
			m_anchorRect = new AnchorRect(*this, *parent);
	}

	Container::~Container()
	{
		if(m_anchorRect != NULL)
			delete m_anchorRect;
	}

	void Container::setParent(Container* parent) noexcept
	{
		// recalculate this container's rect into the local space of new parent container
		Vec2Df screenCoords = getLocalCoordsToScreenCoords({ 0u, 0u });
		Vec2Df localCoords = (parent != NULL) ? parent->getScreenCoordsToLocalCoords(screenCoords) : screenCoords;
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

	bool Container::containsGlobalCoords(Vec2Df globalCoords) const noexcept
	{
		if(getParent() != NULL)
			return contains(getParent()->getScreenCoordsToLocalCoords(globalCoords));
		else
			return contains(globalCoords);
	}

	Vec2Df Container::getScreenCoordsToLocalCoords(Vec2Df screenCoords) const
	{
		return screenCoords - getLocalCoordsToScreenCoords({ 0u, 0u });
	}

	Vec2Df Container::getLocalCoordsToScreenCoords(Vec2Df localCoords) const
	{
		if(getParent() != NULL)
			return getParent()->getLocalCoordsToScreenCoords(getRect().getPosition()) + localCoords;
		return getRect().getPosition() + localCoords;
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

	void Container::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		if(m_anchorRect != NULL)
			m_anchorRect->onParentResize(newRect, isPositionChanged, isSizeChanged);
	}

	void Container::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// if this container is resized and update the renderRect's size as well
		if(m_renderRectCont != NULL)
		{
			_assert(m_isDebug);
			if(isSizeChanged)
				m_renderRectCont->setRect({ 0, 0, newRect.width, newRect.height });
		}

		if(m_anchorRect != NULL)
			m_anchorRect->onChildResize(newRect, isPositionChanged, isSizeChanged);

		// inform the child containers that parent's rect has been resized or has changed its position
		for(Container*& container : m_containers)
			container->onParentResize(newRect, isPositionChanged, isSizeChanged);
	}

	void Container::enableDebug(bool isEnable, Color4 color) noexcept
	{
		if(isEnable)
		{
			// only create SUTK::RenderableContainer and SUTK::RenderRect once in the life-time of this Container
			if(m_renderRectCont == NULL)
			{
				// create SUTK::RenderableContainer and setup its rect
				m_renderRectCont = getUIDriver().createContainer<RenderableContainer>(this);
				m_renderRectCont->setRect({ 0, 0, getRect().width, getRect().height });
				
				// create SUTK::RenderRect and establish parent-child link with SUTK::RenderableContainer just created
				m_renderRect = getUIDriver().createRenderable<RenderRectOutline>(m_renderRectCont);
				m_renderRect->setThickness(0.05f);
			}
			else
			{
				_com_assert(m_renderRect != NULL);
				m_renderRect->setActive(true);
			}
			m_isDebug = true;
		}
		else if(m_renderRect != NULL)
		{
			m_renderRect->setActive(false);
			m_isDebug = false;
		}

		if(m_renderRect != NULL)
			m_renderRect->setColor(color);
	}
}