#pragma once

#include <sutk/defines.hpp> /* for Vec2D, DisplaySizeType, GfxDriverObjectHandleType etc. */

namespace SUTK
{
	class UIDriver;

	class IGfxDriver
	{
	public:
		virtual ~IGfxDriver() = default;
		virtual void render(UIDriver& uiDriver) = 0;
		virtual Vec2D<DisplaySizeType> getSize() = 0;
		virtual GfxDriverObjectHandleType createText() = 0;
		virtual void destroyText(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec2D<DisplaySizeType> position) = 0;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) = 0;
		virtual u32 getBaselineHeightInPixels() = 0;
	};
}
