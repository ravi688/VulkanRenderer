#pragma once

#include <sge/render_window.h>

#include <utility> /* for std::pair */

#include <sge-cpp/Event.hpp> /* for SGE::Event */

namespace SGE
{
	class SGE_API RenderWindow
	{
	private:
		render_window_t* m_handle;
		RenderWindow(render_window_t* window) : m_handle(window) { }
		friend class Driver;
		friend RenderWindow Event::ReinterpretPublisher<RenderWindow>(void* publisher);
	public:

		Event getOnResizeEvent() noexcept
		{
			return Event(render_window_get_on_resize_event(m_handle));
		}

		Event getOnCursorMoveEvent() noexcept
		{
			return Event(render_window_get_on_cursor_move_event(m_handle));
		}

		Event getOnMouseButtonEvent() noexcept
		{
			return Event(render_window_get_on_mouse_button_event(m_handle));
		}

		Event getOnScrollEvent() noexcept
		{
			return Event(render_window_get_on_scroll_event(m_handle));
		}

		Event getOnKeyEvent() noexcept
		{
			return Event(render_window_get_on_key_event(m_handle));
		}

		std::pair<u32, u32> getSize() const noexcept
		{
			u32 width, height;
			render_window_get_framebuffer_extent(m_handle, &width, &height);
			return { width, height };
		}

		vec2_t getCursorPosition() const noexcept
		{
			return render_window_get_cursor_position(m_handle);
		}

		vec2_t getScrollDelta() const noexcept
		{
			return render_window_get_scroll_delta(m_handle);
		}

		mouse_button_type_t getMouseButtonType() const noexcept
		{
			return render_window_get_mouse_button_type(m_handle);
		}

		key_event_type_t getKeyEventType() const noexcept
		{
			return render_window_get_key_event_type(m_handle);
		}

		void* getGLFWWindowPtr() noexcept
		{
			void* ptr;
			render_window_get_glfw_window_ptr(m_handle, &ptr);
			return ptr;
		}
	};

	template<> RenderWindow Event::ReinterpretPublisher<RenderWindow>(void* publisher);
}
