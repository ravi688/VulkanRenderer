#include <sutk/AnchorRect.hpp>
#include <sutk/Container.hpp> // for SUTK::Container member functions

namespace SUTK
{
	AnchorRect::AnchorRect(Container* childRect, Container* parentRect) noexcept : m_childRect(childRect), m_parentRect(parentRect), m_isActive(true)
	{
		// NOTE: initially the anchor rect is degenerate (width and height both is zero) and has position as { 0, 0 }

		recalculateConstraints();
	}

	Rect2Df AnchorRect::getUnnormalizedAnchorRect() noexcept
	{
		_com_assert(m_parentRect != NULL);
		Vec2Df parentSize = m_parentRect->getSize();
		// calculate unnormalized anchor rect in the parent rect's local coordinates
		Rect2Df anchorRect { m_rect.getPosition() * parentSize, m_rect.getSize() * parentSize };
		return anchorRect;
	}

	void AnchorRect::recalculateConstraints() noexcept
	{
		if((m_childRect == NULL) || (m_parentRect == NULL) || !m_isActive)
			return;
		Rect2Df anchorRect = getUnnormalizedAnchorRect();
		// signed difference is the constraint
		m_constraintRect = anchorRect - m_childRect->getRect();
	}

	void AnchorRect::setParent(Container* parent) noexcept
	{
		m_parentRect = parent;
		recalculateConstraints();
	}
	void AnchorRect::setChild(Container* child) noexcept
	{
		m_childRect = child;
		recalculateConstraints();
	}

	void AnchorRect::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		_com_assert(m_childRect != NULL);
		if(!m_isActive) return;
		Rect2Df anchorRect = getUnnormalizedAnchorRect();
		Rect2Df newChildRect = anchorRect - m_constraintRect;
		m_childRect->setRect(newChildRect);
	}

	void AnchorRect::onChildResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) noexcept
	{
		recalculateConstraints();
	}

	void AnchorRect::setActive(bool isActive) noexcept
	{
		if(!(m_isActive ^ isActive))
			return;
		if(isActive)
			recalculateConstraints();
		m_isActive = isActive;
	}

	void AnchorRect::fitToChildRect() noexcept
	{
		_com_assert(m_childRect != NULL);
		_com_assert(m_parentRect != NULL);
		const Rect2Df childRect = m_childRect->getRect();
		const Vec2Df size = m_parentRect->getSize();
		Rect2Df anchorRect;
		anchorRect.setTopLeft(childRect.getTopLeft() / size);
		anchorRect.setBottomRight(childRect.getBottomRight() / size);
		setRect(anchorRect);
	}

	void AnchorRect::fitToParentRect() noexcept
	{
		setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
	}

	void AnchorRect::moveToCenterOfChild() noexcept
	{
		_com_assert(m_childRect != NULL);
		_com_assert(m_parentRect != NULL);
		// Create Rect of zero size (area)
		Rect2Df anchorRect;
		// Change its position
		anchorRect.setPosition(m_childRect->getRect().getCenter() / m_parentRect->getSize());
		setRect(anchorRect);
	}

	void AnchorRect::moveToRightMiddleOfParent() noexcept
	{
		Rect2Df anchorRect;
		anchorRect.setPosition({ 1.0f, 0.5f });
		setRect(anchorRect);
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