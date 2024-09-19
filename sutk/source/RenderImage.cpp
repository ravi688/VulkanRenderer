#include <sutk/RenderImage.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/assert.h>

namespace SUTK
{
	RenderImage::RenderImage(UIDriver& driver, RenderableContainer* container, bool isUpdate) noexcept : RenderRect(driver, container)
	{
		_assert(container != NULL);
		auto rect = container->getRect();
		rect.setPosition({ 0, 0 });
		setRect(rect);
		getGeometry()
			.vertexPositionArray(4)
			.vertexTexCoord({ 0.0f, 0.0f })
			.vertexTexCoord({ 0.0f, 1.0f })
			.vertexTexCoord({ 1.0f, 1.0f })
			.vertexTexCoord({ 1.0f, 0.0f })
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3)
			.fillColor(getColor())
			.fillImage();

		// It is necessarry whenever we modify the 'image' member of Geometry class
		RenderRect::setGeometryDirty(true);

		if(isUpdate)
		{
			// call update for the first time as we have geometry description already and
			// this geometry is supposed to be displayed in the very first frame.
			update();
		}
	}

	void RenderImage::update() noexcept
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

	void RenderImage::setImage(UIDriver::ImageReference image) noexcept
	{
		getGeometry().fillImage(image);
		// Recompile the geometry
		RenderRect::setGeometryDirty(true);
	}
}