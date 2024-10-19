#pragma once

#include <sutk/defines.hpp>
#include <functional> // for std::function

namespace SUTK
{
	class Container;
	class Renderable;
	
	class ContainerUtility
	{
	private:
		// Calls Renderable::setActive() only for the passed as the first parameter, i.e. it is not recursive
		static void RenderableSetActive(Container* container, com::Bool isActive) noexcept;
	public:
		// Calls setActive() for each Container, and Renderable recursively including for the one passed as the first parameter
		static void SetActiveAllRecursive(Container* container, com::Bool isActive) noexcept;
		// Calls Container::setActive() for all the containers (childs) recursively including for the one passed as the first parameter
		static void ContainerSetActiveRecursive(Container* container, com::Bool isActive) noexcept;
		// Calls Renderable::setActive() for all Renderable(s) recursively including for the one passed as the first parameter
		static void RenderablesSetActive(Container* container, com::Bool isActive) noexcept;
		// Calls IColorable::getColor() followed by IColorable::setColor() for all IColorable(s) recursively including for the one passed as the first parameter
		static void RenderablesSetAlpha(Container* container, f32 alpha) noexcept;
		// Calls visit() for each found Renderable object recursively
		static void RenderablesVisit(Container* container, const std::function<void(Renderable*)>& visitor) noexcept;
	};
}