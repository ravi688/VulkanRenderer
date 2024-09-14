#include <sutk/Container.hpp>

#include <sutk/assert.h> /* _assert() */
#include <sutk/RenderableContainer.hpp>
#include <sutk/RenderRectOutline.hpp>
#include <sutk/ILayoutController.hpp> // for SUTK::ILayoutController

namespace SUTK
{
	Container::Container(SUTK::UIDriver& driver, Container* parent, bool isLayoutIgnore) : 
															UIDriverObject(driver), 
															m_rect({0, 0, 5.0f, 5.0f}),
															m_layoutAttr({ 0, 0 }, { std::numeric_limits<f32>::max(), std::numeric_limits<f32>::max() }, { 5.0f, 5.0f }),
															m_isLayoutIgnore(isLayoutIgnore),
															m_anchorRect(NULL),
															m_parent(NULL), 
															m_renderRectCont(NULL), 
															m_renderRect(NULL), 
															m_isDebug(false)
	{
		if(parent != NULL)
			setParent(parent, false);
	}

	Container::~Container()
	{
		if(m_anchorRect != NULL)
			delete m_anchorRect;
	}

	void Container::setParentChildRelation(Container* parent) noexcept
	{
		// previous parent is non-null, then remove this container from the parent
		if(m_parent != NULL)
		{
			// remove this container from the list of "childs" of the (old) parent container
			bool result = com::erase_first_if(m_parent->m_containers.begin(), m_parent->m_containers.end(), [this](Container*& _container) { return _container == this; });
			_assert(result == true);
			_com_assert(m_anchorRect != NULL);
			if(parent == NULL)
				m_anchorRect->setParent(NULL);
			// invoke onRemoveChild callback for the parent container as a child 'this' has been removed from that
			m_parent->onRemoveChild(this);
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
			if(m_anchorRect == NULL)
				m_anchorRect = new AnchorRect(this, parent);
			else
				m_anchorRect->setParent(parent);
			// invoke onAdd callback function for this container as it has now been added into another (parent) container
			onAdd(this);
			// invoke onAddChild callback function for the parent container as a new child 'this' has been added into that
			parent->onAddChild(this);
		}
	}

	void Container::setParent(Container* parent, bool isInvariantPos) noexcept
	{
		if(parent != NULL)
			parent->add(this, isInvariantPos);
		else if(m_parent != NULL)
			m_parent->remove(this);
	}

	bool Container::containsGlobalCoords(Vec2Df globalCoords) const noexcept
	{
		if(getParent() != NULL)
			return contains(getParent()->getScreenCoordsToLocalCoords(globalCoords));
		else
			return contains(globalCoords);
	}

	void Container::alwaysFitInParent() noexcept
	{
		Container* parent = getParent();
		if(parent != NULL)
			setRect({ { 0.0f, 0.0f }, parent->getSize() });
		getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
	}

	void Container::fitInParent() noexcept
	{
		auto parent = getParent();
		if(parent == NULL)
			return;
		setRect({ Vec2Df::zero(), parent->getSize() });
	}

	u32 Container::getDepth() const noexcept
	{
		if(getParent() == NULL)
			return 0;
		return getParent()->getDepth() + 1u;
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

	void Container::add(Container* container, bool isInvariantPos)
	{
		_assert(container != NULL);
		if(isInvariantPos)
		{
			// recalculate this container's rect into the local space of new parent container
			Vec2Df screenCoords = container->getLocalCoordsToScreenCoords({ 0u, 0u });
			Vec2Df localCoords = getScreenCoordsToLocalCoords(screenCoords);
			container->setPosition(localCoords);
		}
		else
			container->setPosition({ 0.0f, 0.0f });

		container->setParentChildRelation(this);
	}

	void Container::remove(Container* container)
	{
		_assert(container != NULL);
		container->setParentChildRelation(NULL);
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
				// NOTE: a Debug rect must have ignore layout flag set to 'true' to avoid its participation in layouting with other non-debug elements.
				m_renderRectCont = getUIDriver().createContainer<RenderableContainer>(this, true);
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

	void Container::recalculateLayoutParent() noexcept
	{
		Container* parent = getParent();
		if(parent != NULL)
		{
			ILayoutController* layoutCtrlr = dynamic_cast<ILayoutController*>(parent);
			if(layoutCtrlr != NULL)
				layoutCtrlr->recalculateLayout();
		}
	}

	void Container::setLayoutAttributes(const LayoutAttributes& attrs) noexcept
	{
		m_layoutAttr = attrs;
		if(!isLayoutIgnore())
			recalculateLayoutParent();
	}

	void Container::setLayoutIgnore(bool isIgnore) noexcept
	{
		// if either already ignored or realised, return.
		if(m_isLayoutIgnore == isIgnore)
			return;
		recalculateLayoutParent();
		m_isLayoutIgnore = isIgnore;
	}

	void Container::setLayoutExpand() noexcept
	{
		auto& attr = getLayoutAttributes();
		attr.prefSize = attr.maxSize;
		setLayoutAttributes(attr);
	}

	void Container::setActive(bool isActive) noexcept
	{
		bool oldActiveState = Activatable::isActive();
		// Mandatory to be called, and it must precede the layout rebuilding (below code)
		// as layout rebuild checks if the container has layout ignore flag set or not, in which 
		// active state is also considered. 
		Activatable::setActive(isActive);
		// If active state is being changed then we need to rebuild the layout
		if(oldActiveState != isActive)
			recalculateLayoutParent();
	}
}