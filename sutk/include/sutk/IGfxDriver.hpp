#pragma once

#include <sutk/defines.hpp> /* for Vec2D, Rect2D, DisplaySizeType, GfxDriverObjectHandleType etc. */

#include <functional> /* for std::function */

namespace SUTK
{
	class UIDriver;

	class Geometry;

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
		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) = 0;
		virtual void setObjectScissor(GfxDriverObjectHandleType handle, const Rect2D<DisplaySizeType> rect) = 0;
		virtual void setObjectPosition(GfxDriverObjectHandleType handle, const Vec2D<f32> position) = 0;
		void setTextScissor(GfxDriverObjectHandleType handle, const Rect2D<DisplaySizeType> rect) noexcept
		{
			GfxDriverObjectHandleType objHandle = getTextObject(handle);
			setObjectScissor(objHandle, rect);
		}
		virtual GfxDriverObjectHandleType compileGeometry(const Geometry& geometryDsc, GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) = 0;
		virtual void destroyGeometry(GfxDriverObjectHandleType geometry) = 0;
		virtual GfxDriverObjectHandleType getGeometryObject(GfxDriverObjectHandleType geometry) = 0;

		virtual u32 getBaselineHeightInPixels() = 0;
		typedef std::function<void(Vec2D<DisplaySizeType>)> OnResizeCallbackHandler;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) = 0;
		virtual void removeOnResizeHandler(u32 id) = 0;
	};
}
