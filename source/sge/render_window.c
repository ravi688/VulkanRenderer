/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.c is a part of VulkanRenderer

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


#include <sge/render_window.h>
#include <sge/debug.h>
#include <sge/assert.h>
#include <sge/internal/vulkan/vulkan_defines.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#include <sge/internal/vulkan/vulkan_instance.h>
#include <sge/internal/vulkan/vulkan_allocator.h>
#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#ifdef PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(WINDOW_SYSTEM_WAYLAND)
#	define GLFW_EXPOSE_NATIVE_WAYLAND
#elif defined(WINDOW_SYSTEM_X11)
#	define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>
#include <memory.h>


#if GLOBAL_DEBUG
static void glfwErrorCallback(int code, const char* description)
{
	printf("GLFW: %d, %s", code, description);
}
#endif

static void glfwOnWindowResizeCallback(GLFWwindow* glfw_window, int width, int height)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	render_window_get_framebuffer_extent(window, &window->width, &window->height);
	event_publish(window->on_resize_event);
}

static void glfwOnWindowCloseCallback(GLFWwindow* glfw_window)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	event_publish(window->on_close_event);
}

static void glfwOnCursorMoveCallback(GLFWwindow* glfw_window, double x, double y)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->cursor_pos.x = x;
	window->cursor_pos.y = y;
	if(window->on_cursor_move_event != NULL)
		event_publish(window->on_cursor_move_event);
}

static void glfwOnScrollCallback(GLFWwindow* glfw_window, double x, double y)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->scroll_delta.x = x;
	window->scroll_delta.y = y;
	if(window->on_scroll_event != NULL)
		event_publish(window->on_scroll_event);
}

static key_event_type_t get_key_event_type(int event_type)
{
	switch(event_type)
	{
		case GLFW_PRESS: return KEY_EVENT_TYPE_PRESS;
		case GLFW_RELEASE: return KEY_EVENT_TYPE_RELEASE;
		case GLFW_REPEAT : return KEY_EVENT_TYPE_REPEAT;
		default:
		{
			debug_log_error("Unknown glfw event type");
			return KEY_EVENT_TYPE_UNDEFINED;
		}
	}
}

static mouse_button_type_t get_mouse_button_type(int button)
{
	switch(button)
	{
		case GLFW_MOUSE_BUTTON_LEFT: return MOUSE_BUTTON_TYPE_LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT: return MOUSE_BUTTON_TYPE_RIGHT;
		case GLFW_MOUSE_BUTTON_MIDDLE: return MOUSE_BUTTON_TYPE_MIDDLE;
		default:
		{
			debug_log_error("Uknown glfw mouse button type");
			return MOUSE_BUTTON_TYPE_UNDEFINED;
		}
	}
}

static void glfwOnMouseButtonCallback(GLFWwindow* glfw_window, int button, int event_type, int modifiers)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	window->mouse_button_type = get_mouse_button_type(button);
	window->key_event_type = get_key_event_type(event_type);
	if(window->on_mouse_button_event != NULL)
		event_publish(window->on_mouse_button_event);
}

static modifier_key_bits_t get_modifiers(int modifiers)
{
	modifier_key_bits_t bits = 0;
	if(HAS_FLAG(modifiers, GLFW_MOD_SHIFT))
		bits |= MODIFIER_KEY_SHIFT_BIT;
	if(HAS_FLAG(modifiers, GLFW_MOD_CONTROL))
		bits |= MODIFIER_KEY_CONTROL_BIT;
	if(HAS_FLAG(modifiers, GLFW_MOD_ALT))
		bits |= MODIFIER_KEY_ALT_BIT;
	if(HAS_FLAG(modifiers, GLFW_MOD_SUPER))
		bits |= MODIFIER_KEY_WINDOWS_BIT;
	if(HAS_FLAG(modifiers, GLFW_MOD_CAPS_LOCK))
		bits |= MODIFIER_KEY_CAPSLOCK_BIT;
	if(HAS_FLAG(modifiers, GLFW_MOD_NUM_LOCK))
		bits |= MODIFIER_KEY_NUMLOCK_BIT;
	return bits;
}

static key_code_t get_keycode(int key)
{
	switch(key)
	{
		case GLFW_KEY_UNKNOWN: return KEY_CODE_UNDEFINED;
		case GLFW_KEY_SPACE: return KEY_CODE_SPACEBAR;
		case GLFW_KEY_APOSTROPHE: return KEY_CODE_QUOTE_SINGLE_DOUBLE;
		case GLFW_KEY_COMMA: return KEY_CODE_COMMA_LESS_THAN;
		case GLFW_KEY_MINUS: return KEY_CODE_MINUS_UNDERSCORE;
		case GLFW_KEY_PERIOD: return KEY_CODE_FULLSTOP_GREATER_THAN;
		case GLFW_KEY_SLASH: return KEY_CODE_FORWARD_SLASH_QUESTION;
		case GLFW_KEY_0: return KEY_CODE_0_CLOSE_PAREN;
		case GLFW_KEY_1: return KEY_CODE_1_EXCLAIM;
		case GLFW_KEY_2: return KEY_CODE_2_ATSIGN;
		case GLFW_KEY_3: return KEY_CODE_3_HASH;
		case GLFW_KEY_4: return KEY_CODE_4_DOLLAR;
		case GLFW_KEY_5: return KEY_CODE_5_PERCENT;
		case GLFW_KEY_6: return KEY_CODE_6_HAT;
		case GLFW_KEY_7: return KEY_CODE_7_AMPERSAND;
		case GLFW_KEY_8: return KEY_CODE_8_ASTERISK;
		case GLFW_KEY_9: return KEY_CODE_9_OPEN_PAREN;
		case GLFW_KEY_SEMICOLON: return KEY_CODE_SEMICOLON_COLON;
		case GLFW_KEY_EQUAL: return KEY_CODE_EQUAL_PLUS;
		case GLFW_KEY_A: return KEY_CODE_A;
		case GLFW_KEY_B: return KEY_CODE_B;
		case GLFW_KEY_C: return KEY_CODE_C;
		case GLFW_KEY_D: return KEY_CODE_D;
		case GLFW_KEY_E: return KEY_CODE_E;
		case GLFW_KEY_F: return KEY_CODE_F;
		case GLFW_KEY_G: return KEY_CODE_G;
		case GLFW_KEY_H: return KEY_CODE_H;
		case GLFW_KEY_I: return KEY_CODE_I;
		case GLFW_KEY_J: return KEY_CODE_J;
		case GLFW_KEY_K: return KEY_CODE_K;
		case GLFW_KEY_L: return KEY_CODE_L;
		case GLFW_KEY_M: return KEY_CODE_M;
		case GLFW_KEY_N: return KEY_CODE_N;
		case GLFW_KEY_O: return KEY_CODE_O;
		case GLFW_KEY_P: return KEY_CODE_P;
		case GLFW_KEY_Q: return KEY_CODE_Q;
		case GLFW_KEY_R: return KEY_CODE_R;
		case GLFW_KEY_S: return KEY_CODE_S;
		case GLFW_KEY_T: return KEY_CODE_T;
		case GLFW_KEY_U: return KEY_CODE_U;
		case GLFW_KEY_V: return KEY_CODE_V;
		case GLFW_KEY_W: return KEY_CODE_W;
		case GLFW_KEY_X: return KEY_CODE_X;
		case GLFW_KEY_Y: return KEY_CODE_Y;
		case GLFW_KEY_Z: return KEY_CODE_Z;
		case GLFW_KEY_LEFT_BRACKET: return KEY_CODE_OPEN_BRACKET_BRACE;
		case GLFW_KEY_BACKSLASH: return KEY_CODE_BACKSLASH_PILLOR;
		case GLFW_KEY_RIGHT_BRACKET: return KEY_CODE_CLOSE_BRACKET_BRACE;
		case GLFW_KEY_GRAVE_ACCENT: return KEY_CODE_TILDE2;
		// case GLFW_KEY_WORLD_1: return KEY_CODE_WORLD_1;
		// case GLFW_KEY_WORLD_2: return KEY_CODE_WORLD_2;
		case GLFW_KEY_ESCAPE: return KEY_CODE_ESCAPE;
		case GLFW_KEY_ENTER: return KEY_CODE_RETURN_ENTER;
		case GLFW_KEY_TAB: return KEY_CODE_TAB;
		case GLFW_KEY_BACKSPACE: return KEY_CODE_DELETE_BACKSPACE;
		case GLFW_KEY_INSERT: return KEY_CODE_INSERT;
		case GLFW_KEY_DELETE: return KEY_CODE_DELETE_FORWARD;
		case GLFW_KEY_RIGHT: return KEY_CODE_RIGHT_ARROW;
		case GLFW_KEY_LEFT: return KEY_CODE_LEFT_ARROW;
		case GLFW_KEY_DOWN: return KEY_CODE_DOWN_ARROW;
		case GLFW_KEY_UP: return KEY_CODE_UP_ARROW;
		case GLFW_KEY_PAGE_UP: return KEY_CODE_PAGE_UP;
		case GLFW_KEY_PAGE_DOWN: return KEY_CODE_PAGE_DOWN;
		case GLFW_KEY_HOME: return KEY_CODE_HOME;
		case GLFW_KEY_END: return KEY_CODE_END;
		case GLFW_KEY_CAPS_LOCK: return KEY_CODE_CAPSLOCK;
		case GLFW_KEY_SCROLL_LOCK: return KEY_CODE_SCROLLLOCK;
		case GLFW_KEY_NUM_LOCK: return KEY_CODE_NUMLOCK;
		case GLFW_KEY_PRINT_SCREEN: return KEY_CODE_PRINTSCREEN;
		case GLFW_KEY_PAUSE: return KEY_CODE_PAUSE;
		case GLFW_KEY_F1: return KEY_CODE_F1;
		case GLFW_KEY_F2: return KEY_CODE_F2;
		case GLFW_KEY_F3: return KEY_CODE_F3;
		case GLFW_KEY_F4: return KEY_CODE_F4;
		case GLFW_KEY_F5: return KEY_CODE_F5;
		case GLFW_KEY_F6: return KEY_CODE_F6;
		case GLFW_KEY_F7: return KEY_CODE_F7;
		case GLFW_KEY_F8: return KEY_CODE_F8;
		case GLFW_KEY_F9: return KEY_CODE_F9;
		case GLFW_KEY_F10: return KEY_CODE_F10;
		case GLFW_KEY_F11: return KEY_CODE_F11;
		case GLFW_KEY_F12: return KEY_CODE_F12;
		case GLFW_KEY_F13: return KEY_CODE_F13;
		case GLFW_KEY_F14: return KEY_CODE_F14;
		case GLFW_KEY_F15: return KEY_CODE_F15;
		case GLFW_KEY_F16: return KEY_CODE_F16;
		case GLFW_KEY_F17: return KEY_CODE_F17;
		case GLFW_KEY_F18: return KEY_CODE_F18;
		case GLFW_KEY_F19: return KEY_CODE_F19;
		case GLFW_KEY_F20: return KEY_CODE_F20;
		case GLFW_KEY_F21: return KEY_CODE_F21;
		case GLFW_KEY_F22: return KEY_CODE_F22;
		case GLFW_KEY_F23: return KEY_CODE_F23;
		case GLFW_KEY_F24: return KEY_CODE_F24;
		case GLFW_KEY_F25: return KEY_CODE_F25;
		case GLFW_KEY_KP_0: return KEY_CODE_KEYPAD_0_INSERT;
		case GLFW_KEY_KP_1: return KEY_CODE_KEYPAD_1_END;
		case GLFW_KEY_KP_2: return KEY_CODE_KEYPAD_2_DOWN_ARROW;
		case GLFW_KEY_KP_3: return KEY_CODE_KEYPAD_3_PAGE_DOWN;
		case GLFW_KEY_KP_4: return KEY_CODE_KEYPAD_4_LEFT_ARROW;
		case GLFW_KEY_KP_5: return KEY_CODE_KEYPAD_5;
		case GLFW_KEY_KP_6: return KEY_CODE_KEYPAD_6_RIGHT_ARROW;
		case GLFW_KEY_KP_7: return KEY_CODE_KEYPAD_7_HOME;
		case GLFW_KEY_KP_8: return KEY_CODE_KEYPAD_8_UP_ARROW;
		case GLFW_KEY_KP_9: return KEY_CODE_KEYPAD_9_PAGE_UP;
		case GLFW_KEY_KP_DECIMAL: return KEY_CODE_KEYPAD_DOT_DELETE;
		case GLFW_KEY_KP_DIVIDE: return KEY_CODE_KEYPAD_FORWARD_SLASH;
		case GLFW_KEY_KP_MULTIPLY: return KEY_CODE_KEYPAD_ASTERISK;
		case GLFW_KEY_KP_SUBTRACT: return KEY_CODE_KEYPAD_MINUS;
		case GLFW_KEY_KP_ADD: return KEY_CODE_KEYPAD_PLUS;
		case GLFW_KEY_KP_ENTER: return KEY_CODE_KEYPAD_ENTER;
		case GLFW_KEY_KP_EQUAL: return KEY_CODE_KEYPAD_EQUAL;
		case GLFW_KEY_LEFT_SHIFT: return KEY_CODE_LEFT_SHIFT;
		case GLFW_KEY_LEFT_CONTROL: return KEY_CODE_LEFT_CONTROL;
		case GLFW_KEY_LEFT_ALT: return KEY_CODE_LEFT_ALT;
		case GLFW_KEY_LEFT_SUPER: return KEY_CODE_LEFT_GUI;
		case GLFW_KEY_RIGHT_SHIFT: return KEY_CODE_RIGHT_SHIFT;
		case GLFW_KEY_RIGHT_CONTROL: return KEY_CODE_RIGHT_CONTROL;
		case GLFW_KEY_RIGHT_ALT: return KEY_CODE_RIGHT_ALT;
		case GLFW_KEY_RIGHT_SUPER: return KEY_CODE_RIGHT_GUI;
		case GLFW_KEY_MENU: return KEY_CODE_APPLICATION;
		default: return KEY_CODE_MAX;
	};
}

static void glfwOnKeyCallback(GLFWwindow* glfw_window, int key, int scan_code, int event_type, int modifiers)
{
	AUTO window = CAST_TO(render_window_t*, glfwGetWindowUserPointer(glfw_window));
	if(window->on_key_event != NULL)
	{
		key_event_data_t data = 
		{
			.keycode = get_keycode(key),
			.event_type = get_key_event_type(event_type),
			.modifiers = get_modifiers(modifiers)
		};
		event_publish_arg(window->on_key_event, CAST_TO(void*, &data));
	}
}

SGE_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable)
{
	render_window_t* window = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_RENDER_WINDOW, render_window_t);
	memzero(window, render_window_t);
	OBJECT_INIT(window, OBJECT_TYPE_RENDER_WINDOW, OBJECT_NATIONALITY_INTERNAL);
	window->allocator = allocator;
	window->on_resize_event = event_create(allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Resize"));
	window->on_close_event = event_create(allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Close"));
	glfwInit();
#if GLOBAL_DEBUG
	glfwSetErrorCallback(glfwErrorCallback);
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
	if(full_screen)
		window->handle = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
	else
		window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetFramebufferSizeCallback(window->handle, glfwOnWindowResizeCallback);
	glfwSetWindowCloseCallback(window->handle, glfwOnWindowCloseCallback);
	glfwSetCursorPosCallback(window->handle, glfwOnCursorMoveCallback);
	glfwSetMouseButtonCallback(window->handle, glfwOnMouseButtonCallback);
	glfwSetScrollCallback(window->handle, glfwOnScrollCallback);
	glfwSetInputMode(window->handle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetKeyCallback(window->handle, glfwOnKeyCallback);
	glfwSetWindowUserPointer(window->handle, window);
	render_window_get_framebuffer_extent(window, &window->width, &window->height);
	log_msg("Render window created successfully\n");
	return window;
}

SGE_API void render_window_set_should_close(render_window_t* window, bool is_close)
{
	glfwSetWindowShouldClose(window->handle, is_close ? GLFW_TRUE : GLFW_FALSE);
}

SGE_API bool render_window_should_close(render_window_t* window)
{
	return glfwWindowShouldClose(window->handle);
}

SGE_API void render_window_poll_events(render_window_t* window)
{
	glfwPollEvents();
}

SGE_API void render_window_destroy(render_window_t* window)
{
	_debug_assert__(OBJECT_IS_INTERNAL(window));

	if(window->on_cursor_move_event != NULL)
	{
		event_destroy(window->on_cursor_move_event);
		event_release_resources(window->on_cursor_move_event);
	}
	if(window->on_mouse_button_event != NULL)
	{
		event_destroy(window->on_mouse_button_event);
		event_release_resources(window->on_mouse_button_event);
	}
	if(window->on_scroll_event != NULL)
	{
		event_destroy(window->on_scroll_event);
		event_release_resources(window->on_scroll_event);
	}
	if(window->on_key_event != NULL)
	{
		event_destroy(window->on_key_event);
		event_release_resources(window->on_key_event);
	}
	event_destroy(window->on_close_event);
	event_release_resources(window->on_close_event);
	event_destroy(window->on_resize_event);
	event_release_resources(window->on_resize_event);
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	memory_allocator_dealloc(window->allocator, window);
	log_msg("Render window destroyed successfully\n");
}

SGE_API event_t* render_window_get_on_cursor_move_event(render_window_t* window)
{
	if(window->on_cursor_move_event == NULL)
		window->on_cursor_move_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Cursor-Move"));
	return window->on_cursor_move_event;
}

SGE_API event_t* render_window_get_on_mouse_button_event(render_window_t* window)
{
	if(window->on_mouse_button_event == NULL)
		window->on_mouse_button_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Mouse-Button"));
	return window->on_mouse_button_event;
}

SGE_API event_t* render_window_get_on_scroll_event(render_window_t* window)
{
	if(window->on_scroll_event == NULL)
		window->on_scroll_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-Mouse-Scroll"));
	return window->on_scroll_event;
}

SGE_API event_t* render_window_get_on_key_event(render_window_t* window)
{
	if(window->on_key_event == NULL)
		window->on_key_event = event_create(window->allocator, (void*)window PARAM_IF_DEBUG("Render-Window-On-Key"));
	return window->on_key_event;
}

SGE_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height)
{
	int height, width;
	glfwGetFramebufferSize(window->handle, &width, &height);
	*out_width = width;
	*out_height = height;
}

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, vulkan_renderer_t* driver)
{
	VkSurfaceKHR surface;
	vkCall(glfwCreateWindowSurface(driver->instance->handle, window, VULKAN_ALLOCATION_CALLBACKS(driver), &surface));
	log_msg("Vulkan surface created successfully\n");
	return surface;
}

SGE_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface)
{
	VkSurfaceKHR surface = glfw_get_vulkan_surface(window->handle, CAST_TO(vulkan_renderer_t*, vk_driver));
	memcpy(out_surface, &surface, sizeof(VkSurfaceKHR));
}

SGE_API void render_window_get_glfw_window_ptr(render_window_t* window, void* out_ptr)
{
	memcpy(out_ptr, &window->handle, sizeof(void*));
}

#ifdef PLATFORM_WINDOWS
#	include <windows.h>
#endif

SGE_API u32 render_window_get_native_size(render_window_t* window)
{
#ifdef PLATFORM_WINDOWS
	return sizeof(HWND);
#elif defined(WINDOW_SYSTEM_WAYLAND)
	return sizeof(struct wl_surface*);
#elif defined(WINDOW_SYSTEM_X11)
	return sizeof(Window);
#else
	return 0;
#endif
}
SGE_API void render_window_get_native(render_window_t* window, void* const out)
{
	#ifdef PLATFORM_WINDOWS
	HWND handle = glfwGetWin32Window(CAST_TO(GLFWwindow*, window->handle));
	memcpy(out, CAST_TO(void*, &handle), sizeof(HWND));
	#elif defined(WINDOW_SYSTEM_WAYLAND)
	struct wl_surface* surface = glfwGetWaylandWindow(CAST_TO(GLFWwindow*, window->handle));
	memcpy(out, CAST_TO(void*, &surface), sizeof(surface));
	#elif defined(WINDOW_SYSTEM_X11)
	Window x11window = glfwGetX11Window(CAST_TO(GLFWwindow*, window->handle));
	memcpy(out, CAST_TO(void*, &x11window), sizeof(x11window));
	#else
		DEBUG_LOG_WARNING("Couldn't get native handle for this platform");
	#endif
}
