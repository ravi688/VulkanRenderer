#pragma once

#include <sutk/defines.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriverObject

#include <vector> 	// for std::vector
#include <optional> // for std::optional

namespace SUTK
{
	class Geometry : public UIDriverObject
	{
	public:
		typedef Vec2D<f32> VertexPosition;
		typedef std::vector<VertexPosition> VertexPositionArray;
		struct VertexPositionArrayInfo
		{
			VertexPositionArray array;
			bool is_modified;
		};
		struct LineStroke
		{
			f32 width;
			Color3 color;

			constexpr LineStroke() noexcept : width(2.0f), color({ 255, 255, 255 }) { }
			constexpr LineStroke(f32 _width, Color3 _color = Color3::white()) noexcept : width(_width), color(_color) { }
		};
		struct LineStrokeInfo
		{
			LineStroke stroke;
			bool isStrokeWidthDynamic;
			bool is_modified;
		};
	private:
		VertexPositionArrayInfo m_positionArrayInfo;
		std::optional<LineStrokeInfo> m_strokeInfo;
	public:
		Geometry(UIDriver& driver) noexcept;

		GfxDriverObjectHandleType compile(GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		VertexPositionArray& getVertexPositionArray() { return m_positionArrayInfo.array; }

		Geometry& setLineStrokeDynamic(bool isDynamic = true) noexcept;

		Geometry& vertexPositionArray(u32 positionCount) noexcept;
		Geometry& vertexPosition(VertexPosition position) noexcept;
		Geometry& lineStroke(LineStroke stroke) noexcept;
		Geometry& lineStroke(float width, Color3 color = Color3::white()) noexcept
		{
			return lineStroke({ width, color });
		}
	};
}