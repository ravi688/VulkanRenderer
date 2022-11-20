
#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/debug.h>
#include <disk_manager/file_reader.h>
#include <phymac_parser/v3d_generic.h>

#include <shader_compiler/utilities/string.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/syntax.h>
#include <shader_compiler/compiler/codegen/codegen.h>

SC_API BUFFER* sc_load_and_compile(const char* file_path)
{
	BUFFER* buffer = load_text_from_file(file_path);
	BUFFER* sb = sc_compile(buffer->bytes, buffer->element_count);
	buf_free(buffer);
	return sb;
}

SC_API BUFFER* sc_compile(const char* start, u32 length)
{
	// end should point to null character
	const char* end = start + length;
	remove_comments((char*)start, end);

	codegen_buffer_t* buffer = codegen_buffer_create();

	// write the file header
	start = write_header(start, end, buffer->main);

	// parse the source code (perhaps partially)
	ppsr_v3d_generic_parse_result_t result = ppsr_v3d_generic_parse(start, CAST_TO(u32, end - start));
	switch(result.result)
	{
		case PPSR_SUCCESS:
			break;
		case PPSR_WARNING:
			debug_log_warning(result.log_buffer);
			break;
		case PPSR_ERROR:
			debug_log_fetal_error(result.log_buffer);
			break;
		case PPSR_ERROR_UNKOWN:
			debug_log_fetal_error("Unknown error has been occured while parsing the shader");
			break;
		default:
			debug_log_fetal_error("Invalid or Unrecognized result code recieved from the parser");
	}

	compiler_ctx_t* ctx = compiler_ctx_create(start);

	/* perform syntax checking */
	syntax(result.root, ctx);
	/* generate code */
	codegen(result.root, ctx, buffer);

	compiler_ctx_destroy(ctx);

	BUFFER* f_buffer = codegen_buffer_flatten(buffer);
	debug_log_info("Compiled shader binary info: { size = %llu bytes }", buf_get_element_count(f_buffer));
	return f_buffer;
}
