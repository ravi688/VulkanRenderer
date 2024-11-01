#pragma once

#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer
#include <sutk/TextTraits.hpp>

#include <string> // for std::string

namespace SUTK
{
	class SmallText;
	class UIDriver;
	class TextGroupContainer;
	
	class SUTK_API Label : public RenderableContainer
	{
	private:
		SmallText* m_text;
	public:
		Label(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept;
		Label(UIDriver& driver, Container* parent) noexcept;

		void set(const std::string_view str) noexcept;
		void setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept;

		SmallText& getText() { return *m_text; }
	};
}