#include <sutk/ILayoutController.hpp>

namespace SUTK
{
	ILayoutController::ILayoutController(bool isLayoutLocked) noexcept : m_isLockedLayout(isLayoutLocked)
	{

	}

	void ILayoutController::lockLayout() noexcept
	{
		m_isLockedLayout = true;
	}

	void ILayoutController::unlockLayout(bool isRecalculate) noexcept
	{
		m_isLockedLayout = false;
		if(isRecalculate)
			recalculateLayout();
	}

	void ILayoutController::recalculateLayout() noexcept
	{
		if(isLockedLayout())
			return;
		onRecalculateLayout();
	}
}