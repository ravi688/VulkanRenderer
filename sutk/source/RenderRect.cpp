#include <sutk/RenderRect.hpp>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept : UIDriverObject(driver), 
																			m_handle(GFX_DRIVER_OBJECT_NULL_HANDLE), 
																			m_isDirty(true), 
																			m_rect({ 0, 0, 100, 100 }), 
																			m_container(container),
																			m_geometry(driver)
	{
		m_geometry.vertexPositionArray(4).lineStroke(2.0f, Color3::white());
	}

	bool RenderRect::isDirty()
	{
		return m_isDirty;
	}

	void RenderRect::update()
	{
		if(!m_isDirty) return;
		m_isDirty = false;

		Geometry::VertexPositionArray& array = m_geometry.getVertexPositionArray();
		array[0] = m_rect.getTopLeft() + m_rect.getCenter();
		array[1] = m_rect.getBottomLeft() + m_rect.getCenter();
		array[2] = m_rect.getBottomRight() + m_rect.getCenter();
		array[3] = m_rect.getTopRight() + m_rect.getCenter();

		m_handle = m_geometry.compile(m_handle);
	}

	void RenderRect::setRect(Rect2D<DisplaySizeType> rect) noexcept
	{
		m_rect = rect;
		m_isDirty = true;
	}
}