#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>

#define SB_HDR_STR "V3D Shader Binary"


/* compiler command category */
typedef enum cmplr_cmd_cat_t
{
	/* command is related to shader binary generation */
	CMPLR_CMD_CAT_SB = 1UL,
	/* command is related to shader language compilation */
	CMPLR_CMD_CAT_SL
} cmplr_cmd_cat_t;

/* shader binary version
   i.e #sb version 2022
 */
typedef enum sb_version_t
{
	SB_VERSION_2021 = 2021UL,
	SB_VERSION_2022
} sb_version_t;

/* shader language version
   i.e #sl version 2022
*/
typedef enum sl_version_t
{
	SL_VERSION_2021 = 2021UL,
	SL_VERSION_2022
} sl_version_t;

/*	parses the string pointed by 'start', ends at 'end', and writes the file header to the codegen buffer 'writer'
	start: pointer to the stsart of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the file header to.

	NOTE: the expected string should be as follows:
	-------------------------
	#sl version 2022
	#sl version 2022
	-------------------------
	the above string must appear before the "Shader" block
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API const char* write_header(const char* start, const char* const end, codegen_buffer_t* writer);
