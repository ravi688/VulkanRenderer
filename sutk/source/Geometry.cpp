#include <sutk/Geometry.hpp>

#include <sutk/assert.h>

namespace SUTK
{
	Geometry::Geometry(UIDriver& driver) noexcept : UIDriverObject(driver), m_positionArrayInfo({ VertexPositionArray { }, false })
	{

	}

	GfxDriverObjectHandleType Geometry::compile(GfxDriverObjectHandleType previous) noexcept
	{
		if(m_strokeInfo.has_value())
			m_strokeInfo->is_modified = false;
		m_positionArrayInfo.is_modified = false;
		return GFX_DRIVER_OBJECT_NULL_HANDLE;
	}

	Geometry& Geometry::setLineStrokeDynamic(bool isDynamic) noexcept
	{
		// if no stroke information already exists then create a new info
		if(!m_strokeInfo.has_value())
			m_strokeInfo = LineStrokeInfo { };

		// update the info
		m_strokeInfo->isStrokeWidthDynamic = isDynamic;
		m_strokeInfo->is_modified = true;

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

		return *this;
	}

	Geometry& Geometry::vertexPosition(VertexPosition position) noexcept
	{
		getVertexPositionArray().push_back(position);
		return *this;
	}

	Geometry& Geometry::lineStroke(LineStroke stroke) noexcept
	{
		if(!m_strokeInfo.has_value())
			m_strokeInfo = { stroke, false };
		else
			m_strokeInfo->stroke = stroke;
		
		m_strokeInfo->is_modified = true;

		return *this;
	}
}