#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/compiler/codegen/codegen_buffer.h>

/* shader stages */
#define VERTEX_BIT 										BIT32(8 + 0)
#define FRAGMENT_BIT 									BIT32(8 + 1)
#define GEOMETRY_BIT 									BIT32(8 + 2)
#define TESSELLATION_BIT 								BIT32(8 + 3)

/* shader resource description types */
#define PUSH_CONSTANT_BIT 								BIT32(16 + 0)
#define OPAQUE_BIT 										BIT32(16 + 1)
#define UNIFORM_BUFFER_BIT 								BIT32(16 + 2)
#define STORAGE_BUFFER_BIT 								BIT32(16 + 3)
#define PER_VERTEX_ATTRIB_BIT 							BIT32(16 + 4)
#define PER_INSTANCE_ATTRIB_BIT 						BIT32(16 + 5)

BEGIN_CPP_COMPATIBLE

/*	parses the string pointed by 'start', ends at 'end', and writes "Properties" or "Layout" block to the codegen buffer 'writer'.
	start: pointer to the start of the string
	end: pointer to the end of the string (must point to null character)
	writer: codgen buffer to which write the layout data to.
	is_write_count: if true, then this function will write the number of layout 'descriptions' found inside the block for which it is called.

	NOTE: the expected string should be as follows:
	-------------------------
	fragment vertex [MATERIAL_SET, MATERIAL_PROPERTIES_BINDING] uniform Parameters 
	{
		//...
		float reflectance;
		float roughness;
		//...
	} parameters;

	per_vertex [POSITION_LOCATION, POSITION_BINDING] vec3 position;
	-------------------------
	see: //Documents/V3D/V3DShaderLangSpec.docx
 */
SC_API void write_layout(const char* start, const char* end, codegen_buffer_t* writer, bool is_write_count);

END_CPP_COMPATIBLE
