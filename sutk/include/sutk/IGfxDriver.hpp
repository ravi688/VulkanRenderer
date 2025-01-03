#pragma once

#include <sutk/defines.hpp> /* for Vec2D, Rect2D, DisplaySizeType, GfxDriverObjectHandleType etc. */

#include <functional> /* for std::function */
#include <span> // for std::span<>

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

	struct TextureAttributes
	{
		u32 width;
		u32 height;
		u32 depth;
		u8 channelCount;
	};

	typedef std::vector<ColorRange> ColorRangeList;

	enum class RenderMode : u8
	{
		Opaque,
		Transparent
	};

	class IGfxDriver
	{
	public:
		virtual ~IGfxDriver() = default;
		virtual void render(UIDriver& uiDriver) = 0;
		virtual u32 getNativeWindowHandleSize() = 0;
		virtual void getNativeWindowHandle(void* const out) = 0;
		virtual Vec2D<DisplaySizeType> getSizeInPixels() = 0;
		virtual Vec2Df getSizeInCentimeters() = 0;
		virtual GfxDriverObjectHandleType createTextGroup(RenderMode renderMode = RenderMode::Transparent) = 0;
		virtual void setTextGroupFont(GfxDriverObjectHandleType textGroup, GfxDriverObjectHandleType font) = 0;
		virtual void setTextGroupDepth(GfxDriverObjectHandleType textGroup, f32 depth) = 0;
		virtual void destroyTextGroup(GfxDriverObjectHandleType textGroup) = 0;
		virtual GfxDriverObjectHandleType createText(GfxDriverObjectHandleType textGroup) = 0;
		virtual void destroyText(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec3Df position) = 0;
		virtual void setTextDepth(GfxDriverObjectHandleType handle, f32 depth) = 0;
		virtual void setTextPointSize(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		// NOTE: IGfxDriver doesn't provide any getters for font of a text,
		// Any IGfxDriver implementaion is free to choose any font when creating a text and won't provide any getters for it.
		// Thus, the font used while creation of the text is implementation-defined and can't be queried.
		// To make it defined, it is required to call IGfxDriver::setFont(). 
		// NOTE: It is allowed to pass GFX_DRIVER_OBJECT_NULL_HANDLE in IGfxDriver::setFont(),
		// In that case, the implementation would use the "implementation defined" font which would remain uniform across all functions of IGfxDriver
		virtual void setTextFont(GfxDriverObjectHandleType handle, GfxDriverObjectHandleType font) = 0;
		virtual f32 getTextPointSize(GfxDriverObjectHandleType handle) = 0;
		virtual void setTextColor(GfxDriverObjectHandleType handle, const Color4 color) = 0;
		virtual void setTextColorRanges(GfxDriverObjectHandleType handle, const ColorRange* ranges, u32 rangeCount) = 0;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) = 0;
		virtual void setTextActive(GfxDriverObjectHandleType handle, com::Bool isActive) = 0;
		virtual LineCountType getTextGlyphIndexFromCoord(GfxDriverObjectHandleType handle, f32 coord) = 0;
		virtual f32 getTextCoordFromGlyphIndex(GfxDriverObjectHandleType handle, LineCountType col) = 0;
		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) = 0;

		virtual GfxDriverObjectHandleType getObject(GfxDriverObjectHandleType handle) = 0;
		virtual void setObjectActive(GfxDriverObjectHandleType handle, com::Bool isActive) = 0;
		// rect should be in centimeters
		virtual void setObjectScissor(GfxDriverObjectHandleType handle, const Rect2Df rect) = 0;
		// position should be centimeters
		virtual void setObjectPositionXY(GfxDriverObjectHandleType handle, const Vec2Df position) = 0;
		virtual void setObjectDepth(GfxDriverObjectHandleType handle, f32 depth) = 0;
		//.rect should be in centimeters
		void setTextScissor(GfxDriverObjectHandleType handle, const Rect2Df rect) noexcept
		{
			GfxDriverObjectHandleType objHandle = getTextObject(handle);
			setObjectScissor(objHandle, rect);
		}
		virtual GfxDriverObjectHandleType loadTexture(std::string_view str) = 0;
		virtual GfxDriverObjectHandleType loadTexture(const std::span<const u8> data) = 0;
		virtual GfxDriverObjectHandleType loadTexture(const u8* pixels, u32 width, u32 height, u32 numChannels) = 0;
		virtual void getTextureAttributes(GfxDriverObjectHandleType texture, TextureAttributes& out) = 0;
		virtual void unloadTexture(GfxDriverObjectHandleType handle) = 0;
		virtual GfxDriverObjectHandleType loadFont(std::string_view str) = 0;
		virtual GfxDriverObjectHandleType loadFont(std::span<const std::byte> bytes) = 0;
		virtual void unloadFont(GfxDriverObjectHandleType handle) = 0;
		virtual GfxDriverObjectHandleType compileGeometry(const Geometry& geometryDsc, GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) = 0;
		virtual void destroyGeometry(GfxDriverObjectHandleType geometry) = 0;
		virtual GfxDriverObjectHandleType getGeometryObject(GfxDriverObjectHandleType geometry) = 0;

		virtual u32 getTextBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		virtual f32 getTextBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		virtual u32 getTextGroupBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		virtual f32 getTextGroupBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize) = 0;
		// the Vec2Df arguments is in centimeters (not pixels!)
		typedef std::function<void(Vec2Df)> OnResizeCallbackHandler;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) = 0;
		virtual void removeOnResizeHandler(u32 id) = 0;
		typedef std::function<void()> OnCloseCallbackHandler;
		virtual u32 addOnCloseHandler(OnCloseCallbackHandler handler) = 0;
		virtual void removeOnCloseHandler(u32 id) = 0;
		virtual void setClose(bool isClose) = 0;
	};
}
