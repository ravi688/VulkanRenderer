// A button has a two parts:
// 	1. Event publishing based on mouse clicks and pointer
// 	2. Graphical Representation and Feedback
//
// The first part can be implemented by deriving from SUTK::Container and, SUTK::MouseMoveHandlerObject and SUTK::MouseClickHandlerObjecxt classes.
// The second part can be implemented by devising an interface as 'IButtonGraphic' and any Graphical Representation object can implement this interface.

#pragma once

#include <sutk/defines.hpp> 
#include <sutk/Container.hpp>
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer
#include <sutk/RenderRect.hpp> // for SUTK::RenderRectFillRound
#include <sutk/ColorTransitionAutomaton.hpp> // for SUTK::ColorTransitionAutomaton
#include <sutk/InputEventHandlerObject.hpp> // for SUTK::MouseMoveHandlerObject, and SUTK::MouseClickHandlerObject

namespace SUTK
{
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

		friend class DefaultButtonGraphic;
	public:
		VisualButtonRect(SUTK::UIDriver& driver, SUTK::RenderableContainer* container) noexcept;

		virtual bool isDirty() noexcept override;
		virtual void update() noexcept override;

		void setTransitionDelay(const f32 transitionDelay) noexcept;

		void setHoverColor(Color4 color) noexcept;
		void setPressColor(Color4 color) noexcept;
		void setIdleColor(Color4 color) noexcept;


		void setState(State state) noexcept;
	};

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
	};

	class SmallText;
	
	class SUTK_API Label : public RenderableContainer
	{
	private:
		SmallText* m_text;
	public:
		Label(UIDriver& driver, Container* parent) noexcept;

		void set(const std::string& str) noexcept;
	};

	class SUTK_API DefaultButtonGraphic : public RenderableContainer, public IButtonGraphic
	{
	private:

		// Visual Representation of the Button's existence
		VisualButtonRect* m_visualButton;
		Label* m_label;

	public:
		DefaultButtonGraphic(UIDriver& driver, Container* parent) noexcept;

		// Implementation of IButtonGraphic's functions
		virtual void onHover(HoverInfo info) noexcept override;
		virtual void onPress() noexcept override;
		virtual void onRelease() noexcept override;

		VisualButtonRect& getVisualButtonRect() noexcept { return *m_visualButton; }
	};

	class SUTK_API Button : public Container, 
							public MouseMoveHandlerObject, 
							public MouseClickHandlerObject
	{
	public:
		typedef com::Event<Button> OnEnterEvent;
		typedef com::Event<Button> OnExitEvent;
		typedef com::Event<Button> OnPressEvent;
		typedef com::Event<Button> OnReleaseEvent;
	private:
		IButtonGraphic* m_graphic;
		OnEnterEvent m_onEnterEvent;
		OnExitEvent m_onExitEvent;
		OnPressEvent m_onPressEvent;
		OnReleaseEvent m_onReleaseEvent;

	public:
		Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic = true) noexcept;

		// Overrides of onMouseEnter, onMouseExit, and onMouseMove functions
		virtual void onMouseEnter() noexcept override;
		virtual void onMouseExit() noexcept override;
		virtual void onMouseMove(Vec2Df position) noexcept override;
		virtual void onMouseClick(MouseButton button, KeyEvent action) noexcept override;

		void setGraphic(IButtonGraphic* graphic) noexcept { m_graphic = graphic; }
		IButtonGraphic* getGraphic() noexcept { return m_graphic; }
		template<typename T>
		T* getGraphicAs() noexcept { return static_cast<T*>(getGraphic()); }

		OnEnterEvent& getOnEnterEvent() noexcept { return m_onEnterEvent; }
		OnExitEvent& getOnExitEvent() noexcept { return m_onExitEvent; }
		OnPressEvent& getOnPressEvent() noexcept { return m_onPressEvent; }
		OnReleaseEvent& getOnReleaseEvent() noexcept { return m_onReleaseEvent; }
	};
}