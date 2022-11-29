#pragma once

#include <shader_compiler/defines.h>
#include <shader_compiler/utilities/binary_writer.h>
#include <bufferlib/buffer.h>

// marks in the binary_writer_t
enum
{
	MARK_ID_DESCRIPTOR_COUNT_0,
	MARK_ID_DESCRIPTOR_COUNT_1,
	MARK_ID_DESCRIPTOR_COUNT_2,
	MARK_ID_DESCRIPTOR_COUNT_3,
	MARK_ID_DESCRIPTOR_COUNT_4,
	MARK_ID_DESCRIPTOR_COUNT_5,
	MARK_ID_DESCRIPTOR_COUNT_6,
	MARK_ID_DESCRIPTOR_COUNT_7,
	MARK_ID_DESCRIPTOR_COUNT_8,
	MARK_ID_DESCRIPTOR_COUNT_9,
	MARK_ID_DESCRIPTOR_COUNT_10,
	MARK_ID_DESCRIPTOR_COUNT_MAX,

	MARK_ID_DESCRIPTOR_OFFSET_0,
	MARK_ID_DESCRIPTOR_OFFSET_1,
	MARK_ID_DESCRIPTOR_OFFSET_2,
	MARK_ID_DESCRIPTOR_OFFSET_3,
	MARK_ID_DESCRIPTOR_OFFSET_4,
	MARK_ID_DESCRIPTOR_OFFSET_5,
	MARK_ID_DESCRIPTOR_OFFSET_6,
	MARK_ID_DESCRIPTOR_OFFSET_7,
	MARK_ID_DESCRIPTOR_OFFSET_8,
	MARK_ID_DESCRIPTOR_OFFSET_9,
	MARK_ID_DESCRIPTOR_OFFSET_10,
	MARK_ID_DESCRIPTOR_OFFSET_11,
	MARK_ID_DESCRIPTOR_OFFSET_12,
	MARK_ID_DESCRIPTOR_OFFSET_13,
	MARK_ID_DESCRIPTOR_OFFSET_14,
	MARK_ID_DESCRIPTOR_OFFSET_15,
	MARK_ID_DESCRIPTOR_OFFSET_16,
	MARK_ID_DESCRIPTOR_OFFSET_17,
	MARK_ID_DESCRIPTOR_OFFSET_18,
	MARK_ID_DESCRIPTOR_OFFSET_19,
	MARK_ID_DESCRIPTOR_OFFSET_20,
	MARK_ID_DESCRIPTOR_OFFSET_21,
	MARK_ID_DESCRIPTOR_OFFSET_22,
	MARK_ID_DESCRIPTOR_OFFSET_23,
	MARK_ID_DESCRIPTOR_OFFSET_24,
	MARK_ID_DESCRIPTOR_OFFSET_25,
	MARK_ID_DESCRIPTOR_OFFSET_26,
	MARK_ID_DESCRIPTOR_OFFSET_27,
	MARK_ID_DESCRIPTOR_OFFSET_28,
	MARK_ID_DESCRIPTOR_OFFSET_29,
	MARK_ID_DESCRIPTOR_OFFSET_30,
	MARK_ID_DESCRIPTOR_OFFSET_31,
	MARK_ID_DESCRIPTOR_OFFSET_32,	
	MARK_ID_DESCRIPTOR_OFFSET_33,	
	MARK_ID_DESCRIPTOR_OFFSET_34,	
	MARK_ID_DESCRIPTOR_OFFSET_35,	
	MARK_ID_DESCRIPTOR_OFFSET_36,	
	MARK_ID_DESCRIPTOR_OFFSET_37,	
	MARK_ID_DESCRIPTOR_OFFSET_38,	
	MARK_ID_DESCRIPTOR_OFFSET_39,	
	MARK_ID_DESCRIPTOR_OFFSET_40,	
	MARK_ID_DESCRIPTOR_OFFSET_41,	
	MARK_ID_DESCRIPTOR_OFFSET_42,	
	MARK_ID_DESCRIPTOR_OFFSET_43,	
	MARK_ID_DESCRIPTOR_OFFSET_44,	
	MARK_ID_DESCRIPTOR_OFFSET_45,	
	MARK_ID_DESCRIPTOR_OFFSET_46,	
	MARK_ID_DESCRIPTOR_OFFSET_47,	
	MARK_ID_DESCRIPTOR_OFFSET_48,	
	MARK_ID_DESCRIPTOR_OFFSET_49,	
	MARK_ID_DESCRIPTOR_OFFSET_50,	
	MARK_ID_DESCRIPTOR_OFFSET_MAX,

	MARK_ID_SPIRV_OFFSET_0,
	MARK_ID_SPIRV_OFFSET_1,
	MARK_ID_SPIRV_OFFSET_2,
	MARK_ID_SPIRV_OFFSET_3,
	MARK_ID_SPIRV_OFFSET_4,
	MARK_ID_SPIRV_OFFSET_5,
	MARK_ID_SPIRV_OFFSET_6,
	MARK_ID_SPIRV_OFFSET_7,
	MARK_ID_SPIRV_OFFSET_8,
	MARK_ID_SPIRV_OFFSET_9,
	MARK_ID_SPIRV_OFFSET_10,
	MARK_ID_SPIRV_OFFSET_11,
	MARK_ID_SPIRV_OFFSET_12,
	MARK_ID_SPIRV_OFFSET_MAX,
	
	MARK_ID_IDENTIFIER_NAME_0,
	MARK_ID_IDENTIFIER_NAME_1,
	MARK_ID_IDENTIFIER_NAME_2,
	MARK_ID_IDENTIFIER_NAME_3,
	MARK_ID_IDENTIFIER_NAME_4,
	MARK_ID_IDENTIFIER_NAME_5,
	MARK_ID_IDENTIFIER_NAME_6,
	MARK_ID_IDENTIFIER_NAME_7,
	MARK_ID_IDENTIFIER_NAME_8,
	MARK_ID_IDENTIFIER_NAME_9,
	MARK_ID_IDENTIFIER_NAME_10,
	MARK_ID_IDENTIFIER_NAME_11,
	MARK_ID_IDENTIFIER_NAME_12,
	MARK_ID_IDENTIFIER_NAME_13,
	MARK_ID_IDENTIFIER_NAME_14,
	MARK_ID_IDENTIFIER_NAME_15,
	MARK_ID_IDENTIFIER_NAME_16,
	MARK_ID_IDENTIFIER_NAME_17,
	MARK_ID_IDENTIFIER_NAME_18,
	MARK_ID_IDENTIFIER_NAME_19,
	MARK_ID_IDENTIFIER_NAME_20,
	MARK_ID_IDENTIFIER_NAME_21,
	MARK_ID_IDENTIFIER_NAME_22,
	MARK_ID_IDENTIFIER_NAME_23,
	MARK_ID_IDENTIFIER_NAME_24,
	MARK_ID_IDENTIFIER_NAME_25,
	MARK_ID_IDENTIFIER_NAME_26,
	MARK_ID_IDENTIFIER_NAME_27,
	MARK_ID_IDENTIFIER_NAME_28,
	MARK_ID_IDENTIFIER_NAME_29,
	MARK_ID_IDENTIFIER_NAME_30,
	MARK_ID_IDENTIFIER_NAME_31,
	MARK_ID_IDENTIFIER_NAME_32,
	MARK_ID_IDENTIFIER_NAME_33,
	MARK_ID_IDENTIFIER_NAME_34,
	MARK_ID_IDENTIFIER_NAME_35,
	MARK_ID_IDENTIFIER_NAME_36,
	MARK_ID_IDENTIFIER_NAME_37,
	MARK_ID_IDENTIFIER_NAME_38,
	MARK_ID_IDENTIFIER_NAME_39,
	MARK_ID_IDENTIFIER_NAME_40,
	MARK_ID_IDENTIFIER_NAME_41,
	MARK_ID_IDENTIFIER_NAME_42,
	MARK_ID_IDENTIFIER_NAME_43,
	MARK_ID_IDENTIFIER_NAME_44,
	MARK_ID_IDENTIFIER_NAME_45,
	MARK_ID_IDENTIFIER_NAME_46,
	MARK_ID_IDENTIFIER_NAME_47,
	MARK_ID_IDENTIFIER_NAME_48,
	MARK_ID_IDENTIFIER_NAME_49,
	MARK_ID_IDENTIFIER_NAME_50,
	MARK_ID_IDENTIFIER_NAME_51,
	MARK_ID_IDENTIFIER_NAME_52,
	MARK_ID_IDENTIFIER_NAME_53,
	MARK_ID_IDENTIFIER_NAME_54,
	MARK_ID_IDENTIFIER_NAME_55,
	MARK_ID_IDENTIFIER_NAME_56,
	MARK_ID_IDENTIFIER_NAME_57,
	MARK_ID_IDENTIFIER_NAME_58,
	MARK_ID_IDENTIFIER_NAME_59,
	MARK_ID_IDENTIFIER_NAME_60,
	MARK_ID_IDENTIFIER_NAME_61,
	MARK_ID_IDENTIFIER_NAME_62,
	MARK_ID_IDENTIFIER_NAME_63,
	MARK_ID_IDENTIFIER_NAME_64,
	MARK_ID_IDENTIFIER_NAME_65,
	MARK_ID_IDENTIFIER_NAME_66,
	MARK_ID_IDENTIFIER_NAME_67,
	MARK_ID_IDENTIFIER_NAME_68,
	MARK_ID_IDENTIFIER_NAME_69,
	MARK_ID_IDENTIFIER_NAME_70,
	MARK_ID_IDENTIFIER_NAME_MAX,

	MARK_ID_FIELD_COUNT_0,
	MARK_ID_FIELD_COUNT_1,
	MARK_ID_FIELD_COUNT_2,
	MARK_ID_FIELD_COUNT_3,
	MARK_ID_FIELD_COUNT_4,
	MARK_ID_FIELD_COUNT_5,
	MARK_ID_FIELD_COUNT_6,
	MARK_ID_FIELD_COUNT_7,
	MARK_ID_FIELD_COUNT_8,
	MARK_ID_FIELD_COUNT_MAX,

	MARK_ID_MTL_RSC_DSCPT,
	MARK_ID_VTX_BFR_LYT_DSCPT,
	MARK_ID_GFX_PPLN_DSCPT,
	MARK_ID_RDPS_DSCPT,

	MARK_ID_MAX,

	MARK_ID_DESCRIPTOR_COUNT = MARK_ID_DESCRIPTOR_COUNT_0,
	MARK_ID_DESCRIPTOR_OFFSET = MARK_ID_DESCRIPTOR_OFFSET_0,
	MARK_ID_SPIRV_OFFSET = MARK_ID_SPIRV_OFFSET_0,
	MARK_ID_OFFSET = MARK_ID_DESCRIPTOR_OFFSET,
	MARK_ID_OFFSET_MAX = MARK_ID_SPIRV_OFFSET_MAX,
	MARK_ID_IDENTIFIER_NAME = MARK_ID_IDENTIFIER_NAME_0,
	MARK_ID_FIELD_COUNT = MARK_ID_FIELD_COUNT_0
};

typedef struct codegen_buffer_t
{
	binary_writer_t* main;
	binary_writer_t* data;
	BUFFER* flat;
} codegen_buffer_t;

/* constructors and destructors */
SC_API codegen_buffer_t* codegen_buffer_new();
SC_API codegen_buffer_t* codegen_buffer_create();
SC_API void codegen_destroy(codegen_buffer_t* buffer);
SC_API void codegen_release_resources(codegen_buffer_t* buffer);

/* flattens the codegen buffer and returns pointer to the BUFFER instance */
SC_API BUFFER* codegen_buffer_flatten(codegen_buffer_t* buffer);
