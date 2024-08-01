#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderRectContainer.hpp>

#include <sutk/assert.h>

namespace SUTK
{
	RenderRectOutline::RenderRectOutline(UIDriver& driver, RenderRectContainer* container) noexcept : RenderRect(driver, container),
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_thickness(0.2f)
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(8)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3) // top line
			.quad(3, 2, 4, 5) // right line
			.quad(5, 4, 6, 7) // bottom line
			.quad(7, 6, 1, 0);// left line
	}

	bool RenderRectOutline::isDirty()
	{
		return m_isPosDirty || m_isSizeDirty;
	}

	void RenderRectOutline::update()
	{
		if(m_isSizeDirty || m_isPosDirty)
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_assert(array.size() == 8);
			Rect2Df rect = getRect();
			auto topLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getTopLeft());
			auto bottomLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomLeft());
			auto bottomRight = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomRight());
			auto topRight = getContainer()->getLocalCoordsToScreenCoords(rect.getTopRight());

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
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isPosDirty || m_isSizeDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
		}
	}

	void RenderRectOutline::setThickness(f32 thickness) noexcept
	{
		 m_thickness = thickness;
		 m_isSizeDirty = true;
	}

	void RenderRectOutline::onGlobalCoordDirty() noexcept
	{
		m_isPosDirty = true;
	}

	void RenderRectOutline::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(m_isSizeDirty)
		{
			// The rect for this RenderRect is in local coordinates of the RenderRectContainer for this RenderRect.
			// Therefore, the position would always be { 0, 0 } for this rect.
			rect.setPosition({ 0, 0 });
			setRect(rect);
		}
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}

	RenderRectFill::RenderRectFill(UIDriver& driver, RenderRectContainer* container) noexcept : RenderRect(driver, container),
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_isColorDirty(true),
																						m_color(Color3::white())
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(4)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3)
			.fillColor(m_color);
	}

	bool RenderRectFill::isDirty()
	{
		return m_isPosDirty || m_isSizeDirty;
	}

	void RenderRectFill::update()
	{
		if(m_isSizeDirty || m_isPosDirty)
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_assert(array.size() == 4);
			Rect2Df rect = getRect();
			auto topLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getTopLeft());
			auto bottomLeft = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomLeft());
			auto bottomRight = getContainer()->getLocalCoordsToScreenCoords(rect.getBottomRight());
			auto topRight = getContainer()->getLocalCoordsToScreenCoords(rect.getTopRight());

			array[0] = topLeft;
			array[1] = bottomLeft;
			array[2] = bottomRight;
			array[3] = topRight;
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isColorDirty)
		{
			getGeometry().fillColor(m_color);
		}

		if(m_isPosDirty || m_isSizeDirty || m_isColorDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
			m_isColorDirty = false;
		}
	}

	void RenderRectFill::setColor(Color3 color) noexcept
	{
		 m_color = color;
		 m_isColorDirty = true;
	}

	void RenderRectFill::onGlobalCoordDirty() noexcept
	{
		m_isPosDirty = true;
	}

	void RenderRectFill::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(m_isSizeDirty)
		{
			// The rect for this RenderRect is in local coordinates of the RenderRectContainer for this RenderRect.
			// Therefore, the position would always be { 0, 0 } for this rect.
			rect.setPosition({ 0, 0 });
			setRect(rect);
		}
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}
}