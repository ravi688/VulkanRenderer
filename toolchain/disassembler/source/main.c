#include <disassembler/disasm.h>
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>

#include <disassembler/debug.h>
#include <disassembler/assert.h>

#include <stdio.h>
#include <stdlib.h>

NO_RETURN_FUNCTION static void display_help_and_exit(const char* application_name)
{
	printf(
		"%s: Help\n"
		"\t%s <input_file_name> \n"
		, application_name, application_name
	);
	exit(0);
}

#define cmd_args_error(...) __cmd_args_error(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
NO_RETURN_FUNCTION static void __cmd_args_error(u32 line, const char* function, const char* file, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	debug_logv("[Error] [Cmd Args] ", line, function, file, format, args);
	va_end(args);
	display_help_and_exit("disasm");
}

int main(int argc, char** argv)
{
	if(argc < 2)
		cmd_args_error("No enough arguments are given");

	BUFFER* bin = load_binary_from_file(argv[1]);
	BUFFER* str = disassemble(buf_get_ptr(bin), buf_get_element_count(bin));
	if(argc > 2)
		write_text_to_file(argv[2], buf_get_ptr(str));
	else
		puts(CAST_TO(const char*, buf_get_ptr(str)));

	debug_log_info("disassembly success");

	buf_free(bin);
	buf_free(str);
	return 0;
}
