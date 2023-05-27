/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: event_system.h is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/type_system.h>

typedef struct event_subscription_handle_t
{
	u64 publisher_handle;
	u64 event_handle;
} event_subscription_handle_t;

#define EVENT_SUBSCRIPTION_HANDLE_INVALID (event_subscription_handle_t) { ~0ULL, ~0ULL }
#define EVENT_SUBSCRIPTION_HANDLE_EQUAL(handle1, handle2) ((handle1.publisher_handle == handle2.publisher_handle) && (handle1.event_handle == handle2.event_handle))

typedef u64 event_type_t;

typedef struct event_t
{
	event_type_t event_type;
	type_t publisher_type;
} event_t;

typedef void (*event_listener_t)(event_t event, void* event_publisher_data, void* event_listener_data);

typedef struct event_system_t
{
 	// TODO
} event_system_t;

RENDERER_API event_system_t* event_system_new();
RENDERER_API event_system_t* event_system_create();
RENDERER_API void event_system_destroy(event_system_t* system);
RENDERER_API void event_system_release_resources(event_system_t* system);

RENDERER_API event_t* event_system_create_event(event_system_t* system, event_type_t event_type, type_t publisher_type);
RENDERER_API void event_system_destroy_event(event_system_t* system, event_t* event);
RENDERER_API event_subscription_handle_t event_system_subscribe(event_system_t* system, event_t* event, event_listener_t listener, void* listener_data);
RENDERER_API void event_system_unsubscribe(event_system_t* system, event_subscription_handle_t handle);
RENDERER_API void event_system_publish(event_system_t* system, event_t* event, void* event_publisher_data);
