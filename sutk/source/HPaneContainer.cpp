#include <sutk/HPaneContainer.hpp>
#include <sutk/RenderRectFill.hpp>

#define HANDLE_HIGHLIGHT_COLOR Color4::grey(0.5f)

namespace SUTK
{
	HPaneContainer::HPaneContainer(UIDriver& driver, Container* parent) noexcept : Container(driver, parent),
																					MouseClickHandlerObject(driver, this),
																					MouseMoveHandlerObject(driver, this)
	{
	}

	void HPaneContainer::onMouseClick(MouseButton button, KeyEvent action)
	{
		// if((m_handleRect == NULL) || (button != MouseButton::Left))
		// 	return;

		// if(action == KeyEvent::Press)
		// {
		// 	Vec2Df pos = getInputDriver().getMousePosition();
		// 	if(m_handleRect->containsGlobalCoords(pos))
		// 		m_isGrabbed = true;
		// }
		// else
		// 	m_isGrabbed = false;
	}

	void HPaneContainer::onMouseMove(Vec2Df position)
	{
	}

	void HPaneContainer::onAddChild(Container* child)
	{
		m_externalChilds.push_back(child);
	}

	void HPaneContainer::onRemoveChild(Container* child)
	{
		bool result = com::find_erase(m_externalChilds, child);
		_com_assert(result);
	}
}