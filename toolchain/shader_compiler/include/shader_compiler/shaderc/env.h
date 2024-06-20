/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: env.h is a part of VulkanRenderer

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

#ifndef SHADERC_ENV_H_
#define SHADERC_ENV_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  shaderc_target_env_vulkan,  // SPIR-V under Vulkan semantics
  shaderc_target_env_opengl,  // SPIR-V under OpenGL semantics
  // NOTE: SPIR-V code generation is not supported for shaders under OpenGL
  // compatibility profile.
  shaderc_target_env_opengl_compat,  // SPIR-V under OpenGL semantics,
                                     // including compatibility profile
                                     // functions
  shaderc_target_env_webgpu,         // Deprecated, SPIR-V under WebGPU
                                     // semantics
  shaderc_target_env_default = shaderc_target_env_vulkan
} shaderc_target_env;

typedef enum {
  // For Vulkan, use Vulkan's mapping of version numbers to integers.
  // See vulkan.h
  shaderc_env_version_vulkan_1_0 = ((1u << 22)),
  shaderc_env_version_vulkan_1_1 = ((1u << 22) | (1 << 12)),
  shaderc_env_version_vulkan_1_2 = ((1u << 22) | (2 << 12)),
  // For OpenGL, use the number from #version in shaders.
  // TODO(dneto): Currently no difference between OpenGL 4.5 and 4.6.
  // See glslang/Standalone/Standalone.cpp
  // TODO(dneto): Glslang doesn't accept a OpenGL client version of 460.
  shaderc_env_version_opengl_4_5 = 450,
  shaderc_env_version_webgpu,  // Deprecated, WebGPU env never defined versions
} shaderc_env_version;

// The known versions of SPIR-V.
typedef enum {
  // Use the values used for word 1 of a SPIR-V binary:
  // - bits 24 to 31: zero
  // - bits 16 to 23: major version number
  // - bits 8 to 15: minor version number
  // - bits 0 to 7: zero
  shaderc_spirv_version_1_0 = 0x010000u,
  shaderc_spirv_version_1_1 = 0x010100u,
  shaderc_spirv_version_1_2 = 0x010200u,
  shaderc_spirv_version_1_3 = 0x010300u,
  shaderc_spirv_version_1_4 = 0x010400u,
  shaderc_spirv_version_1_5 = 0x010500u
} shaderc_spirv_version;

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SHADERC_ENV_H_
