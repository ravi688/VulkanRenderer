// A button has a two parts:
// 	1. Event publishing based on mouse clicks and pointer
// 	2. Graphical Representation and Feedback
//
// The first part can be implemented by deriving from SUTK::Container and, SUTK::MouseMoveHandlerObject and SUTK::MouseClickHandlerObjecxt classes.
// The second part can be implemented by devising an interface as 'IButtonGraphic' and any Graphical Representation object can implement this interface.

#pragma once

#include <sutk/defines.hpp> 
#include <sutk/Container.hpp>
#include <sutk/InputEventHandlerObject.hpp> // for SUTK::MouseMoveHandlerObject, and SUTK::MouseClickHandlerObject

namespace SUTK
{
	class IButtonGraphic;
	
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
		OnEnterEvent* m_onEnterEvent;
		OnExitEvent* m_onExitEvent;
		OnPressEvent* m_onPressEvent;
		OnReleaseEvent* m_onReleaseEvent;
		com::Bool m_isGraphicOwner;
		com::Bool m_isPressed;

		void checkAndOptimizeEvents() noexcept;

	protected:
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		// Override of onMouseMove function
		// Must be called in the overriding method
		virtual bool onMouseMove(MouseMoveEvent event, Vec2Df position) override;
		// Must be called in the overriding method
		virtual bool onMouseClick(MouseButton button, KeyEvent action) override;

	public:
		Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic, GfxDriverObjectHandleType textGroup) noexcept;
		Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic = true) noexcept;
		virtual ~Button() noexcept;

		// Must be called in the overriding method
		virtual void clearAllEvents() noexcept;

		// Must be called in the overriding method
		virtual void setGraphic(IButtonGraphic* graphic) noexcept;
		IButtonGraphic* getGraphic() noexcept { return m_graphic; }
		template<typename T> requires (std::derived_from<T, IButtonGraphic>)
		T* getGraphicAs() noexcept { return dynamic_cast<T*>(getGraphic()); }

		bool hasOnEnterEvent() noexcept { return m_onEnterEvent != NULL; }
		bool hasOnExitEvent() noexcept { return m_onExitEvent != NULL; }
		bool hasOnPressEvent() noexcept { return m_onPressEvent != NULL; }
		bool hasOnReleaseEvent() noexcept { return m_onReleaseEvent != NULL; }

		OnEnterEvent& getOnEnterEvent() noexcept;
		OnExitEvent& getOnExitEvent() noexcept;
		OnPressEvent& getOnPressEvent() noexcept;
		OnReleaseEvent& getOnReleaseEvent() noexcept;
	};
}