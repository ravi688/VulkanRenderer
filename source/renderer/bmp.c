/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: bmp.c is a part of VulkanRenderer

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


#include <renderer/bmp.h>
#include <renderer/assert.h>
#include <disk_manager/file_reader.h>
#include <meshlib/parsers/binary.h>
#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/debug.h>
#include <stdio.h> // FILE
#include <stb/stb_image_write.h>

#define PARSE_ASSERT(boolean, ERROR) if(!(boolean)) bmp_parse_error(ERROR)

#define BMP_WINDOWS_HEADER_2BYTES "BM"

typedef enum parse_error_t
{
	BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID
} parse_error_t;

static void bmp_parse_error(parse_error_t error)
{
	switch(error)
	{
		case BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID: LOG_FETAL_ERR("BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID\n"); break;
	}
}

RENDERER_API function_signature(bmp_t, bmp_load, memory_allocator_t* allocator, const char* file_path)
{
	CALLTRACE_BEGIN();
	BUFFER* file_data = load_binary_from_file(file_path);
	binary_parser_t parser = binary_parser_new(file_data->bytes, file_data->element_count); binary_parser_bind(&parser);

	/* Header */
	PARSE_ASSERT(binary_parser_cmp_bytes(BMP_WINDOWS_HEADER_2BYTES, 2), BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID);
	u32 bmp_size = binary_parser_u32();
	binary_parser_skip_bytes(4);
	/* Offset where the pixel array (bitmap data) can be found */
	u32 offset = binary_parser_u32();

	binary_parser_skip_bytes(4);
	u32 width = binary_parser_u32();
	u32 height = binary_parser_u32();
	/* Skip number of planes, because it would be always one for Bitmap images*/
	binary_parser_skip_bytes(2);
	/* Number of bits per pixel */
	u32 bpp = binary_parser_u16();
	/* Skip BI_BITFIELDS*/
	binary_parser_skip_bytes(4);
	/* Size of the raw bitmap data */
	u32 raw_bitmap_data_size = binary_parser_u32();
	/* Reset the internal cursor to the initial position */
	binary_parser_rewind();
	/* Move to Pixel Array*/
	binary_parser_skip_bytes(offset);

	u8 channel_count = (bpp >> 3); /* (Bits Per Pixels) / Bits per byte*/
	u8* data = CAST_TO(u8*, memory_allocator_alloc(allocator, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, sizeof(u8) * 4 * width * height));
	u8* dst = data;
	const u8* src = parser.bytes;
	u8 pad_channel_count = 4 - channel_count;
	_debug_assert__(pad_channel_count >= 0);
	_debug_assert__(pad_channel_count <= 4);
	u32 pixel_count = width * height;
	while(pixel_count > 0)
	{
		memcopyv(dst, src, u8, channel_count);
		dst += channel_count;
		src += channel_count;
		for(u8 i = 0; i < pad_channel_count; i++)
		{
			switch(i)
			{
				case 0: *dst = 255; dst++; break;
				case 1:
				case 2:
						//TODO: this should be: ref(u8, dst, 0) = 255; dst++; but safe memory only checks for the initial base address, not the intermediate
				case 3: *dst = 0; dst++; break;
			}
		}
		--pixel_count;
	}
	assert((void*)dst <= ((void*)data + 4 * width * height));
	buf_free(file_data);
	CALLTRACE_RETURN((bmp_t) { .allocator = allocator, .data = data, .channel_count = 4, .width = width, .height = height });
}


RENDERER_API function_signature(void, bmp_destroy, bmp_t bmp)
{
	CALLTRACE_BEGIN();
	memory_allocator_dealloc(bmp.allocator, bmp.data);
	CALLTRACE_END();
}

#define write(file, bytes, count) __write(file, (void*)(bytes), count)
static void __write(FILE* file, void* bytes, u32 count)
{
	fwrite((const char*)bytes, 1, count, file);
}

typedef u16 WORD;
typedef u32 DWORD;
typedef s32 LONG;

static void DWORD_write(FILE* file, DWORD value)
{
	write(file, &value, 4);
}

static void LONG_write(FILE* file, LONG value)
{
	write(file, &value, 4);
}

static void WORD_write(FILE* file, WORD value)
{
	write(file, &value, 2);
}

RENDERER_API function_signature(void, bmp_write, void* data, u32 width, u32 height, u8 channel_count, const char* file_path)
{
	CALLTRACE_BEGIN();

	// /* **NOTE** we are writing uncompressed pixel data */

	// FILE* file = fopen(file_path, "w");

	// u32 pixel_data_size = (width * channel_count + width % 4) * height;
	
	// /* write bitmap file header */
	// WORD_write(file, BIT16_PACK8('M', 'B'));
	// DWORD_write(file, 54 + pixel_data_size);
	// WORD_write(file, 0);
	// WORD_write(file, 0);
	// DWORD_write(file, 54);

	// /* write bitmap info header */
	// DWORD_write(file, 40);
	// LONG_write(file, width);
	// LONG_write(file, height);
	// WORD_write(file, 1);
	// WORD_write(file, channel_count * 8);
	// DWORD_write(file, 0);
	// DWORD_write(file, 0);
	// LONG_write(file, 0);
	// LONG_write(file, 0);
	// DWORD_write(file, 0);
	// DWORD_write(file, 0);

	// /* NOTE: Here pixel data is interpreted as laid out in row major form starting from left 
	//  * Hence, start of the texture (0, 0) is on the top-left of the bitmap texture */

	// u32 horizontal_stride = width * channel_count;
	// u32 size = width * height * channel_count;
	// void* _data = data + size - horizontal_stride;

	// /* write pixel data */
	// for(u32 i = 0; i < height; i++, _data -= horizontal_stride)
	// {
	// 	for(u32 j = 0; j < width; j++)
	// 	{
	// 		u8* pixel = CAST_TO(u8*, _data + j * channel_count);
			
	// 		u8 swizzle[channel_count];
	// 		for(u32 k = 0; k < channel_count; k++)
	// 			swizzle[k] = pixel[channel_count - 1 - k];

	// 		write(file, swizzle, channel_count);
	// 	}
	// 	for(u32 j = 0; j < (width % 4); j++)
	// 		fputc(0, file);
	// }

	// fflush(file);
	// fclose(file);

	stbi_write_bmp(file_path, width, height, channel_count, data);

	CALLTRACE_END();
}

