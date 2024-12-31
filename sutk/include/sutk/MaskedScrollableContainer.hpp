#pragma once

#include <sutk/defines.hpp>
#include <sutk/Scrollable.hpp> // for SUTK::Scrollable
#include <sutk/ScrollContainer.hpp> // for SUTK::ScrollContainer
#include <sutk/Renderable.hpp> // for SUTK::GfxDriverRenderable
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility::RenderablesVisit
#include <sutk/InputEventHandlerObject.hpp> // for SUTK::MouseEventsBlockerObject
#include <sutk/Concepts.hpp> // for SUTK::ContainerT

namespace SUTK
{
	template<ContainerT ContainerType>
	class MaskedScrollableContainer : public ContainerType, public MouseEventsBlockerObject, public Scrollable
	{
	protected:
		void setMaskFor(Container* container, Rect2Df rect) const noexcept;
		// Recursively finds all SUTK::GfxDriverRenderable objects
		// and updates their clip rects with that of ScrollContainer's global Rect
		void updateMask() noexcept;

		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onAddChild(Container* child) override;
		virtual void onRemoveChild(Container* child) override;
		virtual bool isInside(Vec2Df point) const noexcept override;
	public:
		template<typename... Args>
		MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept;
		virtual ~MaskedScrollableContainer() noexcept = default;

		// TODO: These two functions are to only allow NotebookView to hack the Masking of the Grabbed Tab
		// In future we need to redesign the working of Masking, any renderable with draw order greater
		// than the Mask won't be affected by it.

		// Makes all the renderables inside 'container' (recursively) clipped inside the mask of the ScrollContainer
		void updateMaskFor(Container* container) const noexcept;
		// Extends clipping area for each renderables inside 'container' (recursively) to full window size
		void restoreMaskFor(Container* container) noexcept;
	};

	template<ContainerT ContainerType>
	template<typename... Args>
	MaskedScrollableContainer<ContainerType>::MaskedScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept : ContainerType(driver, parent, std::forward<Args&&>(args)...), MouseEventsBlockerObject(driver, this, this->getDepth() + 10000), Scrollable(this)
	{
		_com_assert(dynamic_cast<ScrollContainer*>(parent) != NULL);
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::setMaskFor(Container* container, Rect2Df rect) const noexcept
	{
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
	void MaskedScrollableContainer<ContainerType>::updateMaskFor(Container* container) const noexcept
	{
		const ScrollContainer* scrollCont = getScrollContainer();
		if(!scrollCont)
			return;
		Rect2Df rect = scrollCont->getGlobalRect();
		setMaskFor(container, rect);
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
		ContainerType::onParentResize(newRect, isPositionChanged, isSizeChanged);
		updateMask();
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::onAddChild(Container* child)
	{
		ContainerType::onAddChild(child);
		updateMaskFor(child);
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::restoreMaskFor(Container* container) noexcept
	{
		// Restore visible area to the entire window for this removed child
		// as it is now no longer affected by Scrolling and Masking.
		Vec2Df size = ContainerType::getUIDriver().getWindowSize();
		setMaskFor(container, { {0, 0}, size });
	}

	template<ContainerT ContainerType>
	void MaskedScrollableContainer<ContainerType>::onRemoveChild(Container* child)
	{
		restoreMaskFor(child);
		ContainerType::onRemoveChild(child);
	}

	template<ContainerT ContainerType>
	bool MaskedScrollableContainer<ContainerType>::isInside(Vec2Df point) const noexcept
	{
		bool isInsideOfOuterRect = MouseEventsBlockerObject::isInside(point);
		const ScrollContainer* scrollCont = getScrollContainer();
		if(!scrollCont || !isInsideOfOuterRect)
			return isInsideOfOuterRect;
		Rect2Df rect = scrollCont->getGlobalRect();
		if(!rect.contains(point))
		{
			ContainerUtility::IInputEventHandlerObjectsVisit(com::cast_away_const(this), [](std::vector<IInputEventHandlerObject*>& eventHandlerObject)
			{
				for(auto& eventHandlerObject : eventHandlerObject)
					eventHandlerObject->tempSleep();
			}, false);
		}
		return false;
	}
}