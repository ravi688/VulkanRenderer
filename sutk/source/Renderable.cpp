#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer::setRenderable()

namespace SUTK
{
	Renderable::Renderable(UIDriver& driver, RenderableContainer* container) noexcept : UIDriverObject(driver), m_container(container)
	{
		if(container != NULL)
			container->setRenderable(this);
	}
}