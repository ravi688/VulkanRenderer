#include <sutk/Geometry.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::compileGeometry()
#include <sutk/assert.h>

namespace SUTK
{
	Geometry::Geometry(UIDriver& driver, RenderMode renderMode) noexcept : UIDriverObject(driver), 
													m_positionArrayInfo({ VertexPositionArray { }, false }),
													m_texcoordArrayInfo({ VertexTexCoordArray { }, false }),
													m_indexArrayInfo({ VertexIndexArray { }, false }),
													m_transformArrayInfo({ InstanceTransformArray { }, false }),
													m_strokeInfo({ }),
													m_fillColorInfo({Color4::white(), false }),
													m_fillImageInfo({UIDriver::InvalidImage, false }),
													m_topology(Topology::TriangleList),
													m_isArray(false),
													m_isFillImage(false),
													m_sdf(NULL),
													m_renderMode(renderMode)
	{

	}

	Geometry::~Geometry() noexcept
	{
		if(m_sdf != NULL)
		{
			delete m_sdf;
			m_sdf = NULL;
		}
	}

	GfxDriverObjectHandleType Geometry::compile(GfxDriverObjectHandleType previous) noexcept
	{
		auto& gfxDriver = getGfxDriver();
		auto handle = gfxDriver.compileGeometry(*this, previous);
		resetModificationFlags();
		return handle;
	}

	void Geometry::resetModificationFlags() noexcept
	{
		if(m_strokeInfo.has_value())
			m_strokeInfo->isModified = false;
		m_positionArrayInfo.isModified = false;
		m_indexArrayInfo.isModified = false;
		m_transformArrayInfo.isModified = false;
		m_fillColorInfo.isModified = false;
		m_fillImageInfo.isModified = false;
		if(m_sdf != NULL)
			m_sdf->resetModificationFlags();
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

	Geometry& Geometry::vertexTexCoord(VertexTexCoord texcoord) noexcept
	{
		getVertexTexCoordArrayForWrite().push_back(texcoord);
		return *this;
	}

	Geometry& Geometry::instanceTransformArray(u32 transformCount) noexcept
	{
		auto& array = m_transformArrayInfo.array;
		
		// ensure the requested size
		array.reserve(transformCount);

		// if lesser number of positions are requested, then we can decrease
		// the size of position array from the end till its size becomes
		// exactly as 'transformCount'
		if(transformCount < array.size())
		{
			array.erase(array.begin() + transformCount, array.end());
			_assert(array.size() == transformCount);
		}

		// if the number of already existing positions are lesser then
		// add more elements at the end until the its size becomes exactly
		// as 'transformCount'
		while(array.size() < transformCount)
			array.push_back({ });

		m_transformArrayInfo.isModified = true;

		return *this;
	}

	Geometry& Geometry::instanceTransform(InstanceTransform transform) noexcept
	{
		getInstanceTransformArrayForWrite().push_back(transform);
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

	Geometry& Geometry::fillColor(Color4 color) noexcept
	{
		m_fillColorInfo.color = color;
		m_fillColorInfo.isModified = true;
		return *this;
	}

	Geometry& Geometry::fillImage(UIDriver::ImageReference image) noexcept
	{
		m_fillImageInfo.image = image;
		m_fillImageInfo.isModified = true;
		if(!m_isFillImage) m_isFillImage = true;
		return *this;
	}
}