#pragma once

#include <sutk/UIDriver.hpp> // for UIDriver::ImageReference
#include <sutk/RenderableContainer.hpp>

namespace SUTK
{
	class RenderImage;

	class SUTK_API ImageContainer : public RenderableContainer
	{
	private:
		RenderImage* m_renderImage;
	public:
		ImageContainer(UIDriver& driver, Container* parent, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer) noexcept;
		~ImageContainer() noexcept;

		void setImage(UIDriver::ImageReference image) noexcept;
		RenderImage* getRenderImage() noexcept { return m_renderImage; }
	};
}