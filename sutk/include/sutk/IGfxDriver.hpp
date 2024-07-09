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
		// NOTE	: One VkBuffer/VkDeviceMemory object per one Text instance
		virtual GfxDriverObjectHandleType createText() = 0;
		virtual void destroyText(GfxDriverObjectHandleType handle) = 0;
		virtual GfxDriverObjectHandleType createSubtext(GfxDriverObjectHandleType textHandle) = 0;
		virtual void destroySubText(GfxDriverObjectHandleType subTextHandle) = 0;
	};
}
