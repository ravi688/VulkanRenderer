#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	class ScrollContainer;

	class Scrollable
	{
	private:
		ScrollContainer* m_scrollContainer;
	protected:
		ScrollContainer* getScrollContainer() noexcept { return m_scrollContainer; }
		const ScrollContainer* getScrollContainer() const noexcept { return m_scrollContainer; }
		Scrollable(Container* container) noexcept;
	public:
		virtual ~Scrollable() noexcept = default;
	};
}