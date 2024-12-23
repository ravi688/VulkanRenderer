#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer::setRenderable()
#include <sutk/IGfxDriver.hpp>

namespace SUTK
{
	Renderable::Renderable(UIDriver& driver, RenderableContainer* container) noexcept : UIDriverObject(driver), m_container(container), m_drawOrder(0), m_isDrawOrderDirty(true), m_isRedraw(com::False)
	{
		if(container != NULL)
		{
			// NOTE: We can't call RenderableContainer::getRenderable() here because it is virtual, and can be overriden
			// And in that override, one may instantiate this Renderable() which will lead to non-terminating recursion!
			com_assert(COM_DESCRIPTION(!container->m_renderable), "Only one Renderable is allowed per RenderableContainer");
			container->setRenderable(this);
		}
		driver.addRenderable(this);

		setDrawOrder((container == NULL) ? 0 : container->getDepth());
	}

	Renderable::~Renderable() noexcept
	{
		if(getContainer())
		{
			getContainer()->setRenderable(com::null_pointer<Renderable>());
			m_container = com::null_pointer<RenderableContainer>();
		}
		getUIDriver().removeRenderable(this);
	}

	void Renderable::onAnscestorChange(Container* anscestor) noexcept
	{
		std::cout << "Draw Order Set: " << getContainer()->getDepth() << std::endl;
		setDrawOrder(getContainer()->getDepth());
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