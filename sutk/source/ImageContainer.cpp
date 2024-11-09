#include <sutk/ImageContainer.hpp>
#include <sutk/RenderImage.hpp>

namespace SUTK
{
	ImageContainer::ImageContainer(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore, Layer layer) noexcept : RenderableContainer(driver, parent, isLayoutIgnore, layer)
	{
		m_renderImage = driver.createRenderable<RenderImage>(this);
	}

	ImageContainer::~ImageContainer() noexcept
	{
		getUIDriver().destroyRenderable<RenderImage>(m_renderImage);
	}

	void ImageContainer::setImage(UIDriver::ImageReference image) noexcept
	{
		m_renderImage->setImage(image);
	}
}