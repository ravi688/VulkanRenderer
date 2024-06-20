/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: visibility.h is a part of VulkanRenderer

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

// Copyright 2018 The Shaderc Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SHADERC_VISIBILITY_H_
#define SHADERC_VISIBILITY_H_

// SHADERC_EXPORT tags symbol that will be exposed by the shared libraries.
#if defined(SHADERC_SHAREDLIB)
#if defined(_WIN32)
#if defined(SHADERC_IMPLEMENTATION)
#define SHADERC_EXPORT __declspec(dllexport)
#else
#define SHADERC_EXPORT __declspec(dllimport)
#endif
#else
#if defined(SHADERC_IMPLEMENTATION)
#define SHADERC_EXPORT __attribute__((visibility("default")))
#else
#define SHADERC_EXPORT
#endif
#endif
#else
#define SHADERC_EXPORT
#endif

#endif  // SHADERC_VISIBILITY_H_
