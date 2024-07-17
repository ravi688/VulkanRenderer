#pragma once

#include <sutk/defines.hpp>

#include <vector> /* for std::vector */

namespace SUTK
{
	class IRenderable;
	class Container;
	class FullWindowContainer;
	class TextContainer;
	class Text;
	class IGfxDriver;

	class UIDriver
	{
	private:
		IGfxDriver& m_gfxDriver;
		std::vector<IRenderable*> m_renderables;
	public:
		UIDriver(IGfxDriver& gfxDriver);

		void render();

		IGfxDriver& getGfxDriver() { return m_gfxDriver; }

		template<typename ContainerType>
		ContainerType* createContainer(Container* parent = NULL) { return NULL; }
		Text* createText(TextContainer* parent);
	};

	template<>
	Container* UIDriver::createContainer<Container>(Container* parent);
	template<>
	FullWindowContainer* UIDriver::createContainer<FullWindowContainer>(Container* parent);
	template<>
	TextContainer* UIDriver::createContainer<TextContainer>(Container* parent);

	class UIDriverObject
	{
	private:
		UIDriver& m_uiDriver;
	protected:
		UIDriverObject(UIDriver& uiDriver) : m_uiDriver(uiDriver) { }
	public:
		UIDriver& getUIDriver() { return m_uiDriver; }
		IGfxDriver& getGfxDriver() { return getUIDriver().getGfxDriver(); }
	};
}