
#include <shader_compiler/compiler.h>
#include <disk_manager/file_reader.h>
#include <disk_manager/file_writer.h>
// #include <shader_compiler/standard_library.h>
#include <shader_compiler/assert.h>
#include <stdlib.h>
#include <string.h>

void display_help_and_exit(const char* application_name)
{
	printf(
		"%s: Help\n"
		"\t%s <input_file_name> <output_file_name>\n"
		"\tor\n"
		"\t%s <input_file_name>\n"
		, application_name, application_name, application_name
	);
	exit(0);
}

int main(int arg_count, const char* const* argv)
{
	if(arg_count == 1) display_help_and_exit(argv[0]);

	const char* input_file_name = argv[1];
	char* output_file_name;
	uint32_t output_file_name_length;
	if(arg_count == 2)
	{
		output_file_name = alloca(strlen(input_file_name) + strlen(".sb") + 1);
		char* ptr = strrchr(input_file_name, '.');
		strncpy(output_file_name, input_file_name, ptr - input_file_name);
		strcat(output_file_name, ".sb");
	}
	else
	{
		output_file_name = alloca(strlen(argv[2]) + 1);
		strcpy(output_file_name, argv[2]);
	}
	BUFFER* binary = shader_compiler_load_and_compile(input_file_name);
	write_binary_to_file(output_file_name, binary->bytes, binary->element_count);
	buf_free(binary);;
	// puts("Exited Successfully");
}