#pragma once

#include <sutk/defines.hpp> /* for Vec2D, Rect2D, DisplaySizeType, GfxDriverObjectHandleType etc. */

#include <functional> /* for std::function */

namespace SUTK
{
	class UIDriver;

	class Geometry;

	struct ColorRange
	{
		// 4-Byte
		LineCountType begin;
		// 4-Byte
		LineCountType end;
		// 4-Byte
		Color4 color;
	};

	typedef std::vector<ColorRange> ColorRangeList;

	class IGfxDriver
	{
	public:
		virtual ~IGfxDriver() = default;
		virtual void render(UIDriver& uiDriver) = 0;
		virtual Vec2D<DisplaySizeType> getSizeInPixels() = 0;
		virtual Vec2Df getSizeInCentimeters() = 0;
		virtual GfxDriverObjectHandleType createTextGroup() = 0;
		virtual void destroyTextGroup(GfxDriverObjectHandleType textGroup) = 0;
		virtual GfxDriverObjectHandleType createText(GfxDriverObjectHandleType textGroup) = 0;
		virtual void destroyText(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec2Df position) = 0;
		virtual void setTextPointSize(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		virtual f32 getTextPointSize(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextColor(GfxDriverObjectHandleType handle, const Color4 color) = 0;
		virtual void setTextColorRanges(GfxDriverObjectHandleType handle, const ColorRange* ranges, u32 rangeCount) = 0;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) = 0;
		virtual LineCountType getTextGlyphIndexFromCoord(GfxDriverObjectHandleType handle, f32 coord) = 0;
		virtual f32 getTextCoordFromGlyphIndex(GfxDriverObjectHandleType handle, LineCountType col) = 0;
		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) = 0;

		virtual GfxDriverObjectHandleType getObject(GfxDriverObjectHandleType handle) = 0;
		virtual void setObjectActive(GfxDriverObjectHandleType handle, bool isActive) = 0;
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

		virtual u32 getTextBaselineHeightInPixels(GfxDriverObjectHandleType handle) = 0;
		virtual f32 getTextBaselineHeightInCentimeters(GfxDriverObjectHandleType handle) = 0;
		// the Vec2Df arguments is in centimeters (not pixels!)
		typedef std::function<void(Vec2Df)> OnResizeCallbackHandler;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) = 0;
		virtual void removeOnResizeHandler(u32 id) = 0;
	};
}
