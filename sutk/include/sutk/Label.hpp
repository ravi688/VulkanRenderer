#pragma once

#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer
#include <sutk/TextTraits.hpp>
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver::FontReference

#include <string> // for std::string

namespace SUTK
{
	class SmallText;
	class UIDriver;
	
	// Abstract base class for Label class and PassiveLabel class
	class SUTK_API Label : public RenderableContainer
	{
	private:
		SmallText* m_text;
		HorizontalAlignment m_hAlign;
		VerticalAlignment m_vAlign;
		GfxDriverObjectHandleType m_ownedTextGroup { GFX_DRIVER_OBJECT_NULL_HANDLE };
		GfxDriverObjectHandleType m_textGroup { GFX_DRIVER_OBJECT_NULL_HANDLE };
		void createTextFirstTime() noexcept;
		// It is not recomendded to call this function directly in Derived Classes or anywhere else except this class itself, that's why it is in private access mode
		// Use setTextGroup() instead.
		// WARN: If you call this function directly then it would create an internal Text Group instance if GFX_DRIVER_OBJECT_NULL_HANDLE is passed.
		void setTextGroupForce(GfxDriverObjectHandleType textGroup) noexcept;
		// Must not be called outside of this Label class
		SmallText& getText() noexcept;
	protected:
		// Allowed to be called inside the Derived Classes
		void setTextGroup(GfxDriverObjectHandleType textGroup) noexcept;
	public:
		Label(UIDriver& driver, Container* parent = nullptr, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
		~Label() noexcept;
		void set(const std::string_view str) noexcept;
		const std::string& get() noexcept;
		void setAlignment(HorizontalAlignment hAlign, VerticalAlignment vAlign) noexcept;

		virtual Renderable* getRenderable() noexcept override;

		// We need to provide these functions on-behalf of owned SmallText object, we can be recreated at any time, as we can't expose reference to it
		// to prevent the user code storing the reference.
		f32 getHorizontalBound() const noexcept;
		void setColor(Color4 color) noexcept;
		Color4 getColor() const noexcept;
		void setFont(UIDriver::FontReference font) noexcept;
	};
}