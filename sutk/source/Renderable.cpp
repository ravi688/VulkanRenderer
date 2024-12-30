#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer::setRenderable()
#include <sutk/IGfxDriver.hpp>

namespace SUTK
{
	Renderable::Renderable(UIDriver& driver, RenderableContainer* container) noexcept : UIDriverObject(driver), m_container(container), m_drawOrder(0), m_isDrawOrderDirty(true)
	{
		if(container != NULL)
		{
			// NOTE: We can't call RenderableContainer::getRenderable() here because it is virtual, and can be overriden
			// And in that override, one may instantiate this Renderable() which will lead to non-terminating recursion!
			com_assert(COM_DESCRIPTION(!container->m_renderable), "Only one Renderable is allowed per RenderableContainer, destroy the previous one first!");
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
		setDrawOrder(getContainer()->getDepth());
	}

	void Renderable::setActive(com::Bool isActive) noexcept
	{
		if(Activatable::isActive() != isActive)
			m_isActiveDirty = com::True;
		Activatable::setActive(isActive);
		m_isDrawOrderDirty = true;
	}

	bool Renderable::isDirty()
	{
		return static_cast<bool>(m_isActiveDirty) || isActiveDirty();
	}

	void Renderable::update()
	{
		if(m_isActiveDirty)
			onActiveUpdate(isActive());
		m_isActiveDirty = com::False;
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