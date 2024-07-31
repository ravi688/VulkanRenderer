#include <sutk/AnchorRect.hpp>
#include <sutk/Container.hpp> // for SUTK::Container member functions

namespace SUTK
{
	AnchorRect::AnchorRect(Container& childRect, Container& parentRect) noexcept : m_childRect(childRect), m_parentRect(parentRect)
	{
		// NOTE: initially the anchor rect is degenerate (width and height both is zero) and has position as { 0, 0 }

		recalculateConstraints();
	}

	Rect2Df AnchorRect::getUnnormalizedAnchorRect() noexcept
	{
		Vec2Df parentSize = m_parentRect.getSize();
		// calculate unnormalized anchor rect in the parent rect's local coordinates
		Rect2Df anchorRect { m_rect.getPosition() * parentSize, m_rect.getSize() * parentSize };
		return anchorRect;
	}

	void AnchorRect::recalculateConstraints() noexcept
	{
		Rect2Df anchorRect = getUnnormalizedAnchorRect();
		// signed difference is the constraint
		m_constraintRect = anchorRect - m_childRect.getRect();
	}

	void AnchorRect::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		Rect2Df anchorRect = getUnnormalizedAnchorRect();
		Rect2Df newChildRect = anchorRect - m_constraintRect;
		m_childRect.setRect(newChildRect);
	}

	void AnchorRect::onChildResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		recalculateConstraints();
	}

	void AnchorRect::setRect(Rect2Df rect) noexcept
	{
		m_rect = rect;
		recalculateConstraints();	
	}

	void AnchorRect::setTopLeft(Vec2Df point) noexcept
	{
		m_rect.setTopLeft(point);
		recalculateConstraints();
	}

	void AnchorRect::setBottomRight(Vec2Df point) noexcept
	{
		m_rect.setBottomRight(point);
		recalculateConstraints();
	}


	void AnchorRect::setTopRight(Vec2Df point) noexcept
	{
		m_rect.setTopRight(point);
		recalculateConstraints();
	}

	void AnchorRect::setBottomLeft(Vec2Df point) noexcept
	{
		m_rect.setBottomLeft(point);
		recalculateConstraints();
	}


	void AnchorRect::extendTopLeft(Vec2Df disp) noexcept
	{
		m_rect.extendTopLeft(disp);
		recalculateConstraints();
	}

	void AnchorRect::extendTopRight(Vec2Df disp) noexcept
	{
		m_rect.extendTopRight(disp);
		recalculateConstraints();
	}

	void AnchorRect::extendBottomRight(Vec2Df disp) noexcept
	{
		m_rect.extendBottomRight(disp);
		recalculateConstraints();
	}

	void AnchorRect::extendBottomLeft(Vec2Df disp) noexcept
	{
		m_rect.extendBottomLeft(disp);
		recalculateConstraints();
	}

}