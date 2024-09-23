#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>

#include <sutk/assert.h>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderableContainer* container) noexcept : GeometryRenderable(driver, container), 
																						m_rect { 0, 0, 2.0f, 2.0f }, 
																						m_color(Color4::white()), 
																						m_isActiveDirty(false),
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_isColorDirty(true),
																						m_isGeometryDirty(false)
	{

	}

	void RenderRect::setActive(bool isActive) noexcept
	{
		// mandatory to call
		GeometryRenderable::setActive(isActive);
		m_isActiveDirty = true;
	}

	bool RenderRect::isDirty() noexcept
	{
		// See: SmallText.cpp:isDirty() for more understanding of why isActive() is here.
		return m_isActiveDirty || ((isRectDirty() || m_isColorDirty || m_isGeometryDirty) && isActive());
	}

	void RenderRect::update() noexcept
	{
		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isColorDirty)
			getGeometry().fillColor(getColor());

		GfxDriverObjectHandleType handle = GFX_DRIVER_OBJECT_NULL_HANDLE;
		if(m_isPosDirty || m_isSizeDirty || m_isColorDirty || m_isGeometryDirty)
		{
			handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
			m_isColorDirty = false;
			m_isGeometryDirty = false;
		}

		if(m_isActiveDirty)
		{
			if(handle == GFX_DRIVER_OBJECT_NULL_HANDLE)
			{
				handle = getGfxDriverObjectHandle();
				_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
			}
			GfxDriverObjectHandleType obj = getGfxDriver().getGeometryObject(handle);
			getGfxDriver().setObjectActive(obj, isActive());
			m_isActiveDirty = false;
		}
	}

	void RenderRect::updateNormalizedDrawOrder(f32 normalizedDrawOrder) noexcept
	{
		// mandatory to be called in the overriding function
		Renderable::updateNormalizedDrawOrder(normalizedDrawOrder);

		GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		handle = getGfxDriver().getGeometryObject(handle);
		getGfxDriver().setObjectDepth(handle, normalizedDrawOrder);
	}

	void RenderRect::setColor(Color4 color) noexcept
	{
		m_color = color;
		m_isColorDirty = true;
	}

	void RenderRect::onGlobalCoordDirty() noexcept
	{
		m_isPosDirty = true;
	}

	void RenderRect::onContainerResize(Rect2Df rect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		if(isSizeChanged)
		{
			// The rect for this RenderRect is in local coordinates of the RenderableContainer for this RenderRect.
			// Therefore, the position would always be { 0, 0 } for this rect.
			rect.setPosition({ 0, 0 });
			setRect(rect);
		}
		m_isPosDirty = isPositionChanged;
		m_isSizeDirty = isSizeChanged;
	}
}