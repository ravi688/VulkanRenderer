#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderRectContainer.hpp>

#include <sutk/assert.h>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderRectContainer* container) noexcept : UIDriverObject(driver), 
																			m_handle(GFX_DRIVER_OBJECT_NULL_HANDLE), 
																			m_isDirty(true), 
																			m_rect({ 0, 0, 100, 100 }), 
																			m_container(container),
																			m_geometry(driver),
																			m_thickness(0.2f)
	{
		m_geometry
			.vertexPositionArray(8)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3) // top line
			.quad(3, 2, 4, 5) // right line
			.quad(5, 4, 6, 7) // bottom line
			.quad(7, 6, 1, 0);// left line
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
			_assert(array.size() == 8);
			auto topLeft = getContainer()->getLocalCoordsToScreenCoords(m_rect.getTopLeft());
			auto bottomLeft = getContainer()->getLocalCoordsToScreenCoords(m_rect.getBottomLeft());
			auto bottomRight = getContainer()->getLocalCoordsToScreenCoords(m_rect.getBottomRight());
			auto topRight = getContainer()->getLocalCoordsToScreenCoords(m_rect.getTopRight());

			/*   0   		  3
				 _____________
				|\___________/|
				| |1	   2| |
				| |			| |
				| |6	   4| |
				| |_________| |
				|/___________\|
				7  			  5
			*/

			auto downRight = (Vec2Df::down() + Vec2Df::right()) * m_thickness * 0.5f;
			auto downLeft = (Vec2Df::down() + Vec2Df::left()) * m_thickness * 0.5f;
			array[0] = static_cast<Vec2Df>(topLeft) - downRight;
			array[1] = static_cast<Vec2Df>(topLeft) + downRight;
			array[2] = static_cast<Vec2Df>(topRight) + downLeft;
			array[3] = static_cast<Vec2Df>(topRight) - downLeft;

			auto upLeft = (Vec2Df::up() + Vec2Df::left()) * m_thickness * 0.5f;
			auto upRight = (Vec2Df::up() + Vec2Df::right()) * m_thickness * 0.5f;
			array[4] = static_cast<Vec2Df>(bottomRight) + upLeft;
			array[5] = static_cast<Vec2Df>(bottomRight) - upLeft;
			array[6] = static_cast<Vec2Df>(bottomLeft) + upRight;
			array[7] = static_cast<Vec2Df>(bottomLeft) - upRight;

			m_handle = m_geometry.compile(m_handle);
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(m_rect.getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}
	}

	void RenderRect::setThickness(f32 thickness) noexcept
	{
		 m_thickness = thickness;
		 m_isSizeDirty = true;
	}

	void RenderRect::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		m_rect = rect;
		m_isDirty = true;
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}
}