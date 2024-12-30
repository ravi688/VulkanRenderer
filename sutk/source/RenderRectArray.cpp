#include <sutk/RenderRectArray.hpp>
#include <sutk/RenderableContainer.hpp>
#include <common/assert.h> // for _com_assert()

namespace SUTK
{
	RenderRectFillArray::RenderRectFillArray(UIDriver& driver, RenderableContainer* container, RenderMode renderMode) noexcept : GeometryRenderable(driver, container, renderMode),
																						m_color(Color4::white()),
																						m_isColorDirty(true),
																						m_isRectsDirty(false)
	{
		_com_assert(container != NULL);
		// unit rect (1 centimeter by 1 centimeter)
		Rect2Df rect = { 0, 0, 1.0f, 1.0f };
		getGeometry()
			.vertexPosition(rect.getTopLeft())
			.vertexPosition(rect.getBottomLeft())
			.vertexPosition(rect.getBottomRight())
			.vertexPosition(rect.getTopRight())
			.topology(Geometry::Topology::TriangleList)
			.quad(0, 1, 2, 3)
			.fillColor(getColor())
			.setArray(true);

		// call update for the first time as we have geometry description already and
		// this geometry is supposed to be displayed in the very first frame.
		update();
	}

	RenderRectFillArray::~RenderRectFillArray() noexcept
	{
		if(getGfxDriverObjectHandle() != GFX_DRIVER_OBJECT_NULL_HANDLE)
			getUIDriver().getGfxDriver().destroyGeometry(getGfxDriverObjectHandle());
	}

	bool RenderRectFillArray::isDirty()
	{
		return GeometryRenderable::isDirty() || m_isRectsDirty || m_isColorDirty;
	}

	void RenderRectFillArray::update()
	{
		// Mandatory to be called in the overriding method
		GeometryRenderable::update();
		if(m_isRectsDirty)
		{
			Geometry::InstanceTransformArray& transformArray = getGeometry().getInstanceTransformArrayForWrite();
			transformArray.clear();
			for(const Rect2Df& rect : m_rects)
			{
				Rect2Df _rect = getContainer()->getLocalCoordsToScreenCoords(rect);
				transformArray.push_back(_rect);
			}
		}
		if(m_isColorDirty)
		{
			getGeometry().fillColor(getColor());
		}

		if(m_isRectsDirty || m_isColorDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isRectsDirty = false;
			m_isColorDirty = false;
		}
	}

	std::vector<Rect2Df>& RenderRectFillArray::getRectsForWrite() noexcept
	{
		m_isRectsDirty = true;
		return m_rects;
	}

	void RenderRectFillArray::setColor(const Color4 color) noexcept
	{
		m_color = color;
		m_isColorDirty = true;
	}

	void RenderRectFillArray::onGlobalCoordDirty() noexcept
	{
		m_isRectsDirty = true;
	}

	void RenderRectFillArray::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		m_isRectsDirty = true;
	}

	void RenderRectFillArray::updateNormalizedDrawOrder(f32 normalizedDrawOrder) noexcept
	{
		// mandatory to be called in the overriding function
		GeometryRenderable::updateNormalizedDrawOrder(normalizedDrawOrder);

		GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		handle = getGfxDriver().getGeometryObject(handle);
		getGfxDriver().setObjectDepth(handle, normalizedDrawOrder);
	}

	void RenderRectFillArray::onActiveUpdate(com::Bool isActive) noexcept
	{
		auto handle = getGfxDriverObjectHandle();
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		auto obj = getGfxDriver().getGeometryObject(handle);
		getGfxDriver().setObjectActive(obj, isActive);
	}
}