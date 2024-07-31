#pragma once

#include <sutk/defines.hpp> // for SUTK_API, and Rect2D

namespace SUTK
{
	class Container;

	// This class is derived from Container
	class SUTK_API AnchorRect
	{
	private:
		Rect2Df m_rect;
		Rect2Df m_constraintRect;
		Container& m_childRect;
		Container& m_parentRect;
		AnchorRect(Container& childRect, Container& parentRect) noexcept;

		friend class Container;
		// should be called by the parent container whenever its rect is resized or changes its position
		void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept;
		// should be called by the child container whenever its rect is resized or changes its position
		void onChildResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept;

		Rect2Df getUnnormalizedAnchorRect() noexcept;
		void recalculateConstraints() noexcept;
	public:

		// getters
		Rect2Df getRect() const noexcept { return m_rect; }

		// setters

		void setRect(Rect2Df rect) noexcept;

		void setTopLeft(Vec2Df point) noexcept;
		void setBottomRight(Vec2Df point) noexcept;

		void setTopRight(Vec2Df point) noexcept;
		void setBottomLeft(Vec2Df point) noexcept;

		void extendTopLeft(Vec2Df disp) noexcept;
		void extendTopRight(Vec2Df disp) noexcept;
		void extendBottomRight(Vec2Df disp) noexcept;
		void extendBottomLeft(Vec2Df disp) noexcept;
	};
}