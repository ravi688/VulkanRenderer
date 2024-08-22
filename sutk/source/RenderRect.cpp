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
																						m_isColorDirty(true)
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
		return isRectDirty() || m_isColorDirty || m_isActiveDirty;
	}

	void RenderRect::update() noexcept
	{
		if(m_isActiveDirty)
		{
			auto handle = getGfxDriverObjectHandle();
			_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
			auto obj = getGfxDriver().getGeometryObject(handle);
			getGfxDriver().setObjectActive(obj, isActive());
			m_isActiveDirty = false;
		}

		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPosition(objHandle, position);
		}

		if(m_isColorDirty)
			getGeometry().fillColor(getColor());

		if(m_isPosDirty || m_isSizeDirty || m_isColorDirty)
		{
			GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
			handle = getGeometry().compile(handle);
			setGfxDriverObjectHandle(handle);
			m_isPosDirty = false;
			m_isSizeDirty = false;
			m_isColorDirty = false;
		}
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