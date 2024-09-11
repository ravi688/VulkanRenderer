#pragma once

#include <sutk/defines.hpp>
#include <sutk/RenderRectFillRound.hpp> // for SUTK::RenderRectFillRound
#include <sutk/ColorTransitionAutomaton.hpp> // for SUTK::ColorTransitionAutomaton
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer

namespace SUTK
{
	struct HoverInfo
	{
		Vec2Df position;
		bool isEnter;
		bool isExit;
	};

	class SUTK_API IButtonGraphic
	{
	public:
		// Called every time mouse pointer changes its position but still stays within the boundaries of Button's Rect
		// position is in local coordinates of the Button's Rect
		virtual void onHover(HoverInfo info) = 0;
		// Called once for each left-mouse-button down within the boundaries of Button's Rect
		virtual void onPress() = 0;
		// Called once for each left-mouse-button up/release within the boundaries of Button's Rect or even outside if earlier press was within the boundaries.
		virtual void onRelease() = 0;
		virtual Vec2Df getMinBoundSize() = 0;
	};

	class SUTK_API VisualButtonRect : public RenderRectFillRound
	{
		// States
		enum State : u32
		{
			Hover,
			Press,
			Idle
		};

		ColorTransitionAutomaton m_cta;

		friend class DefaultButtonGraphicNoLabel;
	public:
		VisualButtonRect(SUTK::UIDriver& driver, SUTK::RenderableContainer* container) noexcept;

		virtual void setActive(bool isActive) noexcept override;

		virtual bool isDirty() noexcept override;
		virtual void update() noexcept override;

		void setTransitionDelay(const f32 transitionDelay) noexcept;

		void setHoverColor(Color4 color) noexcept;
		void setPressColor(Color4 color) noexcept;
		void setIdleColor(Color4 color) noexcept;


		void setState(State state) noexcept;
	};


	class SUTK_API DefaultButtonGraphicNoLabel : public RenderableContainer, public IButtonGraphic
	{
	private:

		// Visual Representation of the Button's existence
		VisualButtonRect* m_visualButton;

	public:
		DefaultButtonGraphicNoLabel(UIDriver& driver, Container* parent) noexcept;

		// Implementation of IButtonGraphic's functions
		virtual void onHover(HoverInfo info) noexcept override;
		virtual void onPress() noexcept override;
		virtual void onRelease() noexcept override;
		virtual Vec2Df getMinBoundSize() noexcept override;

		VisualButtonRect& getVisualButtonRect() noexcept { return *m_visualButton; }
	};

	class Label;
	class SUTK_API DefaultButtonGraphic : public DefaultButtonGraphicNoLabel
	{
		private:		
			Label* m_label;
		public:
			DefaultButtonGraphic(UIDriver& driver, Container* parent) noexcept;

			// Overrides
			virtual Vec2Df getMinBoundSize() noexcept override;

			Label& getLabel() noexcept { return *m_label; }
	};
}