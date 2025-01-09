#pragma once

#include <sutk/Scrollable.hpp> // for SUTK::Scrollable
#include <sutk/Concepts.hpp> // for SUTK::ContainerT

namespace SUTK
{
	// Any Container which is supposed to be a child of ScrollContainer, i.e. it is Scrollable, then 
	// It must be derived from this class.
	// NOTE: If the container also handles inputs and display visuals (Renderables) then you're probably lookin for
	// MaskedScrollableContainer which is also derived from this class. (see: MaskedScrollableContainer.hpp)
	template<ContainerT ContainerType = Container>
	class SUTK_API ScrollableContainer : public ContainerType, public Scrollable
	{
	public:
		template<typename... Args>
		ScrollableContainer(UIDriver& driver, Container* parent, Args&&... args) noexcept : ContainerType(driver, parent, std::forward<Args&&>(args)...), Scrollable(this) { }
	};
}