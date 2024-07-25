#include <sutk/RenderRect.hpp>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept : UIDriverObject(driver), m_isDirty(true), m_rect({ 0, 0, 100, 100 }), m_container(container)
	{

	}

	bool RenderRect::isDirty()
	{
		return m_isDirty;
	}

	void RenderRect::update()
	{
		if(!m_isDirty) return;
		m_isDirty = false;
	}

	void RenderRect::setRect(Rect2D<DisplaySizeType> rect) noexcept
	{
		m_rect = rect;
		m_isDirty = true;
	}
}