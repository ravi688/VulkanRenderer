#include <sutk/RenderRect.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderableContainer.hpp>

#include <sutk/assert.h>

namespace SUTK
{
	RenderRect::RenderRect(UIDriver& driver, RenderableContainer* container, RenderMode renderMode) noexcept : GeometryRenderable(driver, container, renderMode), 
																						m_rect { 0, 0, 2.0f, 2.0f }, 
																						m_color(Color4::white()), 
																						m_isPosDirty(true),
																						m_isSizeDirty(true),
																						m_isColorDirty(true),
																						m_isGeometryDirty(false)
	{

	}

	RenderRect::~RenderRect() noexcept
	{
		if(getGfxDriverObjectHandle() != GFX_DRIVER_OBJECT_NULL_HANDLE)
			getUIDriver().getGfxDriver().destroyGeometry(getGfxDriverObjectHandle());
	}

	bool RenderRect::isDirty() noexcept
	{
		return GeometryRenderable::isDirty() || isRectDirty() || m_isColorDirty || m_isGeometryDirty;
	}

	void RenderRect::update() noexcept
	{
		// Mandatory to be called in the overriding method
		GeometryRenderable::update();
		if(m_isPosDirty)
		{
			// auto& gfxDriver = getGfxDriver();
			// GfxDriverObjectHandleType objHandle = gfxDriver.getGeometryObject(m_handle);
			// auto position = getContainer()->getLocalCoordsToScreenCoords(getRect().getPosition());
			// gfxDriver.setObjectPositionXY(objHandle, position);
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
	}

	void RenderRect::updateNormalizedDrawOrder(f32 normalizedDrawOrder) noexcept
	{
		// mandatory to be called in the overriding function
		GeometryRenderable::updateNormalizedDrawOrder(normalizedDrawOrder);

		GfxDriverObjectHandleType handle = getGfxDriverObjectHandle();
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		handle = getGfxDriver().getGeometryObject(handle);
		getGfxDriver().setObjectDepth(handle, normalizedDrawOrder);
	}

	void RenderRect::onActiveUpdate(com::Bool isActive) noexcept
	{
		// Activation and Deactivation Updates must happen immediately
		if(auto handle = getGfxDriverObjectHandle(); handle != GFX_DRIVER_OBJECT_NULL_HANDLE)
		{
			GfxDriverObjectHandleType obj = getGfxDriver().getGeometryObject(handle);
			getGfxDriver().setObjectActive(obj, isActive);
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

 	void RenderRect::setRect(const Rect2Df& rect) noexcept 
 	{ 
 		m_rect = rect; 
 		m_isPosDirty = true;
 		m_isSizeDirty = true;
 	}
}