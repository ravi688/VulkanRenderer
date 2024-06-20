/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: RenderWindow.hpp is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/


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