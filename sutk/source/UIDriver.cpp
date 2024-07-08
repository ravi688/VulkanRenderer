#include <sutk/UIDriver.hpp>

#include <sutk/TextContainer.hpp>
#include <sutk/Text.hpp>

namespace SUTK
{
	UIDriver::UIDriver(UIRendererCallbacks& callbacks): m_callbacks(callbacks)
	{

	}

	void UIDriver::render()
	{

	}

	template<>
	Container* UIDriver::createContainer<Container>(Container* parent)
	{
		Container* cntr = new Container(*this);
		if(parent != NULL)
			parent->add(cntr);
		return cntr;
	}

	template<>
	TextContainer* UIDriver::createContainer<TextContainer>(Container* parent)
	{
		TextContainer* txtCntr = new TextContainer(*this);
		if(parent != NULL)
			parent->add(txtCntr);
		return txtCntr;
	}

	Text* UIDriver::createText(TextContainer* container)
	{
		Text* text = new Text(*this, container);
		container->setText(text);
		return text;
	}
}