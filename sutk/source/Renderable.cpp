#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer::setRenderable()
#include <sutk/IGfxDriver.hpp>

namespace SUTK
{
	Renderable::Renderable(UIDriver& driver, RenderableContainer* container) noexcept : UIDriverObject(driver), m_container(container), m_drawOrder(0), m_isDrawOrderDirty(true)
	{
		if(container != NULL)
			container->setRenderable(this);
		driver.addRenderable(this);

		setDrawOrder((container == NULL) ? 0 : container->getDepth());
	}

	Renderable::~Renderable() noexcept
	{
		getUIDriver().removeRenderable(this);
	}

	void GfxDriverRenderable::setClipRectGlobalCoords(const Rect2Df rect) noexcept
	{
		auto handle = getGfxDriverObjectHandle();
		getGfxDriver().setObjectScissor(getGfxDriver().getObject(handle), rect);
	}

	void GfxDriverRenderable::setClipRect(const Rect2Df rect) noexcept
	{
		Container* container = getContainer();
		if(container->getParent() != NULL)
			container = container->getParent();
		// convert the top-left corner of the container, in which renderable is, to global coordinates
		auto position = container->getLocalCoordsToScreenCoords(rect.getPosition());
		setClipRectGlobalCoords({ position, rect.getSize() });
	}
}