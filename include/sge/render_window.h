/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: render_window.h is a part of VulkanRenderer

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

#include <sge/defines.h>
#include <sge/event.h>
#include <sge/type_system.h>
#include <sge/object.h>
#include <hpml/vec2.h>

static const type_t TYPE_ID(render_window_t) = TYPE_ID_CREATE(1);

typedef enum mouse_button_type_t
{
	MOUSE_BUTTON_TYPE_UNDEFINED,
	MOUSE_BUTTON_TYPE_LEFT,
	MOUSE_BUTTON_TYPE_RIGHT,
	MOUSE_BUTTON_TYPE_MIDDLE,
	MOUSE_BUTTON_TYPE_FORWARD,
	MOUSE_BUTTON_TYPE_BACKWARD
} mouse_button_type_t;

typedef enum key_event_type_t
{
	KEY_EVENT_TYPE_UNDEFINED,
	KEY_EVENT_TYPE_PRESS,
	KEY_EVENT_TYPE_RELEASE,
	KEY_EVENT_TYPE_REPEAT
} key_event_type_t;

typedef enum modifier_key_bits_t
{
	MODIFIER_KEY_SHIFT_BIT = BIT32(0),
	MODIFIER_KEY_CONTROL_BIT = BIT32(1),
	MODIFIER_KEY_ALT_BIT = BIT32(2),
	MODIFIER_KEY_WINDOWS_BIT = BIT32(3),
	MODIFIER_KEY_CMD_BIT = MODIFIER_KEY_WINDOWS_BIT,
	MODIFIER_KEY_CAPSLOCK_BIT = BIT32(4),
	MODIFIER_KEY_NUMLOCK_BIT = BIT32(5)
} modifier_key_bits_t;

typedef enum key_code_t
{
	KEY_CODE_UNDEFINED,
	KEY_CODE_A,
	KEY_CODE_B,
	KEY_CODE_C,
	KEY_CODE_D,
	KEY_CODE_E,
	KEY_CODE_F,
	KEY_CODE_G,
	KEY_CODE_H,
	KEY_CODE_I,
	KEY_CODE_J,
	KEY_CODE_K,
	KEY_CODE_L,
	KEY_CODE_M,
	KEY_CODE_N,
	KEY_CODE_O,
	KEY_CODE_P,
	KEY_CODE_Q,
	KEY_CODE_R,
	KEY_CODE_S,
	KEY_CODE_T,
	KEY_CODE_U,
	KEY_CODE_V,
	KEY_CODE_W,
	KEY_CODE_X,
	KEY_CODE_Y,
	KEY_CODE_Z,
	KEY_CODE_1_EXCLAIM,
	KEY_CODE_2_ATSIGN,
	KEY_CODE_3_HASH,
	KEY_CODE_4_DOLLAR,
	KEY_CODE_5_PERCENT,
	KEY_CODE_6_HAT,
	KEY_CODE_7_AMPERSAND,
	KEY_CODE_8_ASTERISK,
	KEY_CODE_9_OPEN_PAREN,
	KEY_CODE_0_CLOSE_PAREN,
	KEY_CODE_RETURN_ENTER,
	KEY_CODE_ESCAPE,
	KEY_CODE_DELETE_BACKSPACE,
	KEY_CODE_TAB,
	KEY_CODE_SPACEBAR,
	KEY_CODE_MINUS_UNDERSCORE,
	KEY_CODE_EQUAL_PLUS,
	KEY_CODE_OPEN_BRACKET_BRACE,
	KEY_CODE_CLOSE_BRACKET_BRACE,
	KEY_CODE_BACKSLASH_PILLOR,
	KEY_CODE_TILDE,
	KEY_CODE_SEMICOLON_COLON,
	KEY_CODE_QUOTE_SINGLE_DOUBLE,
	KEY_CODE_TILDE2,
	KEY_CODE_COMMA_LESS_THAN,
	KEY_CODE_FULLSTOP_GREATER_THAN,
	KEY_CODE_FORWARD_SLASH_QUESTION,
	KEY_CODE_CAPSLOCK,
	KEY_CODE_F1,
	KEY_CODE_F2,
	KEY_CODE_F3,
	KEY_CODE_F4,
	KEY_CODE_F5,
	KEY_CODE_F6,
	KEY_CODE_F7,
	KEY_CODE_F8,
	KEY_CODE_F9,
	KEY_CODE_F10,
	KEY_CODE_F11,
	KEY_CODE_F12,
	KEY_CODE_PRINTSCREEN,
	KEY_CODE_SCROLLLOCK,
	KEY_CODE_PAUSE,
	KEY_CODE_INSERT,
	KEY_CODE_HOME,
	KEY_CODE_PAGE_UP,
	KEY_CODE_DELETE_FORWARD,
	KEY_CODE_END,
	KEY_CODE_PAGE_DOWN,
	KEY_CODE_RIGHT_ARROW,
	KEY_CODE_LEFT_ARROW,
	KEY_CODE_DOWN_ARROW,
	KEY_CODE_UP_ARROW,
	KEY_CODE_KEYPAD_NUMLOCK_CLEAR,
	KEY_CODE_KEYPAD_FORWARD_SLASH,
	KEY_CODE_KEYPAD_ASTERISK,
	KEY_CODE_KEYPAD_MINUS,
	KEY_CODE_KEYPAD_PLUS,
	KEY_CODE_KEYPAD_ENTER,
	KEY_CODE_KEYPAD_1_END,
	KEY_CODE_KEYPAD_2_DOWN_ARROW,
	KEY_CODE_KEYPAD_3_PAGE_DOWN,
	KEY_CODE_KEYPAD_4_LEFT_ARROW,
	KEY_CODE_KEYPAD_5,
	KEY_CODE_KEYPAD_6_RIGHT_ARROW,
	KEY_CODE_KEYPAD_7_HOME,
	KEY_CODE_KEYPAD_8_UP_ARROW,
	KEY_CODE_KEYPAD_9_PAGE_UP,
	KEY_CODE_KEYPAD_0_INSERT,
	KEY_CODE_KEYPAD_DOT_DELETE,
	KEY_CODE_BACKSLASH_PILLOR_NEAR_LEFT_SHIFT,
	KEY_CODE_APPLICATION,
	KEY_CODE_POWER,
	KEY_CODE_KEYPAD_EQUAL,
	KEY_CODE_F13,
	KEY_CODE_F14,
	KEY_CODE_F15,
	KEY_CODE_F16,
	KEY_CODE_F17,
	KEY_CODE_F18,
	KEY_CODE_F19,
	KEY_CODE_F20,
	KEY_CODE_F21,
	KEY_CODE_F22,
	KEY_CODE_F23,
	KEY_CODE_F24,
	KEY_CODE_F25,
	KEY_CODE_EXECUTE,
	KEY_CODE_HELP,
	KEY_CODE_MENU,
	KEY_CODE_SELECT,
	KEY_CODE_STOP,
	KEY_CODE_AGAIN,
	KEY_CODE_UNDO,
	KEY_CODE_CUT,
	KEY_CODE_COPY,
	KEY_CODE_PASTE,
	KEY_CODE_FIND,
	KEY_CODE_MUTE,
	KEY_CODE_VOLUME_UP,
	KEY_CODE_VOLUME_DOWN,
	KEY_CODE_NUMLOCK,
	KEY_CODE_KEYPAD_COMMA,
	KEY_CODE_KEYPAD_EQUAL_SIGN,
	KEY_CODE_LEFT_CONTROL,
	KEY_CODE_LEFT_SHIFT,
	KEY_CODE_LEFT_ALT,
	KEY_CODE_LEFT_GUI,
	KEY_CODE_RIGHT_CONTROL,
	KEY_CODE_RIGHT_SHIFT,
	KEY_CODE_RIGHT_ALT,
	KEY_CODE_RIGHT_GUI,
	KEY_CODE_MAX
} key_code_t;

/* passed as arg in event_publish_arg for on_key_event */
typedef struct key_event_data_t
{
	key_code_t keycode;
	key_event_type_t event_type;
	modifier_key_bits_t modifiers;
} key_event_data_t;

typedef struct render_window_t
{
	__OBJECT__;
	memory_allocator_t* allocator;

	/* handle to the internal GLFWwindow object */
	void* handle;

	/* width of the window (in pixels) */
	u32 width;
	/* height of the window (in pixels) */
	u32 height;

	/* position of the cursor relative to top-left corner of the window (in pixels) */
	vec2_t cursor_pos;
	vec2_t scroll_delta;
	mouse_button_type_t mouse_button_type;
	key_event_type_t key_event_type;

	/* event triggered whenever the window resizes */
	event_t* on_resize_event;
	/* event triggered whenever the cursor moves */
	event_t* on_cursor_move_event;
	/* event triggered whenever a mouse button is pressed or released */
	event_t* on_mouse_button_event;
	/* event triggered whenever scroll wheel is turned/rotated */
	event_t* on_scroll_event;
	/* event triggered whenever a key on the keyboard is pressed/released */
	event_t* on_key_event;
} render_window_t;

#define RENDER_WINDOW(ptr) OBJECT_UP_CAST(render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CONST(ptr) OBJECT_UP_CAST(const render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CAST(ptr) OBJECT_TYPE_CAST(render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)
#define RENDER_WINDOW_CAST_CONST(ptr) OBJECT_TYPE_CAST_CONST(const render_window_t*, OBJECT_TYPE_RENDER_WINDOW, ptr)

BEGIN_CPP_COMPATIBLE

SGE_API render_window_t* render_window_init(memory_allocator_t* allocator, u32 width, u32 height, const char* title, bool full_screen, bool resizable);
SGE_API bool render_window_should_close(render_window_t* window);
SGE_API void render_window_poll_events(render_window_t* window);
SGE_API void render_window_destroy(render_window_t* window);

/* returns the last reported position of the cursor */
static CAN_BE_UNUSED_FUNCTION vec2_t render_window_get_cursor_position(render_window_t* window) { return window->cursor_pos; }
static CAN_BE_UNUSED_FUNCTION vec2_t render_window_get_scroll_delta(render_window_t* window) { return window->scroll_delta; }
static CAN_BE_UNUSED_FUNCTION mouse_button_type_t render_window_get_mouse_button_type(render_window_t* window) { return window->mouse_button_type; }
static CAN_BE_UNUSED_FUNCTION key_event_type_t render_window_get_key_event_type(render_window_t* window) { return window->key_event_type; }

static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE event_t* render_window_get_on_resize_event(render_window_t* window) { return window->on_resize_event; }
SGE_API event_t* render_window_get_on_cursor_move_event(render_window_t* window);
SGE_API event_t* render_window_get_on_mouse_button_event(render_window_t* window);
SGE_API event_t* render_window_get_on_scroll_event(render_window_t* window);
SGE_API event_t* render_window_get_on_key_event(render_window_t* window);

// getters
SGE_API void render_window_get_framebuffer_extent(render_window_t* window, u32* out_width, u32* out_height);

//API specific extensions
SGE_API void render_window_get_vulkan_surface(render_window_t* window, void* vk_driver, void* out_surface);

// Window System specific extension
// out_ptr: will be a double pointer (&(GLFWwindow*))
SGE_API void render_window_get_glfw_window_ptr(render_window_t* window, void* out_ptr);
END_CPP_COMPATIBLE
