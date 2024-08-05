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
		virtual Vec2D<DisplaySizeType> getSizeInPixels() = 0;
		virtual Vec2Df getSizeInCentimeters() = 0;
		virtual GfxDriverObjectHandleType createText() = 0;
		virtual void destroyText(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec2Df position) = 0;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) = 0;
		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) = 0;

		virtual GfxDriverObjectHandleType getObject(GfxDriverObjectHandleType handle) = 0;
		// rect should be in centimeters
		virtual void setObjectScissor(GfxDriverObjectHandleType handle, const Rect2Df rect) = 0;
		// position should be centimeters
		virtual void setObjectPosition(GfxDriverObjectHandleType handle, const Vec2Df position) = 0;
		//.rect should be in centimeters
		void setTextScissor(GfxDriverObjectHandleType handle, const Rect2Df rect) noexcept
		{
			GfxDriverObjectHandleType objHandle = getTextObject(handle);
			setObjectScissor(objHandle, rect);
		}
		virtual GfxDriverObjectHandleType compileGeometry(const Geometry& geometryDsc, GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) = 0;
		virtual void destroyGeometry(GfxDriverObjectHandleType geometry) = 0;
		virtual GfxDriverObjectHandleType getGeometryObject(GfxDriverObjectHandleType geometry) = 0;

		virtual u32 getBaselineHeightInPixels() = 0;
		virtual f32 getBaselineHeightInCentimeters() = 0;
		// the Vec2Df arguments is in centimeters (not pixels!)
		typedef std::function<void(Vec2Df)> OnResizeCallbackHandler;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) = 0;
		virtual void removeOnResizeHandler(u32 id) = 0;
	};
}
