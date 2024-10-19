#include <sutk/UIDriverObject.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	IGfxDriver& UIDriverObject::getGfxDriver() { return getUIDriver().getGfxDriver(); }
	const IGfxDriver& UIDriverObject::getGfxDriver() const { return getUIDriver().getGfxDriver(); }
	IInputDriver& UIDriverObject::getInputDriver() { return getUIDriver().getInputDriver(); }
	const IInputDriver& UIDriverObject::getInputDriver() const { return getUIDriver().getInputDriver(); }	
}
