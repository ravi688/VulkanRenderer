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
		HorizontalAlignment m_hAlign;
		VerticalAlignment m_vAlign;
		GfxDriverObjectHandleType m_textGroup;
		com::Bool m_isReassociateOnParentChange;
		void createTextFirstTime() noexcept;
		void tryReassociate() noexcept;
	protected:
		virtual void onAnscestorChange(Container* anscestor) noexcept override;
	public:
		Label(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept;
		Label(UIDriver& driver, Container* parent) noexcept;

		void setReassociateOnParentChange(com::Bool isReassociate, com::Bool isApplyNow = com::False) noexcept;
		void set(const std::string_view str) noexcept;
		void setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept;

		virtual Renderable* getRenderable() noexcept override;
		SmallText& getText() noexcept;
	};
}