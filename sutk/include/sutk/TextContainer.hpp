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
		TextContainer(UIDriver& driver, Container* parent) : Container(driver, parent), m_text(NULL) { }

		friend class UIDriver;

	protected:
		virtual void onAdd(Container* parent) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

	public:

		void setText(Text* text);

		Text* getText() noexcept { return m_text; }

	};
}