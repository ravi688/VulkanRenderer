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
			bool isModified;
		};
		typedef u32 VertexIndex;
		typedef std::vector<VertexIndex> VertexIndexArray;
		struct VertexIndexArrayInfo
		{
			VertexIndexArray array;
			bool isModified;
		};
		// Rect2D<f32>::size : this value is multiple by position vertex attribute
		// Rect2D<f32>::position : this value is added to the above result
		typedef Rect2D<f32> InstanceTransform;
		typedef std::vector<InstanceTransform> InstanceTransformArray;
		struct InstanceTransformArrayInfo
		{
			InstanceTransformArray array;
			bool isModified;
		};
		struct LineStroke
		{
			f32 width;
			Color4 color;

			constexpr LineStroke() noexcept : width(2.0f), color({ 255, 255, 255 }) { }
			constexpr LineStroke(f32 _width, Color4 _color = Color4::white()) noexcept : width(_width), color(_color) { }
		};
		struct LineStrokeInfo
		{
			LineStroke stroke;
			bool isStrokeWidthDynamic;
			bool isModified;
		};
		enum class Topology
		{
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip
		};
		struct FillColorInfo
		{
			Color4 color;
			bool isModified;
		};
	private:
		VertexPositionArrayInfo m_positionArrayInfo;
		VertexIndexArrayInfo m_indexArrayInfo;
		InstanceTransformArrayInfo m_transformArrayInfo;
		std::optional<LineStrokeInfo> m_strokeInfo;
		FillColorInfo m_fillColorInfo;
		Topology m_topology;
	public:
		Geometry(UIDriver& driver) noexcept;

		GfxDriverObjectHandleType compile(GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

		Geometry& topology(Topology topology) noexcept;

		Geometry& line(VertexIndex p1, VertexIndex p2) noexcept;
		Geometry& triangle(VertexIndex p1, VertexIndex p2, VertexIndex p3) noexcept;
		Geometry& quad(VertexIndex p1, VertexIndex p2, VertexIndex p3, VertexIndex p4) noexcept;
		Geometry& nextLine(VertexIndex p) noexcept;
		Geometry& nextTriangle(VertexIndex p) noexcept;

		Geometry& vertexPositionArray(u32 positionCount) noexcept;
		Geometry& vertexPositionArray(const std::vector<VertexPosition>& positions) noexcept;
		Geometry& vertexPosition(VertexPosition position) noexcept;

		Geometry& instanceTransformArray(u32 transformCount) noexcept;
		Geometry& instanceTransform(InstanceTransform transform) noexcept;

		Geometry& lineStroke(LineStroke stroke, bool isDynamic = false) noexcept;
		Geometry& lineStroke(float width, Color4 color = Color4::white(), bool isDynamic = false) noexcept
		{
			return lineStroke({ width, color }, isDynamic);
		}
		Geometry& fillColor(Color4 color) noexcept;

		Geometry& array(u32 count) noexcept;

		// getters

		Topology getTopology() const noexcept { return m_topology; }

		VertexPositionArray& getVertexPositionArrayForWrite() { m_positionArrayInfo.isModified = true; return m_positionArrayInfo.array; }
		const VertexPositionArray& getVertexPositionArray() const { return m_positionArrayInfo.array; }
		VertexIndexArray& getVertexIndexArrayForWrite() { m_indexArrayInfo.isModified = true; return m_indexArrayInfo.array; }
		const VertexIndexArray& getVertexIndexArray() const { return m_indexArrayInfo.array; }
		InstanceTransformArray& getInstanceTransformArrayForWrite() { m_transformArrayInfo.isModified = true; return m_transformArrayInfo.array; }
		const InstanceTransformArray& getInstanceTransformArray() const { return m_transformArrayInfo.array; }

		Color4 getFillColor() const { return m_fillColorInfo.color; }

		bool isVertexIndexArrayModified() const noexcept { return m_indexArrayInfo.isModified; }
		bool isVertexPositionArrayModified() const noexcept { return m_positionArrayInfo.isModified; }
		bool isInstanceTransformArrayModified() const noexcept { return m_transformArrayInfo.isModified; }
		bool isLineStrokeModified() const noexcept { return m_strokeInfo.has_value() && m_strokeInfo->isModified; }
		bool isFillColorModified() const noexcept { return m_fillColorInfo.isModified; }

		bool isModified() const noexcept
		{
			return isVertexIndexArrayModified() ||
					isVertexPositionArrayModified() || 
					isInstanceTransformArrayModified() ||
					isLineStrokeModified() ||
					isFillColorModified();
		}
	};
}