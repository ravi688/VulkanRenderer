#include <sutk/HPaneContainer.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/RenderableContainer.hpp>

#define HANDLE_HOVER_COLOR Color4::grey(0.5f)
#define HANDLE_IDLE_COLOR Color4::grey(0.2f)
#define HANDLE_WIDTH 0.1f

namespace SUTK
{
	HPaneContainer::HPaneContainer(UIDriver& driver, Container* parent) noexcept : HBoxContainer(driver, parent),
																					MouseClickHandlerObject(driver, this),
																					MouseMoveHandlerObject(driver, this),
																					m_isHandleRect(false),
																					m_grabbedHandle({ NULL, NULL, NULL }),
																					m_isCalibratedForFirstTime(false)
	{
		enableMouseMoveOutside(true);
		MouseMoveHandlerObject::sleep();
	}

	bool HPaneContainer::onMouseClick(MouseButton button, KeyEvent action)
	{
		if(button != MouseButton::Left)
			return true;

		if(action == KeyEvent::Press)
		{
			Vec2Df pos = getInputDriver().getMousePosition();
			for(const auto &handle : m_handleRects)
			{
				if(handle.first->containsGlobalCoords(pos))
				{
					m_grabbedHandle[0] = handle.first;
					m_grabbedHandle[1] = handle.second.left;
					m_grabbedHandle[2] = handle.second.right;
					m_prevPos = pos;
					MouseMoveHandlerObject::awake();
					MouseMoveHandlerObject::grabExclusiveAccess();
					break;
				}
			}
		}
		else
		{
			// If the handle was grabbed, only then call sleep()
			if(m_grabbedHandle[0])
			{
				MouseMoveHandlerObject::releaseExclusiveAccess();
				MouseMoveHandlerObject::sleep();
			}
			m_grabbedHandle = { NULL, NULL, NULL };
		}
		return true;
	}

	bool HPaneContainer::onMouseMove(MouseMoveEvent event, Vec2Df position)
	{
		if(event == MouseMoveEvent::Move)
		{
			if(m_grabbedHandle[0])
			{
				Vec2Df pos = getInputDriver().getMousePosition();
				auto disp = pos - m_prevPos;
				m_prevPos = pos;

				auto left = m_grabbedHandle[1];
				auto middle = m_grabbedHandle[0];
				auto right = m_grabbedHandle[2];

				auto rect = left->getRect();
				rect.extendRight(disp.x);
				left->setRect(rect);
			
				rect = middle->getRect();
				rect.extendLeft(-disp.x);
				rect.extendRight(disp.x);
				middle->setRect(rect);

				rect = right->getRect();
				rect.extendLeft(-disp.x);
				right->setRect(rect);
			}
		}
		return true;
	}

	void HPaneContainer::add(Container* child, bool isInvariantPos)
	{
		if(m_isHandleRect)
		{
			HBoxContainer::add(child, isInvariantPos);
			m_isHandleRect = false;
			return;
		}
		std::size_t externChildCount = m_externalChilds.size();
		if(externChildCount > 0)
		{
			m_isHandleRect = true;
			RenderableContainer* handleRectCont = getUIDriver().createContainer<RenderableContainer>(this);
			LayoutAttributes attr = handleRectCont->getLayoutAttributes();
			attr.minSize.width = HANDLE_WIDTH;
			attr.prefSize.width = HANDLE_WIDTH;
			handleRectCont->setLayoutAttributes(attr);
			RenderRect* handleRect = getUIDriver().createRenderable<RenderRectFill>(handleRectCont);
			handleRect->setColor(HANDLE_IDLE_COLOR);
			_com_assert(getChilds().size() > 1);
			m_handleRects.push_back({ handleRectCont, { getChilds()[getChilds().size() - 2], child } });
		}
		HBoxContainer::add(child, isInvariantPos);
		m_externalChilds.push_back(child);
	}

	void HPaneContainer::remove(Container* child)
	{
		HBoxContainer::remove(child);
		bool result = com::find_erase(m_externalChilds, child);
		_com_assert(result);
	}

	void HPaneContainer::onRecalculateLayout() noexcept
	{
		HBoxContainer::onRecalculateLayout();
		if(!m_isCalibratedForFirstTime)
		{
			m_isCalibratedForFirstTime = true;
			recalibrate();
		}
	}

	void HPaneContainer::recalibrate() noexcept
	{
		lockLayout();
		// Explanation for the offset subtraction:
		// Suppose I is the old/previous width of this rect,
		// and F is the new width of this rect.
		// Then we woulld want to ensure that in the new layout, the handle rects would be consitent with their previous sizes.
		// But the following equation doesn't hold true:
		// F - ((a / I) * F + (b / I) * F) not equals to I - (a + b)
		// Therefore, we need to calculate the normalized attr.minSize after subtracting the aggregate space allocated by handle rects.

		Vec2Df offset = { m_handleRects.size() * HANDLE_WIDTH, 0 };
		Vec2Df size = getRect().getSize();
		size -= offset;
		for(Container* & externChild : m_externalChilds)
		{
			auto attr = externChild->getLayoutAttributes();
			attr.isNormalized = true;
			attr.offset = -offset;
			Vec2Df childSize = externChild->getRect().getSize();
			attr.minSize = childSize / size;
			if(externChild != m_externalChilds.back())
				attr.prefSize = attr.minSize;
			else
				attr.prefSize = { 1.0f, 1.0f };
			externChild->setLayoutAttributes(attr);
		}
		// In case the function recalibrate() function is called manually by the user code
		m_isCalibratedForFirstTime = true;
		unlockLayout();
	}

	void HPaneContainer::onBeforeResize(Rect2Df newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// The user might have changed the sizes of external Containers, therefore we need to recalculate
		// the normalized values so that the same space allocation ratio can be maintained for the newer size.
		if(!isLockedLayout() && m_isCalibratedForFirstTime)
			recalibrate();
	}
}