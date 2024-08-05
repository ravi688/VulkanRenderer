#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer::setRenderable()
#include <sutk/IGfxDriver.hpp>

namespace SUTK
{
	Renderable::Renderable(UIDriver& driver, RenderableContainer* container) noexcept : UIDriverObject(driver), m_container(container)
	{
		if(container != NULL)
			container->setRenderable(this);
		driver.m_renderables.push_back(this);
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