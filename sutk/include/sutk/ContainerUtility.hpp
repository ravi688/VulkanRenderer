#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	
	class ContainerUtility
	{
	private:
		// Calls Renderable::setActive() only for the passed as the first parameter, i.e. it is not recursive
		static void RenderableSetActive(Container* container, bool isActive) noexcept;
	public:
		// Calls setActive() for each Container, and Renderable recursively including for the one passed as the first parameter
		static void SetActiveAllRecursive(Container* container, bool isActive) noexcept;
		// Calls Container::setActive() for all the containers (childs) recursively including for the one passed as the first parameter
		static void ContainerSetActiveRecursive(Container* container, bool isActive) noexcept;
		// Calls Renderable::setActive() for all Renderable(s) recursively including for the one passed as the first parameter
		static void RenderablesSetActive(Container* container, bool isActive) noexcept;
		// Calls IColorable::getColor() followed by IColorable::setColor() for all IColorable(s) recursively including for the one passed as the first parameter
		static void RenderablesSetAlpha(Container* container, f32 alpha) noexcept;
	};
}