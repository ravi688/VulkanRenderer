#include <sutk/ContainerUtility.hpp>

#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/IColorable.hpp>

namespace SUTK
{
	void ContainerUtility::RenderablesSetActive(Container* container, bool isActive) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			renderable->setActive(isActive);
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesSetActive(child, isActive);
	}

	void ContainerUtility::RenderablesSetAlpha(Container* container, f32 alpha) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			IColorable* colorable = dynamic_cast<IColorable*>(renderable);
			Color4 color = colorable->getColor();
			color.alpha(alpha * 255);
			colorable->setColor(color);
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesSetAlpha(child, alpha);
	}
}