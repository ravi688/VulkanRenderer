/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: legal.c is a part of VulkanRenderer

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


#include <sge/legal/legal.h>

#include <stdio.h>

SGE_API void legal_show_notice(const char* path)
{
	printf("***NOTICE REGARDING THE USE OF THIS SOFTWARE*** \n"
    "%s (a.k.a VulkanRenderer @https://github.com/ravi688/VulkanRenderer.git)\n"
    "Copyright (C) Year: 2023  Author: Ravi Prakash Singh\n"
    "This program comes with ABSOLUTELY NO WARRANTY\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under certain conditions; see the LICENSE file in the root directory of the source of this program\n"
    "Electronic contact: rp0412204@gmail.com\n",
    path);
    fflush(stdout);
}
