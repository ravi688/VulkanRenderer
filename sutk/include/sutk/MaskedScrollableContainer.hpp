#pragma once

#include <sutk/defines.hpp>
#include <sutk/Scrollable.hpp> // for SUTK::Scrollable
#include <sutk/ScrollContainer.hpp> // for SUTK::ScrollContainer
#include <sutk/Renderable.hpp> // for SUTK::GfxDriverRenderable
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility::RenderablesVisit
#include <sutk/Concepts.hpp> // for SUTK::ContainerT

namespace SUTK
{
	template<ContainerT ContainerType>
	class MaskedScrollableContainer : public ContainerType, public Scrollable
	{
	protected:
		template<typename... Args>
		MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept;
		void updateMaskFor(Container* container) const noexcept;
		// Recursively finds all SUTK::GfxDriverRenderable objects
		// and updates their clip rects with that of ScrollContainer's global Rect
		void updateMask() noexcept;

		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		virtual ~MaskedScrollableContainer() noexcept = default;
	};

	template<ContainerT ContainerType>
	template<typename... Args>
	MaskedScrollableContainer<ContainerType>::MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept : ContainerType(driver, parent, std::forward<Args&&>(args)...), Scrollable(this)
	{
		_com_assert(dynamic_cast<ScrollContainer*>(parent) != NULL);
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::updateMaskFor(Container* container) const noexcept
	{
		const ScrollContainer* scrollCont = getScrollContainer();
		if(!scrollCont)
			return;
		Rect2Df rect = scrollCont->getGlobalRect();
		ContainerUtility::RenderablesVisit(container, [rect](Renderable* renderable)
		{
			GfxDriverRenderable* gfxRenderable = dynamic_cast<GfxDriverRenderable*>(renderable);
			if(gfxRenderable != NULL)
			{
				gfxRenderable->setClipRectGlobalCoords(rect);
			}
		});
	}
	
	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::updateMask() noexcept
	{
		updateMaskFor(this);
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// Mandatory to be called by the overriding method
		Container::onParentResize(newRect, isPositionChanged, isSizeChanged);
		updateMask();
	}
}