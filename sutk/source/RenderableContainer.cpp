#include <sutk/RenderableContainer.hpp>
#include <sutk/Renderable.hpp> // for SUTK::Renderable::onContainerResize callback

namespace SUTK
{
	RenderableContainer::RenderableContainer(UIDriver& driver, Container* parent, bool isLayoutIgnore, Layer layer) noexcept : Container(driver, parent, isLayoutIgnore, layer), m_renderable(NULL)
	{

	}
	void RenderableContainer::onAdd(Container* parent)
	{
		Container::onAdd(parent);
	}
	
	void RenderableContainer::setRenderable(Renderable* renderable) noexcept
	{
		m_renderable = renderable;
	}

	void RenderableContainer::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// mandatory
		Container::onParentResize(newRect, isPositionChanged, isSizeChanged);

		if((m_renderable != NULL) && isPositionChanged)
			m_renderable->onGlobalCoordDirty();
	}

	void RenderableContainer::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// It is mandatory to call Container::onResize in any of the derived class overriding the function.
		Container::onResize(newRect, isPositionChanged, isSizeChanged);

		if(m_renderable != NULL)
			m_renderable->onContainerResize(newRect, isPositionChanged, isSizeChanged);
	}
}