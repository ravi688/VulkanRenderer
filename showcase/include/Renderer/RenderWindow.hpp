
#pragma once

#include <Showcase/Defines.hpp>
#include <renderer/render_window.h> 			// C header

namespace V3D
{
	class SHOWCASE_API RenderWindow
	{
	private:
		render_window_t* handle;

		friend class Renderer;
		RenderWindow(u32 width, u32 height, const char* title, bool isFullScreen = false);
	
	public:

		inline RenderWindow(render_window_t* window) : handle(window) { }

		inline u32 getWidth() const { return handle->width; }
		inline u32 getHeight() const { return handle->height; }
		
		inline bool shouldClose() const { return render_window_should_close(handle); }
		inline void pollEvents() const { render_window_poll_events(handle); }
		inline void subscribeOnResize(void (*callback)(RenderWindow window, void* userData), void* userData) const
		{
			render_window_subscribe_on_resize(handle, reinterpret_cast<void (*)(render_window_t*, void*)>(callback), userData);
		}
		inline void unsubscribeOnResize(void (*callback)(RenderWindow window, void* userData)) const
		{
			render_window_unsubscribe_on_resize(handle, reinterpret_cast<void (*)(render_window_t*, void*)>(callback));
		}
	};
}