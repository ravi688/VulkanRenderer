/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: conio.h is a part of VulkanRenderer

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

#include <renderer/defines.h>


BEGIN_CPP_COMPATIBLE

#if defined(PLATFORM_LINUX)
	// custom defined using ncurses
	static FORCE_INLINE char cio_getch() { /* TODO */ return 0; }
	static FORCE_INLINE bool cio_kbhit() { /* TODO */ return false; }
	#define getch cio_getch
	#define kbhit cio_kbhit
#elif defined(PLATFORM_WINDOWS)
#	ifdef COMPILER_MINGW
#		include <conio.h>
		static FORCE_INLINE char cio_getch() { return getch(); }
		static FORCE_INLINE bool cio_kbhit() { return kbhit(); }
#	else
		// custom defined using ncurses
		static FORCE_INLINE char cio_getch() { /* TODO */ return 0; }
		static FORCE_INLINE bool cio_kbhit() { /* TODO */ return false; }
		#define getch cio_getch
		#define kbhit cio_kbhit
#	endif // SGE_COMPILER_MINGW
#else
#	warning "Unrecognized platform you are compiling on, some features/functionalities might not work"
	static FORCE_INLINE char cio_getch() { return 0; }
	static FORCE_INLINE bool cio_kbit() { return false; }
	#define getch cio_getch
	#define kbhit cio_kbhit
#endif

END_CPP_COMPATIBLE
