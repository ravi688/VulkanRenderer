#include <sutk/FullWindowContainer.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/IGfxDriver.hpp>

#include <common/debug.h>

namespace SUTK
{
	FullWindowContainer::FullWindowContainer(UIDriver& driver) noexcept : Container(driver, NULL)
	{
		auto size = getGfxDriver().getSize();
		setRect({0, 0, size.width, size.height });
		driver.getGfxDriver().addOnResizeHandler([this](Vec2D<DisplaySizeType> size)
			{ 
				setRect({ 0, 0, size.width, size.height });
			});
	}
}