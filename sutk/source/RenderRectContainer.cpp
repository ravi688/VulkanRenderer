#include <sutk/RenderRectContainer.hpp>
#include <sutk/RenderRect.hpp> // for SUTK::RenderRect::onContainerResize callback

namespace SUTK
{
	void RenderRectContainer::onAdd(Container* parent)
	{
		Container::onAdd(parent);
	}
	
	void RenderRectContainer::setRenderRect(RenderRect* renderRect)
	{
		m_renderRect = renderRect;
	}

	void RenderRectContainer::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// It is mandatory to call Container::onResize in any of the derived class overriding the function.
		Container::onResize(newRect, isPositionChanged, isSizeChanged);

		if(m_renderRect != NULL)
			m_renderRect->onContainerResize(newRect, isPositionChanged, isSizeChanged);
	}
}