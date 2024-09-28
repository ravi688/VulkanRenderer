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
		OnEnterEvent m_onEnterEvent;
		OnExitEvent m_onExitEvent;
		OnPressEvent m_onPressEvent;
		OnReleaseEvent m_onReleaseEvent;

	public:
		Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic = true, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

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