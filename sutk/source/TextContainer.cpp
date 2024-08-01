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

	void TextContainer::onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// mandatory
		Container::onParentResize(newRect, isPositionChanged, isSizeChanged);

		// the text is being rendered in absolute coordinate system of SGE, so to recalcuate the absolute coordinates we need to call 
		// the function onContainerResize again.
		if(m_text != NULL)
			m_text->onContainerResize(getRect(), true, false);
	}

	void TextContainer::onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged)
	{
		// It is mandatory to call Container::onResize in any of the derived class overriding the function.
		Container::onResize(newRect, isPositionChanged, isSizeChanged);

		if(m_text != NULL)
			m_text->onContainerResize(newRect, isPositionChanged, isSizeChanged);
	}
}