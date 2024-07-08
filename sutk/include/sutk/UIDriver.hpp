#pragma once

#include <sutk/defines.hpp>

namespace SUTK
{
	class Container;
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

		Container* createContainer();
		Text* createText();
	};
}