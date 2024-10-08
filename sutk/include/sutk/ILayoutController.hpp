#pragma once

#include <sutk/defines.hpp> // for SUTK_API

namespace SUTK
{
	class SUTK_API ILayoutController
	{
	private:
		bool m_isLockedLayout;
	protected:
		// must be implemented by the derived class
		virtual void onRecalculateLayout() noexcept = 0;
	public:
		ILayoutController(bool isLayoutLocked = false) noexcept;
		virtual ~ILayoutController() noexcept = default;

		void lockLayout() noexcept;
		void unlockLayout(bool isRecalculate = false) noexcept;
		bool isLockedLayout() const noexcept { return m_isLockedLayout; }
		void recalculateLayout() noexcept;
	};
}