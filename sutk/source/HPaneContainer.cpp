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
																					m_grabbedHandle({ NULL, NULL, NULL })
	{
	}

	void HPaneContainer::onMouseClick(MouseButton button, KeyEvent action)
	{
		if(button != MouseButton::Left)
			return;

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
					break;
				}
			}
		}
		else
			m_grabbedHandle = { NULL, NULL, NULL };
	}

	void HPaneContainer::onMouseMove(Vec2Df position)
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
			{
				auto& attr = left->getLayoutAttributes();
				f32 parentWidth = getParent()->getRect().width;
				attr.isNormalized = true;
				attr.prefSize.width = rect.width / parentWidth;
				attr.minSize.width = rect.width / parentWidth;
			}
			
			rect = middle->getRect();
			rect.extendLeft(-disp.x);
			rect.extendRight(disp.x);
			middle->setRect(rect);

			rect = right->getRect();
			rect.extendLeft(-disp.x);
			right->setRect(rect);
			{
				auto& attr = right->getLayoutAttributes();
				f32 parentWidth = getParent()->getRect().width;
				attr.isNormalized = true;
				attr.prefSize.width = rect.width / parentWidth;
				attr.minSize.width = rect.width / parentWidth;
			}
		}
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
}