#pragma once

#include <sutk/ButtonGraphic.hpp> // for SUTK::DefaultButtonGraphicNoLabel

namespace SUTK
{
	class Label;
	class ImageButtonGraphic;
	class Button;

	class SUTK_API TabButtonGraphic : public DefaultButtonGraphicNoLabel
	{
	private:		
		Label* m_label;
		Button* m_closeButton;
		ImageButtonGraphic* m_closeButtonGraphic;
		Color4 m_hoverColor;
		Color4 m_pressColor;
		Color4 m_idleColor;
		static UIDriver::ImageReference s_closeIcon;

	public:
		TabButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
		virtual ~TabButtonGraphic() noexcept;

		// Overrides
		virtual Vec2Df getMinBoundSize() noexcept override;

		Button* getCloseButton() noexcept { return m_closeButton; }
		Label& getLabel() noexcept { return *m_label; }

		void selectedState() noexcept;
		void unselectedState() noexcept;
	};
}
