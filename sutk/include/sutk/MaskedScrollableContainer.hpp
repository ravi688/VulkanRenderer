#pragma once

#include <sutk/defines.hpp>
#include <sutk/Scrollable.hpp> // for SUTK::Scrollable
#include <sutk/ScrollContainer.hpp> // for SUTK::ScrollContainer
#include <sutk/Renderable.hpp> // for SUTK::GfxDriverRenderable
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility::RenderablesVisit

#include <concepts> // for std::is_derived_from and std::same_as concepts

namespace SUTK
{
	template<typename T>
	concept ContainerT = std::derived_from<T, SUTK::Container> || std::same_as<T, SUTK::Container>;

	template<ContainerT ContainerType, typename... Args>
	class MaskedScrollableContainer : public ContainerType, public Scrollable
	{
	protected:
		MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept;
		// Recursively finds all SUTK::GfxDriverRenderable objects
		// and updates their clip rects with that of ScrollContainer's global Rect
		void updateMask() noexcept;

		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		virtual ~MaskedScrollableContainer() noexcept = default;
	};

	template<ContainerT ContainerType, typename... Args>
	MaskedScrollableContainer<ContainerType, Args...>::MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept : ContainerType(driver, parent, std::forward(args)...), Scrollable(this)
	{

	}
	
	template<ContainerT ContainerType, typename... Args>
	void MaskedScrollableContainer<ContainerType, Args...>::updateMask() noexcept
	{
		ScrollContainer* scrollCont = getScrollContainer();
		if(!scrollCont)
			return;
		Rect2Df rect = scrollCont->getGlobalRect();
		ContainerUtility::RenderablesVisit(this, [rect](Renderable* renderable)
		{
			GfxDriverRenderable* gfxRenderable = dynamic_cast<GfxDriverRenderable*>(renderable);
			if(gfxRenderable != NULL)
			{
				gfxRenderable->setClipRectGlobalCoords(rect);
			}
		});
	}

	template<ContainerT ContainerType, typename... Args>
	void MaskedScrollableContainer<ContainerType, Args...>::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// Mandatory to be called by the overriding method
		Container::onParentResize(newRect, isPositionChanged, isSizeChanged);
		updateMask();
	}
}