#include <sutk/Container.hpp>

#include <sutk/assert.h> /* _assert() */
#include <sutk/RenderableContainer.hpp>
#include <sutk/RenderRectOutline.hpp>
#include <sutk/ILayoutController.hpp> // for SUTK::ILayoutController
#include <sutk/ContainerUtility.hpp>
#include <sutk/InputEventHandlerObject.hpp> // for dynamic_cast to work for SUTK::IOrderedInputEventHandlerObject

namespace SUTK
{
	Container::Container(SUTK::UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) : 
															UIDriverObject(driver), 
															m_inputEventHandlers(NULL),
															m_rect({0, 0, 5.0f, 5.0f}),
															m_layoutAttr(false, { 0, 0 }, { 0, 0 }, { F32_INFINITY, F32_INFINITY }, { 5.0f, 5.0f }),
															m_isLayoutIgnore(isLayoutIgnore),
															m_anchorRect(NULL),
															m_parent(NULL), 
															#ifdef CONTAINER_DEBUG
															m_renderRectCont(NULL), 
															m_renderRect(NULL), 
															m_isDebug(false),
															#endif
															m_recycleState(RecycleState::Undefined),
															m_layer(layer)
	{
		if(parent != NULL)
			setParent(parent, false);
	}

	Container::~Container()
	{
		com_assert(COM_DESCRIPTION(getChildCount() == 0), "You must destroy child containers first!");
		
		if(m_parent)
			m_parent->remove(this);
		
		#ifdef CONTAINER_DEBUG
		if(m_renderRectCont)
		{
			// Renderable should be destroyed first.
			_com_assert(m_renderRect != com::null_pointer<RenderRectOutline>());
			getUIDriver().destroyRenderable<RenderRectOutline>(m_renderRect);
			getUIDriver().destroyContainer<RenderableContainer>(m_renderRectCont);
		}
		#endif

		if(m_inputEventHandlers)
			delete m_inputEventHandlers;
		if(m_anchorRect != NULL)
			delete m_anchorRect;
	}

	std::vector<IInputEventHandlerObject*>& Container::getInputEventHandlers() noexcept
	{
		if(!m_inputEventHandlers)
			m_inputEventHandlers = new std::vector<IInputEventHandlerObject*>();
		return *m_inputEventHandlers;
	}

	const std::vector<IInputEventHandlerObject*>& Container::getInputEventHandlers() const noexcept
	{
		return com::cast_away_const(this)->getInputEventHandlers();
	}

	void Container::updateEventsOrder() noexcept
	{
		com_assert(COM_DESCRIPTION(hasInputEventHandlers()), "First check by calling hasInputEventHandlers() and then only call updateEventsOrder()");
		std::vector<IInputEventHandlerObject*>& handlers = getInputEventHandlers();
		for(auto& handler : handlers)
		{
			auto _handler = dynamic_cast<IOrderedInputEventHandlerObject*>(handler);
			if(_handler)
				_handler->updateOrder();
		}
	}

	void Container::updateEventsOrderRecursive() noexcept
	{
		if(hasInputEventHandlers())
			updateEventsOrder();
		std::vector<Container*>& childs = getChilds();
		for(Container* &child : childs)
			if(child->hasInputEventHandlers())
				child->updateEventsOrder();
	}

	void Container::setParentChildRelation(Container* parent, std::size_t index) noexcept
	{
		// previous parent is non-null, then remove this container from the parent
		if(m_parent != NULL)
		{
			// remove this container from the list of "childs" of the (old) parent container
			bool result = com::find_erase(m_parent->m_containers, this);
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
			if(index == std::numeric_limits<std::size_t>::max())
				parent->m_containers.push_back(this);
			else
			{
				_com_assert(index <= parent->m_containers.size());
				DEBUG_BLOCK(
					if(index == parent->m_containers.size())
						_com_assert(std::next(parent->m_containers.begin(), index) == parent->m_containers.end());
				);
				parent->m_containers.insert(std::next(parent->m_containers.begin(), index), this);
			}
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

		ContainerUtility::ContainersVisit(this, [parent](Container* container) noexcept
			{
				container->onAnscestorChange(parent);
			});

		updateEventsOrderRecursive();
	}

	void Container::setParent(Container* parent, bool isInvariantPos) noexcept
	{
		if(parent != NULL)
			parent->add(this, isInvariantPos);
		else if(m_parent != NULL)
			m_parent->remove(this);
	}

	Rect2Df Container::getGlobalRect() const noexcept
	{
		if(m_parent != NULL)
			return m_parent->getLocalCoordsToScreenCoords(getRect());
		else
			return getRect();
	}

	Vec2Df Container::getGlobalPosition() const noexcept
	{
		if(m_parent != NULL)
			return m_parent->getLocalCoordsToScreenCoords(getPosition());
		else
			return getPosition();
	}

	bool Container::containsGlobalCoords(Vec2Df globalCoords) const noexcept
	{
		if(getParent() != NULL)
			return contains(getParent()->getScreenCoordsToLocalCoords(globalCoords));
		else
			return contains(globalCoords);
	}

	void Container::setRect(Rect2Df rect) noexcept
	{ 
		onBeforeResize(rect, true, true); 
		m_rect = rect; 
		onResize(rect, true, true); 
	}

	void Container::setPosition(Vec2Df pos) noexcept
	{ 
		onBeforeResize({ pos, m_rect.getSize() }, true, false); 
		m_rect.setPosition(pos); 
		onResize(m_rect, true, false); 
	}

	void Container::setSize(Vec2Df size) noexcept
	{ 
		onBeforeResize({ m_rect.getPosition(), size }, false, true); 
		m_rect.setSize(size); 
		onResize(m_rect, false, true); 
	}

	void Container::moveRight(f32 amount) noexcept
	{
		Vec2Df pos = getPosition();
		pos.x += amount;
		setPosition(pos);
	}

	void Container::moveLeft(f32 amount) noexcept
	{
		Vec2Df pos = getPosition();
		pos.x -= amount;
		setPosition(pos);
	}

	void Container::extendRight(f32 amount) noexcept
	{
		Rect2Df rect = getRect();
		rect.extendRight(amount);
		// Calling setRect() might be expensive
		setSize(rect.getSize());
	}

	void Container::alwaysFitInParent() noexcept
	{
		Container* parent = getParent();
		if(parent != NULL)
			setRect({ { 0.0f, 0.0f }, parent->getSize() });
		getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
	}

	void Container::fitInParent(Vec4Df margins) noexcept
	{
		auto parent = getParent();
		if(parent == NULL)
			return;
		auto size = parent->getSize();
		setRect({ { margins.x, margins.z }, { size.width - margins.y - margins.x, size.height - margins.w - margins.z } });
	}

	u32 Container::getDepth() const noexcept
	{
		if(getLayer() != InvalidLayer)
			return getLayer();
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
		
		// calculate this container's rect into the local space of new parent container
		Vec2Df screenCoords;
		if(isInvariantPos)
			screenCoords = container->getLocalCoordsToScreenCoords({ 0u, 0u });
		
		container->setParentChildRelation(this);

		if(isInvariantPos)
		{
			Vec2Df localCoords = getScreenCoordsToLocalCoords(screenCoords);
			container->setPosition(localCoords);
		}
		else container->setPosition({ 0.0f, 0.0f });
	}

	void Container::addAt(Container* container, std::size_t index, bool isInvariantPos)
	{
		_assert(container != NULL);
		
		// calculate this container's rect into the local space of new parent container
		Vec2Df screenCoords;
		if(isInvariantPos)
			screenCoords = container->getLocalCoordsToScreenCoords({ 0u, 0u });
		
		container->setParentChildRelation(this, index);

		if(isInvariantPos)
		{
			Vec2Df localCoords = getScreenCoordsToLocalCoords(screenCoords);
			container->setPosition(localCoords);
		}
		else container->setPosition({ 0.0f, 0.0f });
	}

	void Container::remove(Container* container)
	{
		Vec2Df screenCoords = container->getLocalCoordsToScreenCoords({ 0u, 0u });
		_assert(container != NULL);
		container->setParentChildRelation(NULL);
		_com_assert(container->getParent() == NULL);
		container->setPosition(screenCoords);
	}

	void Container::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		if(m_anchorRect != NULL)
			m_anchorRect->onParentResize(newRect, isPositionChanged, isSizeChanged);
	}

	void Container::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		#ifdef CONTAINER_DEBUG
		// if this container is resized and update the renderRect's size as well
		if(m_renderRectCont != NULL)
		{
			_assert(m_isDebug);
			Vec2Df pos = getLocalCoordsToScreenCoords({ 0, 0 });
			m_renderRectCont->setRect({ pos.x, pos.y, newRect.width, newRect.height });
		}
		#endif

		if(m_anchorRect != NULL)
			m_anchorRect->onChildResize(newRect, isPositionChanged, isSizeChanged);

		// inform the child containers that parent's rect has been resized or has changed its position
		for(Container*& container : m_containers)
			container->onParentResize(newRect, isPositionChanged, isSizeChanged);
	}

	void Container::enableDebug(bool isEnable, Color4 color) noexcept
	{
		#ifdef CONTAINER_DEBUG
		if(isEnable)
		{
			// only create SUTK::RenderableContainer and SUTK::RenderRect once in the life-time of this Container
			if(m_renderRectCont == NULL)
			{
				// create SUTK::RenderableContainer and setup its rect
				// NOTE: a Debug rect must have ignore layout flag set to 'true' to avoid its participation in layouting with other non-debug elements.
				m_renderRectCont = getUIDriver().createContainer<RenderableContainer>(getUIDriver().getDebugRootContainer(), com::Bool::True());
				Vec2Df pos = getLocalCoordsToScreenCoords({ 0, 0 });
				m_renderRectCont->setRect({ pos.x, pos.y, getRect().width, getRect().height });
				
				// create SUTK::RenderRect and establish parent-child link with SUTK::RenderableContainer just created
				m_renderRect = getUIDriver().createRenderable<RenderRectOutline>(m_renderRectCont);
				m_renderRect->setThickness(0.05f);
			}
			else
			{
				_com_assert(m_renderRect != NULL);
				m_renderRect->setActive(com::Bool::True());
			}
			m_isDebug = true;
		}
		else if(m_renderRect != NULL)
		{
			m_renderRect->setActive(com::Bool::False());
			m_isDebug = false;
		}

		if(m_renderRect != NULL)
			m_renderRect->setColor(color);
		#endif
	}

	void Container::setLayer(Layer layer) noexcept
	{
		m_layer = layer;
		// Visit each Renderable recursively and update its draw order
		ContainerUtility::RenderablesVisit(this, [](Renderable* renderable)
		{
			Container* container = renderable->getContainer();
			_com_assert(container != com::null_pointer<Container>());
				renderable->setDrawOrder(container->getDepth());
		});
		updateEventsOrderRecursive();
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

	LayoutAttributes Container::getAbsoluteLayoutAttributes() const noexcept
	{
		auto parent = getParent();
		if(!(parent && m_layoutAttr.isNormalized))
			return m_layoutAttr;
		LayoutAttributes absAttr { };
		absAttr.isNormalized = false;
		auto size = parent->getRect().getSize();
		size += m_layoutAttr.offset;
		absAttr.minSize = m_layoutAttr.minSize * size;
		absAttr.maxSize = m_layoutAttr.maxSize * size;
		if((m_layoutAttr.prefSize == Vec2Df(1.0f, 1.0f))
			|| (m_layoutAttr.prefSize == Vec2Df(F32_INFINITY, F32_INFINITY)))
			absAttr.prefSize = { F32_INFINITY, F32_INFINITY };
		else
			absAttr.prefSize = m_layoutAttr.prefSize * size;
		return absAttr;
	}

	void Container::setLayoutIgnore(com::Bool isIgnore) noexcept
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

	void Container::setLayoutFixed(Vec2Df fixedSize) noexcept
	{
		auto& attr = getLayoutAttributes();
		attr.minSize = fixedSize;
		attr.prefSize = fixedSize;
		setLayoutAttributes(attr);
	}

	void Container::swapChildren(u32 index1, u32 index2) noexcept
	{
		_com_assert(index1 < getChildCount());
		_com_assert(index2 < getChildCount());
		std::swap(m_containers[index1], m_containers[index2]);
	}

	void Container::setActive(com::Bool isActive) noexcept
	{
		com::Bool oldActiveState = Activatable::isActive();
		// Mandatory to be called, and it must precede the layout rebuilding (below code)
		// as layout rebuild checks if the container has layout ignore flag set or not, in which 
		// active state is also considered. 
		Activatable::setActive(isActive);
		// If active state is being changed then we need to rebuild the layout
		if(oldActiveState != isActive)
			recalculateLayoutParent();
	}
}