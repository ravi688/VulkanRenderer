
#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/debug.h>
#include <phymac_parser/v3d_generic.h>

#include <shader_compiler/utilities/string.h>
#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/compiler/syntax.h>
#include <shader_compiler/compiler/codegen/codegen.h>

SC_API BUFFER* sc_compile(compiler_ctx_t* ctx)
{
	const char* start = ctx->src;
	// end should point to null character
	const char* end = start + ctx->src_len;
	remove_comments((char*)start, end);

	codegen_buffer_t* buffer = codegen_buffer_create();

	// write the file header
	ctx->src = start = write_header(start, end, buffer->main);

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

	/* perform syntax checking */
	syntax(result.root, ctx);
	/* generate code */
	codegen(result.root, ctx, buffer);

	BUFFER* f_buffer = codegen_buffer_flatten(buffer);
	debug_log_info("Compiled shader binary info: { size = %llu bytes }", buf_get_element_count(f_buffer));
	return f_buffer;
}
