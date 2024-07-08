#include <sutk/UIDriver.hpp>

#include <sutk/Text.hpp>
#include <sutk/Container.hpp>

namespace SUTK
{
	UIDriver::UIDriver(UIRendererCallbacks& callbacks): m_callbacks(callbacks)
	{

	}

	void UIDriver::render()
	{

	}

	Container* UIDriver::createContainer()
	{
		return NULL;
	}

	Text* UIDriver::createText()
	{
		return NULL;
	}
}