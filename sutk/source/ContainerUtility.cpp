#include <sutk/ContainerUtility.hpp>

#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/IColorable.hpp>

namespace SUTK
{
	void ContainerUtility::RenderableSetActive(Container* container, bool isActive) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			renderable->setActive(isActive);
		}
	}
	void ContainerUtility::SetActiveAllRecursive(Container* container, bool isActive) noexcept
	{
		if(container->isRecyclable() && (container->getRecycleState() == Container::RecycleState::Disposed))
			return;
		container->setActive(isActive);
		RenderableSetActive(container, isActive);
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			SetActiveAllRecursive(child, isActive);			
	}

	void ContainerUtility::ContainerSetActiveRecursive(Container* container, bool isActive) noexcept
	{
		container->setActive(isActive);
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			ContainerSetActiveRecursive(child, isActive);
	}

	void ContainerUtility::RenderablesSetActive(Container* container, bool isActive) noexcept
	{
		RenderableSetActive(container, isActive);
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
			if(colorable)
			{
				Color4 color = colorable->getColor();
				color.alpha(alpha * 255);
				colorable->setColor(color);
			}
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesSetAlpha(child, alpha);
	}

	void ContainerUtility::RenderablesVisit(Container* container, const std::function<void(Renderable*)>& visitor) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			visitor(renderable);
		}
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			RenderablesVisit(child, visitor);
	}
}