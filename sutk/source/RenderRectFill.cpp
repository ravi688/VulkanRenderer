#include <sutk/RenderRectFill.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/assert.h>

namespace SUTK
{
	RenderRectFill::RenderRectFill(UIDriver& driver, RenderableContainer* container, RenderMode renderMode, bool isUpdate) noexcept : RenderRect(driver, container, renderMode)
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(4)
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3)
			.fillColor(getColor());

		if(isUpdate)
		{
			// call update for the first time as we have geometry description already and
			// this geometry is supposed to be displayed in the very first frame.
			update();
		}
	}

	void RenderRectFill::update() noexcept
	{
		if(isRectDirty())
		{
			Geometry::VertexPositionArray& array = getGeometry().getVertexPositionArrayForWrite();
			_com_assert(array.size() == 4);
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
		RenderRect::update();
	}
}