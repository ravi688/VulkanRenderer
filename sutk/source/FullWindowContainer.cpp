#include <sutk/FullWindowContainer.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/IGfxDriver.hpp>

#include <common/debug.h>

namespace SUTK
{
	FullWindowContainer::FullWindowContainer(UIDriver& driver, Container* parent) noexcept : Container(driver, parent)
	{
		auto size = getGfxDriver().getSizeInCentimeters();
		setRect({0, 0, size.width, size.height });
		driver.getGfxDriver().addOnResizeHandler([this](Vec2Df size)
			{ 
				setRect({ 0, 0, size.width, size.height });
			});
	}
}