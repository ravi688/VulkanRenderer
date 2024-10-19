#pragma once

namespace SUTK
{
	class UIDriver;
	class IGfxDriver;
	class IInputDriver;

	class UIDriverObject
	{
	private:
		UIDriver& m_uiDriver;
	protected:
		UIDriverObject(UIDriver& uiDriver) : m_uiDriver(uiDriver) { }
	public:
		UIDriver& getUIDriver() { return m_uiDriver; }
		const UIDriver& getUIDriver() const { return m_uiDriver; }
		IGfxDriver& getGfxDriver();
		const IGfxDriver& getGfxDriver() const;
		IInputDriver& getInputDriver();
		const IInputDriver& getInputDriver() const;
	};
}