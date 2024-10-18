#include <sutk/RenderRectOutline.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/assert.h>

namespace SUTK
{
	RenderRectOutline::RenderRectOutline(UIDriver& driver, RenderableContainer* container, RenderMode renderMode) noexcept : RenderRect(driver, container, renderMode),
																						m_thickness(0.2f),
																						m_isThicknessDirty(true)
	{
		_com_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(8)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3) // top line
			.quad(3, 2, 4, 5) // right line
			.quad(5, 4, 6, 7) // bottom line
			.quad(7, 6, 1, 0)// left line
			.fillColor(getColor());

		// call update for the first time as we have geometry description already and
		// this geometry is supposed to be displayed in the very first frame.
		update();
	}

	bool RenderRectOutline::isDirty() noexcept
	{
		return RenderRect::isDirty() || m_isThicknessDirty;
	}

	void RenderRectOutline::update() noexcept
	{
		if(isRectDirty() || m_isThicknessDirty)
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_com_assert(array.size() == 8);
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

		RenderRect::update();
	}

	void RenderRectOutline::setThickness(f32 thickness) noexcept
	{
		 m_thickness = thickness;
		 m_isThicknessDirty = true;
	}
}