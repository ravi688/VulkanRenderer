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

SC_API void write_layout(const char* start, const char* end, codegen_buffer_t* writer);
