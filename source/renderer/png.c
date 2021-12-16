
#include <renderer/png.h>
#include <renderer/assert.h>
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>
#include <meshlib/parsers/binary.h>

/*this is the little endian version of the big endian png signature: 89 50 4E 47 0D 0A 1A 0A*/
#define PNG_SIGNATURE 0x0a1a0a0d474e5089L

typedef enum parse_error_t
{
	PNG_PARSE_ERROR_SIGNATURE_INVALID
} parse_error_t;

#define png_parse_error(...) define_alias_function_macro(png_parse_error, __VA_ARGS__)
function_signature(static void, png_parse_error, parse_error_t error)
{
	CALLTRACE_BEGIN();
	switch(error)
	{
		case PNG_PARSE_ERROR_SIGNATURE_INVALID: LOG_FETAL_ERR("PNG_PARSE_ERROR_SIGNATURE_INVALID\n"); break;
		default: LOG_FETAL_ERR("PNG_PARSE_ERROR (Unknown)\n"); break;
	}
	CALLTRACE_END();
}


function_signature(png_t, png_load, const char* file_path)
{
	CALLTRACE_BEGIN();
	LOG_FETAL_ERR("This method is undefined, it is yet to be defined completely\n");
	png_t png = { };
	BUFFER* file_data = load_binary_from_file(file_path);
	binary_parser_t parser = binary_parser_new(file_data->bytes, file_data->element_count);
	binary_parser_bind(&parser);

	/* png header */
	if(!binary_parser_cmp_u64(PNG_SIGNATURE))
		png_parse_error(PNG_PARSE_ERROR_SIGNATURE_INVALID);

	/* IHDR chunk*/
	printf("Content bytes: %u\n", binary_parser_u32());

	binary_parser_unbind();
	buf_free(file_data);
	CALLTRACE_RETURN(png);
}


void png_destroy(png_t png)
{

}
