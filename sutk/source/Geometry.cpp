#include <sutk/Geometry.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::compileGeometry()
#include <sutk/assert.h>

namespace SUTK
{
	Geometry::Geometry(UIDriver& driver) noexcept : UIDriverObject(driver), 
													m_positionArrayInfo({ VertexPositionArray { }, false }),
													m_indexArrayInfo({ VertexIndexArray { }, false }),
													m_strokeInfo({ }),
													m_fillColorInfo({Color3::white(), false }),
													m_topology(Topology::TriangleList)
	{

	}

	GfxDriverObjectHandleType Geometry::compile(GfxDriverObjectHandleType previous) noexcept
	{
		auto& gfxDriver = getGfxDriver();
		auto handle = gfxDriver.compileGeometry(*this, previous);
		if(m_strokeInfo.has_value())
			m_strokeInfo->isModified = false;
		m_positionArrayInfo.isModified = false;
		m_indexArrayInfo.isModified = false;
		m_fillColorInfo.isModified = false;
		return handle;
	}

	Geometry& Geometry::topology(Topology topology) noexcept
	{
		m_topology = topology;
		return *this;
	}

	Geometry& Geometry::line(VertexIndex p1, VertexIndex p2) noexcept
	{
		m_indexArrayInfo.array.push_back(p1);
		m_indexArrayInfo.array.push_back(p2);
		m_indexArrayInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::triangle(VertexIndex p1, VertexIndex p2, VertexIndex p3) noexcept
	{
		m_indexArrayInfo.array.push_back(p1);
		m_indexArrayInfo.array.push_back(p2);
		m_indexArrayInfo.array.push_back(p3);
		m_indexArrayInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::quad(VertexIndex p1, VertexIndex p2, VertexIndex p3, VertexIndex p4) noexcept
	{
		/*	
			Anti-clockwise winding

			 p1 <----------- p4
			  |  		     |
			  |				 |
			  |				 |
			  p2 -----------p3

		*/
		triangle(p1, p2, p3);
		triangle(p3, p4, p1);
		return *this;
	}

	Geometry& Geometry::nextLine(VertexIndex p) noexcept
	{
		m_indexArrayInfo.array.push_back(p);
		m_indexArrayInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::nextTriangle(VertexIndex p) noexcept
	{
		m_indexArrayInfo.array.push_back(p);
		m_indexArrayInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::vertexPositionArray(u32 positionCount) noexcept
	{
		auto& array = m_positionArrayInfo.array;
		
		// ensure the requested size
		array.reserve(positionCount);

		// if lesser number of positions are requested, then we can decrease
		// the size of position array from the end till its size becomes
		// exactly as 'positionCount'
		if(positionCount < array.size())
		{
			array.erase(array.begin() + positionCount, array.end());
			_assert(array.size() == positionCount);
		}

		// if the number of already existing positions are lesser then
		// add more elements at the end until the its size becomes exactly
		// as 'positionCount'
		while(array.size() < positionCount)
			array.push_back({ });

		m_positionArrayInfo.isModified = true;

		return *this;
	}

	Geometry& Geometry::vertexPositionArray(const std::vector<VertexPosition>& positions) noexcept
	{
		auto& array = m_positionArrayInfo.array;
		for(std::size_t i = 0; i < positions.size(); ++i)
			array.push_back(positions[i]);
		m_positionArrayInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::vertexPosition(VertexPosition position) noexcept
	{
		getVertexPositionArrayForWrite().push_back(position);
		return *this;
	}

	Geometry& Geometry::lineStroke(LineStroke stroke, bool isDynamic) noexcept
	{
		if(!m_strokeInfo.has_value())
			m_strokeInfo = { stroke, false };
		else
			m_strokeInfo->stroke = stroke;
		
		m_strokeInfo->isStrokeWidthDynamic = isDynamic;

		m_strokeInfo->isModified = true;

		return *this;
	}

	Geometry& Geometry::fillColor(Color3 color) noexcept
	{
		m_fillColorInfo.color = color;
		m_fillColorInfo.isModified = true;
		return *this;
	}
}