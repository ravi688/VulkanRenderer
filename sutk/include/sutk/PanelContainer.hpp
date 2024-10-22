#pragma once

#include <sutk/Container.hpp> // for SUTK::Container
#include <sutk/InputEventHandlerObject.hpp> // for SUTK::MouseEventsBlockerObject
#include <sutk/Concepts.hpp> // for SUTK::ContainerT concept

namespace SUTK
{
	template<ContainerT ContainerType>
	class SUTK_API TPanelContainer : public ContainerType, private MouseEventsBlockerObject
	{
	public:
		template<typename... Args>
		TPanelContainer(SUTK::UIDriver& driver, Container* parent = NULL, Args&&... args) noexcept : 
			ContainerType(driver, parent, std::forward<Args&&>(args)...), MouseEventsBlockerObject(driver, this) { }
	};

	typedef TPanelContainer<Container> PanelContainer;
}
