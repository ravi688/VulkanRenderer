#include <sutk/RenderImage.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/assert.h>

namespace SUTK
{
	RenderImage::RenderImage(UIDriver& driver, RenderableContainer* container, RenderMode renderMode, bool isUpdate) noexcept : RenderRect(driver, container, renderMode), m_aspectRatio({ 1, 1 }), m_isPreserveAspectRatio(false), m_isImagePreserveAspectRatio(false)
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
			// Set white (default) image
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
		if(m_isImagePreserveAspectRatio)
			preserveImageAspectRatio();
	}

	UIDriver::ImageReference RenderImage::getImage() noexcept
	{
		return getGeometry().getFillImage();
	}

	void RenderImage::preserveImageAspectRatio() noexcept
	{
		m_isImagePreserveAspectRatio = true;
		UIDriver::ImageReference image = getImage();
		if(image != UIDriver::InvalidImage)
		{
			UIDriver::ImageAttributes attr = getUIDriver().getImageAttributes(image);
			setAspectRatio({ attr.width, attr.height });
		}
		setPreserveAspectRatio(true);
	}

	void RenderImage::setAspectRatio(AspectRatio aspectRatio) noexcept
	{
		m_aspectRatio = aspectRatio;
		if(m_isPreserveAspectRatio)
			updateImageRect();
	}

	void RenderImage::setPreserveAspectRatio(bool isPreserve) noexcept
	{
		m_isPreserveAspectRatio = isPreserve;
		updateImageRect();
	}

	Rect2Df RenderImage::calculateImageRectAR(Rect2Df rect) noexcept
	{
		Rect2Df imageRect;

		// In case you aren't able to understand it, then try to visualize it.
		auto ar = m_aspectRatio.nominator / static_cast<f32>(m_aspectRatio.denominator);
		auto ar2 = rect.width / rect.height;

		if(ar < ar2)
		{
			imageRect.height = rect.height;
			imageRect.width = rect.height * ar;
			imageRect.x = (rect.width - imageRect.width) * 0.5f;
		}
		else
		{
			imageRect.width = rect.width;
			imageRect.height = rect.width * m_aspectRatio.denominator / static_cast<f32>(m_aspectRatio.nominator);
			imageRect.y = (rect.height - imageRect.height) * 0.5f;
		}
		return imageRect;
	}

	void RenderImage::updateImageRect(Rect2Df rect) noexcept
	{
		Rect2Df imageRect;
		if(!m_isPreserveAspectRatio)
		{
			imageRect = getContainer()->getRect();
			imageRect.setPosition({ 0, 0 });
		}
		else
			imageRect = calculateImageRectAR(rect);
		setRect(imageRect);
	}

	void RenderImage::updateImageRect() noexcept
	{
		updateImageRect(getContainer()->getRect());
	}

	void RenderImage::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(!m_isPreserveAspectRatio)
		{
			RenderRect::onContainerResize(rect, isPositionChanged, isSizeChanged);
			return;
		}
		if(!isSizeChanged)
			return;
		updateImageRect(rect);
	}
}