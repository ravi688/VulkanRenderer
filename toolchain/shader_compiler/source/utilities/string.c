/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: string.c is a part of VulkanRenderer

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


#include <shader_compiler/utilities/string.h>
#include <shader_compiler/utilities/misc.h>
#include <common/debug.h>

SC_API const char* skip_whitespaces(const char* str, const char* const end)
{
	while((str < end) && isspace(*str)) str++;
	return str;
}

SC_API u32 __get_word_length(const char* str, const char* const end, const char* delimiters)
{
	u32 count = 0;
	while((str < end) && (!isspace(*str)) && (strchr(delimiters, *str) == NULL)) { str++; count++; }
	return count;
}


SC_API u32 get_word_length(const char* string, const char delimiter)
{
	u32 count = 0;
	while((!isspace(*string)) && (delimiter != (*string))) { string++; count++; }
	return count;
}

SC_API int safe_strncmp(const char* str1, const char* const str2, u32 len)
{
	int result = strncmp(str1, str2, min(strlen(str2), len));
	return result;
}

SC_API bool is_empty(const char* start, const char* const end)
{
	bool empty = true;
	while(start < end)
	{
		if(!isspace(*start))
		{
			empty = false;
			break;
		}
		++start;
	}
	return empty;
}

SC_API void remove_comments(char* start, const char* const end)
{
	const char* cmt_start = start;
	while(((end - cmt_start) >= 4) && ((cmt_start = strstr(cmt_start, "/*")) != NULL))
	{
		const char* cmt_end = strstr(cmt_start + 2, "*/");
		if(cmt_end == NULL)
			DEBUG_LOG_FETAL_ERROR("[Parse error]: Multiline comment is not closed");
		else cmt_end += 2;
		memset((void*)cmt_start, (int)(' '), cmt_end - cmt_start);
		cmt_start = cmt_end;
	}

	cmt_start = start;
	while(((end - cmt_start) >= 2) && ((cmt_start = strstr(cmt_start, "//")) != NULL))
	{
		const char* cmt_end = strchr(cmt_start + 2, '\n');
		/* //...EOF */
		if(cmt_end == NULL)
			cmt_end = end;
		/* //...\n */
		else cmt_end += 1;
		memset((void*)cmt_start, (int)(' '), cmt_end - cmt_start);
		cmt_start = cmt_end;
	}
}
