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

		void checkAndOptimizeEvents() noexcept;

	protected:
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

	public:
		Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic = true, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;
		virtual ~Button() noexcept;

		// Overrides of onMouseEnter, onMouseExit, and onMouseMove functions
		// Must be called in the overriding method
		virtual void onMouseEnter() noexcept override;
		// Must be called in the overriding method
		virtual void onMouseExit() noexcept override;
		// Must be called in the overriding method
		virtual void onMouseMove(Vec2Df position) noexcept override;
		// Must be called in the overriding method
		virtual void onMouseClick(MouseButton button, KeyEvent action) noexcept override;

		// Must be called in the overriding method
		virtual void setGraphic(IButtonGraphic* graphic) noexcept;
		IButtonGraphic* getGraphic() noexcept { return m_graphic; }
		template<typename T> requires (std::derived_from<T, IButtonGraphic>)
		T* getGraphicAs() noexcept { return com::iknow_down_cast<T*>(getGraphic()); }

		OnEnterEvent& getOnEnterEvent() noexcept;
		OnExitEvent& getOnExitEvent() noexcept;
		OnPressEvent& getOnPressEvent() noexcept;
		OnReleaseEvent& getOnReleaseEvent() noexcept;
	};
}