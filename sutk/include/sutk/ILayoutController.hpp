#pragma once

#include <sutk/defines.hpp> // for SUTK_API

namespace SUTK
{
	class SUTK_API ILayoutController
	{
	public:
		ILayoutController() noexcept = default;
		virtual ~ILayoutController() noexcept = default;

		// must be implemented by the derived class
		virtual void recalculateLayout() = 0;
	};
}