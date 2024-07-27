#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept : UIDriverObject(driver), 
																			m_handle(GFX_DRIVER_OBJECT_NULL_HANDLE), 
																			m_isDirty(true), 
																			m_rect({ 0, 0, 100, 100 }), 
																			m_container(container),
																			m_geometry(driver)
	{
		m_geometry
			.vertexPositionArray(4)
			.topology(Geometry::Topology::LineStrip)
			.line(0, 1)
			.nextLine(2)
			.nextLine(3)
			.nextLine(0)
			.lineStroke(2.0f, Color3::white());
	}

	bool RenderRect::isDirty()
	{
		return m_isDirty;
	}

	void RenderRect::update()
	{
		if(!m_isDirty) return;
		m_isDirty = false;

		if(m_isSizeDirty)
		{
			Geometry::VertexPositionArray& array = m_geometry.getVertexPositionArray();
			array[0] = m_rect.getTopLeft();
			array[1] = m_rect.getBottomLeft();
			array[2] = m_rect.getBottomRight();
			array[3] = m_rect.getTopRight();
			m_handle = m_geometry.compile(m_handle);
		}

		if(m_isPosDirty)
		{
			auto& gfxDriver = getGfxDriver();
			GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			gfxDriver.setObjectPosition(objHandle, m_rect.getPosition());
		}
	}

	void RenderRect::onContainerResize(Rect2D<DisplaySizeType> rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		m_rect = rect;
		m_isDirty = true;
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}
}