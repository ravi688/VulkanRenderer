#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/utilities/binary_writer.h>

#define SB_HDR_STR "V3D Shader Binary"


/* compiler command category
   i.e. #sb version 2022
*/
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

// shader binary header
typedef struct sb_header_t
{
	// file identification header for shader binary
	const char* header_str;

	// shader language version
	sl_version_t sl_version;

	// shader binary version
	sb_version_t sb_version;
} sb_header_t;

SC_API const char* write_header(const char* start, const char* const end, binary_writer_t* writer);
