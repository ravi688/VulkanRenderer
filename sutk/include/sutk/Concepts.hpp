#pragma once

#include <sutk/defines.hpp>
#include <sutk/Container.hpp> // for SUTK::Container
#include <concepts> // for std::is_derived_from and std::same_as concepts

namespace SUTK
{
	template<typename T>
	concept ContainerT = std::derived_from<T, SUTK::Container> || std::same_as<T, SUTK::Container>;
}
