#include <sutk/TextContainer.hpp>

namespace SUTK
{
	void TextContainer::onAdd(Container* parent)
	{
		Container::onAdd(parent);
	}
	
	void TextContainer::setText(Text* text)
	{
		m_text = text;
	}
}