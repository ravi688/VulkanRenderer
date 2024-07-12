#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class Text;

	class TextContainer : public Container
	{
	private:
		Text* m_text;

		// this can only be called by SUTK::UIDriver
		TextContainer(UIDriver& driver) : Container(driver) { }

		friend class UIDriver;

	protected:
		virtual void onAdd(Container* parent) override;

	public:

		void setText(Text* text);

		Text* getText() noexcept { return m_text; }

		virtual void onResize(const Rect2D<DisplaySizeType>& newRect, bool isPositionChanged, bool isSizeChanged) override;
	};
}