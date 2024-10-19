#pragma once

#include <sutk/defines.hpp>
#include <sutk/Container.hpp> // for SUTK::Container
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer
#include <sutk/UIDriverObject.hpp> // for 
#include <concepts> // for std::is_derived_from and std::same_as concepts

namespace SUTK
{
	template<typename T>
	concept ContainerT = std::derived_from<T, Container> || std::same_as<T, Container>;
	template<typename T>
	concept RenderableContainerT = std::derived_from<T, RenderableContainer> || std::same_as<T, RenderableContainer>;
	template<typename T>
	concept RenderableT = std::derived_from<T, Renderable> || std::same_as<T, Renderable>;
	template<typename T>
	concept UIDriverObjectT = std::derived_from<T, UIDriverObject> || std::same_as<T, UIDriverObject>;
}
