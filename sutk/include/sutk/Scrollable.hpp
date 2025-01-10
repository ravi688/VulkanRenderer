#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	class ScrollContainer;

	class Scrollable
	{
	private:
		// pointer to the Container object which is driven by this Scrollable "attribute" class (actually it's the Scroll Container which drives)
		Container* m_container;
		ScrollContainer* m_scrollContainer;
		RectMarginsf m_scrollMargins { };
		RectEdgeBits m_fullMarginEdges { };
	protected:
		ScrollContainer* getScrollContainer() noexcept { return m_scrollContainer; }
		const ScrollContainer* getScrollContainer() const noexcept { return m_scrollContainer; }
		Scrollable(Container* container) noexcept;
	public:
		virtual ~Scrollable() noexcept = default;

		// Setters

		// margins.left: is the max distance of the left edge of this Scrollable's Rect to the left edge of the ScrollbleContainer's Rect
		//				If it is zero then the Scrollable can't be scrolled rightwards detaching its left edge off the left edge of the ScrollContainer's Rect
		// margins.right: is the max distance of the right edge of this Scrollable's Rect to the right edge of the ScrollableContainer's Rect
		//				If it is zero then the Scrollable can't be scrolled leftwards detaching its right edge off the right edge of the ScrollContainer's Rect
		// margins.top:  is the max distance of the top edge of this Scrollable's Rect to the top edge of the ScrollContainer's Rect
		//			 	If it is zero then the Scrollable can't be scrolled downwards detaching its top edge off the top edge of the ScrollContainer's Rect
		// margins.bottom: is the max distance of the bottom edge of this Scrollable's Rect to the bottom edge of the ScrollContainer's Rect
		//				If it is zero then the Scrollable can't be scrolled upwards detaching its bottom edge off the bottom edge of the ScrollContainer's Rect
		void setScrollMargins(RectMarginsf margins) noexcept;
		// edgeBits: Bit Mask telling which sides/edges of this Scrollable's Rect will have full margin 
		//	(if left/right then its the ScrollContainer's width, if top/bottom then its the ScrollContainer's height).
		// 		RectEdgeBits::Left means margins.left will be overriden with ScrollContainer's width
		//		RectEdgeBits::Right means margins.right will be overriden with ScrollContainer's width
		//		RectEdgeBits::Top means margins.top will be overriden with ScrollContainer's height
		//		RectEdgeBits::Bottom means margins.bottom will be overriden with ScrollContainer's height
		void setFullScrollMargins(RectEdgeBits edgeBits) noexcept;
		
		// Getters
		// If any of the flags in m_fullMarginEdges are set then it overrides the corresponding margin in the return copy of m_scrollMargins.
		RectMarginsf getScrollMargins() const noexcept;
		Container* getContainer() noexcept { return m_container; }
		const Container* getContainer() const noexcept { return m_container; }
	};
}