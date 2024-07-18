#include <sutk/TextContainer.hpp>
#include <sutk/Text.hpp> /* for SUTK::Text::onContainerResize callback */
#include <sutk/assert.h> /* for _assert */

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

	void TextContainer::onResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		if(m_text != NULL)
			m_text->onContainerResize(newRect, isPositionChanged, isSizeChanged);
	}
}