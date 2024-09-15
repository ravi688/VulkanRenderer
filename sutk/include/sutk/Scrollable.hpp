#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	class ScrollContainer;

	class Scrollable
	{
	protected:
		ScrollContainer* m_scrollContainer;
		ScrollContainer* getScrollContainer() noexcept { return m_scrollContainer; }
		const ScrollContainer* getScrollContainer() const noexcept { return m_scrollContainer; }
		Scrollable(Container* container) noexcept;
	public:
		virtual ~Scrollable() noexcept = default;
	};
}