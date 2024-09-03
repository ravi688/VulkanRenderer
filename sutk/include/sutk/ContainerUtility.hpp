#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	
	class ContainerUtility
	{
	public:
		static void RenderablesSetActive(Container* container, bool isActive) noexcept;
		static void RenderablesSetAlpha(Container* container, f32 alpha) noexcept;
	};
}