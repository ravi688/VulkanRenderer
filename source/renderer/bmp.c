
#include <renderer/bmp.h>
#include <renderer/assert.h>
#include <disk_manager/file_reader.h>
#include <meshlib/parsers/binary.h>
#include <memory_allocator/memory_allocator.h>

#define PARSE_ASSERT(boolean, ERROR) if(!(boolean)) bmp_parse_error(ERROR)

#define BMP_WINDOWS_HEADER_2BYTES "BM"

typedef enum parse_error_t
{
	BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID
} parse_error_t;

void bmp_parse_error(parse_error_t error)
{
	switch(error)
	{
		case BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID: LOG_FETAL_ERR("BMP_PARSE_ERROR_WINDOWS_HEADER_INVALID\n"); break;
	}
}

function_signature(bmp_t, bmp_load, const char* file_path)
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
	u8* data = heap_newv(u8, 4 * width * height);
	u8* dst = data;
	const u8* src = parser.bytes;
	u8 pad_channel_count = 4 - channel_count;
	assert(pad_channel_count >= 0);
	assert(pad_channel_count <= 4);
	while(raw_bitmap_data_size > 0)
	{
		memcpy(dst, src, channel_count);
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
		raw_bitmap_data_size -= channel_count;
	}
	assert((void*)dst <= ((void*)data + 4 * width * height));
	buf_free(file_data);
	CALLTRACE_RETURN((bmp_t) { .data = data, .channel_count = 4, .width = width, .height = height });
}


function_signature(void, bmp_destroy, bmp_t bmp)
{
	CALLTRACE_BEGIN();

	CALLTRACE_END();
}
