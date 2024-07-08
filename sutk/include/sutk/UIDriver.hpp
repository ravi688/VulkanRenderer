#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
	class TextContainer;
	class Text;

	struct UIRendererCallbacks
	{
		void* userData;
	};

	class UIDriver
	{
	private:
		UIRendererCallbacks m_callbacks;
	public:
		UIDriver(UIRendererCallbacks& callbacks);

		void render();

		template<typename ContainerType>
		ContainerType* createContainer(Container* parent = NULL) { return NULL; }
		Text* createText(TextContainer* parent);
	};

	template<>
	Container* UIDriver::createContainer<Container>(Container* parent);
	template<>
	TextContainer* UIDriver::createContainer<TextContainer>(Container* parent);
}