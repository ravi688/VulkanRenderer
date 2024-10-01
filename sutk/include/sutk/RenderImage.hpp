#pragma once

#include <sutk/RenderRect.hpp>

namespace SUTK
{
	class RenderImage : public RenderRect
	{
	public:
		struct AspectRatio
		{
			// width
			u32 nominator;
			// height
			u32 denominator;
		};
	private:
		AspectRatio m_aspectRatio;
		bool m_isPreserveAspectRatio;

		Rect2Df calculateImageRectAR(Rect2Df rect) noexcept;
		void updateImageRect(Rect2Df rect) noexcept;
		void updateImageRect() noexcept;

	protected:
		virtual void onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept override;
	public:
		// Constructors
		RenderImage(UIDriver& driver, RenderableContainer* container, bool isUpdate = true) noexcept;

		// Override of RenderRect::update()
		virtual void update() noexcept override;

		void setImage(UIDriver::ImageReference image) noexcept;
		void setAspectRatio(AspectRatio aspectRatio) noexcept;
		void setPreserveAspectRatio(bool isPreserve) noexcept;
	};
}
