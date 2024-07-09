#include <sutk/UIDriver.hpp>

#include <sutk/TextContainer.hpp>
#include <sutk/Text.hpp>
#include <sutk/IGfxDriver.hpp>

#include <common/assert.h>

namespace SUTK
{
	UIDriver::UIDriver(IGfxDriver& gfxDriver): m_gfxDriver(gfxDriver)
	{

	}

	void UIDriver::render()
	{
		// update GPU side data
		for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
			if((*it)->isDirty())
				(*it)->update();

		// now record and dispatch rendering commands (delegated that to rendering backend)
		m_gfxDriver.render(*this);
	}

	template<>
	Container* UIDriver::createContainer<Container>(Container* parent)
	{
		Container* cntr = new Container(*this);
		if(parent != NULL)
			parent->add(cntr);
		else
		{
			auto size = m_gfxDriver.getSize();
			cntr->setRect({ 0, 0, size.width, size.height });
		}
		return cntr;
	}

	template<>
	TextContainer* UIDriver::createContainer<TextContainer>(Container* parent)
	{
		TextContainer* txtCntr = new TextContainer(*this);
		if(parent != NULL)
			parent->add(txtCntr);
		else
		{
			auto size = m_gfxDriver.getSize();
			txtCntr->setRect({ 0, 0, size.width, size.height });
		}
		return txtCntr;
	}

	Text* UIDriver::createText(TextContainer* container)
	{
		_assert(container != NULL);
		Text* text = new Text(*this, container);
		container->setText(text);
		m_renderables.push_back(text);
		return text;
	}
}