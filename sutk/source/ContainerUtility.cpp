#include <sutk/ContainerUtility.hpp>

#include <sutk/Renderable.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/IColorable.hpp>
#include <sutk/InputEventHandlerObject.hpp> // for dynamic_cast to work
#include <sutk/Button.hpp>
#include <sutk/TextGroupContainer.hpp> // for dynamic_cast to work

namespace SUTK
{
	void ContainerUtility::RenderableSetActive(Container* container, com::Bool isActive) noexcept
	{
		auto renderCont = dynamic_cast<RenderableContainer*>(container);
		if(renderCont != NULL)
		{
			Renderable* renderable = renderCont->getRenderable();
			_com_assert(renderable != NULL);
			renderable->setActive(isActive);
		}
	}
	void ContainerUtility::SetActiveAllRecursive(Container* container, com::Bool isActive) noexcept
	{
		if(container->isRecyclable() && (container->getRecycleState() == Container::RecycleState::Disposed))
			return;
		container->setActive(isActive);
		RenderableSetActive(container, isActive);
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			SetActiveAllRecursive(child, isActive);			
	}

	void ContainerUtility::ContainerSetActiveRecursive(Container* container, com::Bool isActive) noexcept
	{
		container->setActive(isActive);
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			ContainerSetActiveRecursive(child, isActive);
	}

	void ContainerUtility::RenderablesSetActive(Container* container, com::Bool isActive) noexcept
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

	void ContainerUtility::ContainersWalkUpUntil(Container* container, const std::function<bool(Container*)>& visitor) noexcept
	{
		if(container == NULL) return;
		if(!visitor(container))
			return;
		ContainersWalkUpUntil(container->getParent(), visitor);
	}

	GfxDriverObjectHandleType ContainerUtility::findTextGroupHandle(Container* container) noexcept
	{
		GfxDriverObjectHandleType handle = GFX_DRIVER_OBJECT_NULL_HANDLE;
		ContainerUtility::ContainersWalkUpUntil(container, [&handle](Container* container)
		{
			TextGroupContainer* textGroupContainer = dynamic_cast<TextGroupContainer*>(container);
			if(textGroupContainer)
			{
				handle = textGroupContainer->getGfxDriverObjectHandle();
				// Stop walk up
				return false;
			}
			return true;
		});
		return handle;
	}

	void ContainerUtility::IInputEventHandlerObjectsVisit(Container* container, IInputEventHandlerObjectsVisitor visitor) noexcept
	{
		if(container->hasInputEventHandlers())
			visitor(container->getInputEventHandlers());
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			IInputEventHandlerObjectsVisit(child, visitor);
	}

	void ContainerUtility::IInputEventHandlerObjectsVisit(Container* container, IInputEventHandlerObjectsVisitor visitor, bool isIncludeItself) noexcept
	{
		if(isIncludeItself)
			if(container->hasInputEventHandlers())
				visitor(container->getInputEventHandlers());
		std::vector<Container*>& childs = container->getChilds();
		for(Container* &child : childs)
			IInputEventHandlerObjectsVisit(child, visitor);
	}
}