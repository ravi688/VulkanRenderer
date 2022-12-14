#include <shader_compiler/compiler/codegen/header.h>
#include <shader_compiler/utilities/string.h>
#include <shader_compiler/debug.h>
#include <phymac_parser/string.h>
#include <stdlib.h>

/* adding due to ucrt64 compilation issues */
#include <stdio.h>

SC_API const char* write_header(const char* start, const char* const end, codegen_buffer_t* writer)
{
	binary_writer_string(writer->main, SB_HDR_STR);

	binary_writer_u32_mark(writer->main, MARK_ID_COMPILER_CMD_COUNT);

	// skip the whitespaces
	start = skip_ws(start, end);
	u32 count = 0;
	while(*start == '#')
	{
		start++;
		while(*start != '\n')
		{
			// skip the spaces and tabs but not the newline as it is a delimiter for a preprocessor line
			start = skip(start, " \t", end);

			// command is for shader binary generation
			if(safe_strncmp(start, "sb", 2) == 0)
			{
				binary_writer_u32(writer->main, (u32)CMPLR_CMD_CAT_SB);
				start += 2;
				count++;
			}
			// command is for shader language compilation
			else if(safe_strncmp(start, "sl", 2) == 0)
			{
				binary_writer_u32(writer->main, (u32)CMPLR_CMD_CAT_SL);
				start += 2;
				count++;
			}
			// version { shader binary generation, shader language generation }
			else if(safe_strncmp(start, "version", 7) == 0)
			{
				start += 7;
				u32 ver = _strtoui64(start, (char**)&start, 10);
				if(ver == 0)
					DEBUG_LOG_FETAL_ERROR("Invalid shader binary or language version");
				binary_writer_u32(writer->main, ver);
			}
		}
		// skip the new line character
		start++;
	}

	binary_writer_u32_set(writer->main, MARK_ID_COMPILER_CMD_COUNT, count);

	return start;
}
