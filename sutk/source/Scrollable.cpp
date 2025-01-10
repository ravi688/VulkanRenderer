#include <sutk/Scrollable.hpp>
#include <sutk/Container.hpp>
#include <sutk/ScrollContainer.hpp> // otherwise dynamic_cast to this target won't work!
#include <sutk/ContainerUtility.hpp> // for SUTK::ContainerUtility

namespace SUTK
{
	Scrollable::Scrollable(Container* container) noexcept : m_container(container)
	{
		m_scrollContainer = dynamic_cast<ScrollContainer*>(container->getParent());
		_COM_ASSERT(m_scrollContainer);
		m_scrollContainer->onScrollableAdd(this);
		ContainerUtility::EnsureTopLeftAnchors(container);
	}

	void Scrollable::setScrollMargins(RectMarginsf margins) noexcept
	{
		m_scrollMargins = margins;
	}
	void Scrollable::setFullScrollMargins(RectEdgeBits edgeBits) noexcept
	{
		m_fullMarginEdges = edgeBits;
	}

	RectMarginsf Scrollable::getScrollMargins() const noexcept
	{
		auto margins = m_scrollMargins;
		auto size = m_scrollContainer->getSize();
		if(com::EnumClassHasFlag(m_fullMarginEdges, RectEdgeBits::Left))
			margins.left = size.width;
		if(com::EnumClassHasFlag(m_fullMarginEdges, RectEdgeBits::Right))
			margins.right = size.width;
		if(com::EnumClassHasFlag(m_fullMarginEdges, RectEdgeBits::Top))
			margins.top = size.height;
		if(com::EnumClassHasFlag(m_fullMarginEdges, RectEdgeBits::Bottom))
			margins.bottom = size.height;
		return margins;
	}
}