#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class Text;

	class TextContainer : public Container
	{
	private:
		Text* m_text;

	protected:
		virtual void onAdd(Container* parent) override;

	public:
		TextContainer(UIDriver& driver) : Container(driver) { }

		void setText(Text* text);

		Text* getText() noexcept { return m_text; }
	};
}