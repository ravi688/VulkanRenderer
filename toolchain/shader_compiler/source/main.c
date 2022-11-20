
#include <shader_compiler/compiler/compiler.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/defines.h>
#include <shader_compiler/assert.h>
#include <disk_manager/file_writer.h>
#include <disk_manager/file_reader.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void display_help_and_exit(const char* application_name)
{
	printf(
		"%s: Help\n"
		"\t%s [-I<include_path>] <input_file_name> <output_file_name>\n"
		"\tor\n"
		"\t%s [-I<include_path>] <input_file_name>\n"
		, application_name, application_name, application_name
	);
	exit(0);
}

typedef struct cmd_args_parse_result_t
{
	/* path the vsc executable file */
	const char* exe_path;
	/* unordered list of include path to be searched for when include a file with <> brackets */
	BUFFER include_paths;
	/* ordered list of paths (it includes src path and sb path*/
	BUFFER paths;
} cmd_args_parse_result_t;

#define cmd_args_error(...) __cmd_args_error(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
static void __cmd_args_error(u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv("[Error] [Cmd Args] ", line, function, file, format, args);
	va_end(args);
	display_help_and_exit("vsc");
}

static cmd_args_parse_result_t* parse_cmd_args(const char* const* argv, int arg_count)
{
	cmd_args_parse_result_t* result = CAST_TO(cmd_args_parse_result_t*, malloc(sizeof(cmd_args_parse_result_t)));
	memset(result, 0, sizeof(cmd_args_parse_result_t));

	_assert(arg_count > 1);

	result->exe_path = argv[0];
	result->include_paths = buf_new(char*);				// list of null terminated strings
	result->paths = buf_new(char*);						// list of null terminated strings

	for(u32 i = 1; i < arg_count; i++)
	{
		const char* str = argv[i];
		u32 len = strlen(str);

		_assert(len != 0);

		/* option */
		if((len > 0) && (str[0] == '-'))
		{
			/* include path */
			if((len > 1) && (str[1] == 'I'))
			{
				if(len > 2)
				{
					const char* ptr = str + 2;
					buf_push(&result->include_paths, CAST_TO(void*, &ptr));
				}
				else
					cmd_args_error("Invalid include path is given");
			}
			else if(len == 1)
				cmd_args_error("Invalid option is given");
			else
				cmd_args_error("Unrecognized option \"%.*s\"", get_word_length(str + 1, 0), str + 1);
		}
		else
			buf_push(&result->paths, CAST_TO(void*, &str));
	}

	return result;
}

int main(int arg_count, const char* const* argv)
{
	cmd_args_parse_result_t* result = parse_cmd_args(argv, arg_count);

	u32 path_count = buf_get_element_count(&result->paths);

	_assert(path_count <= 2);

	if(path_count == 0)
		cmd_args_error("No source input found");

	char* sb_path = (path_count > 1) ? CAST_TO(char**, buf_pop_get_ptr(&result->paths))[0] : NULL;
	const char* src_path = CAST_TO(const char* const*, buf_pop_get_ptr(&result->paths))[0];

	if(sb_path == NULL)
	{
		sb_path = CAST_TO(char*, malloc(strlen(src_path) + strlen(".sb") + 1));
		strcpy(sb_path, src_path);
		
		char* ptr = strrchr(sb_path, '.');
		
		/* if the src_path has an extension */
		if(ptr != NULL)
		{
			ptr++;
			strcpy(ptr, "sb");
		}
		else
		{
			/* otherwise just append .sb */
			ptr = sb_path + strlen(sb_path);
			strcpy(ptr, ".sb");
		}
	}

	/* load the source file into memory */
	BUFFER* src = load_text_from_file(src_path);

	/* create compiler context */
	compiler_ctx_t* ctx = compiler_ctx_create();
	ctx->src = buf_get_ptr(src);
	ctx->src_len = buf_get_element_count(src);
	ctx->src_path = src_path;
	ctx->exe_path = result->exe_path;
	memcpy(&ctx->include_paths, &result->include_paths, sizeof(BUFFER));
	
	/* kick the compilation  */	
	BUFFER* sb = sc_compile(ctx);

	/* write the result into disk */
	write_binary_to_file(sb_path, buf_get_ptr(sb), buf_get_element_count(sb));

	/* release allocated memory */
	compiler_ctx_destroy(ctx);
	buf_free(src);
	buf_free(sb);;
}
