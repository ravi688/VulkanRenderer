#pragma once

#include <shader_compiler/defines.h>
#include <common/binary_writer.h>
#include <bufferlib/buffer.h>

// marks in the binary_writer_t
enum
{
	MARK_ID_COMPILER_CMD_COUNT,
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
	MARK_ID_DESCRIPTOR_COUNT_MAX,		// 12

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
	MARK_ID_DESCRIPTOR_OFFSET_51,	
	MARK_ID_DESCRIPTOR_OFFSET_52,	
	MARK_ID_DESCRIPTOR_OFFSET_53,	
	MARK_ID_DESCRIPTOR_OFFSET_54,	
	MARK_ID_DESCRIPTOR_OFFSET_55,	
	MARK_ID_DESCRIPTOR_OFFSET_56,	
	MARK_ID_DESCRIPTOR_OFFSET_57,	
	MARK_ID_DESCRIPTOR_OFFSET_58,	
	MARK_ID_DESCRIPTOR_OFFSET_59,	
	MARK_ID_DESCRIPTOR_OFFSET_60,	
	MARK_ID_DESCRIPTOR_OFFSET_61,	
	MARK_ID_DESCRIPTOR_OFFSET_62,	
	MARK_ID_DESCRIPTOR_OFFSET_63,	
	MARK_ID_DESCRIPTOR_OFFSET_64,	
	MARK_ID_DESCRIPTOR_OFFSET_65,	
	MARK_ID_DESCRIPTOR_OFFSET_66,	
	MARK_ID_DESCRIPTOR_OFFSET_67,	
	MARK_ID_DESCRIPTOR_OFFSET_68,	
	MARK_ID_DESCRIPTOR_OFFSET_69,	
	MARK_ID_DESCRIPTOR_OFFSET_70,	
	MARK_ID_DESCRIPTOR_OFFSET_71,	
	MARK_ID_DESCRIPTOR_OFFSET_72,	
	MARK_ID_DESCRIPTOR_OFFSET_73,	
	MARK_ID_DESCRIPTOR_OFFSET_74,	
	MARK_ID_DESCRIPTOR_OFFSET_75,	
	MARK_ID_DESCRIPTOR_OFFSET_76,	
	MARK_ID_DESCRIPTOR_OFFSET_77,	
	MARK_ID_DESCRIPTOR_OFFSET_78,	
	MARK_ID_DESCRIPTOR_OFFSET_79,	
	MARK_ID_DESCRIPTOR_OFFSET_80,	
	MARK_ID_DESCRIPTOR_OFFSET_81,	
	MARK_ID_DESCRIPTOR_OFFSET_82,	
	MARK_ID_DESCRIPTOR_OFFSET_83,	
	MARK_ID_DESCRIPTOR_OFFSET_84,	
	MARK_ID_DESCRIPTOR_OFFSET_85,	
	MARK_ID_DESCRIPTOR_OFFSET_86,	
	MARK_ID_DESCRIPTOR_OFFSET_87,	
	MARK_ID_DESCRIPTOR_OFFSET_88,	
	MARK_ID_DESCRIPTOR_OFFSET_89,	
	MARK_ID_DESCRIPTOR_OFFSET_90,	
	MARK_ID_DESCRIPTOR_OFFSET_91,	
	MARK_ID_DESCRIPTOR_OFFSET_92,	
	MARK_ID_DESCRIPTOR_OFFSET_93,	
	MARK_ID_DESCRIPTOR_OFFSET_94,	
	MARK_ID_DESCRIPTOR_OFFSET_95,	
	MARK_ID_DESCRIPTOR_OFFSET_96,	
	MARK_ID_DESCRIPTOR_OFFSET_97,	
	MARK_ID_DESCRIPTOR_OFFSET_98,	
	MARK_ID_DESCRIPTOR_OFFSET_99,	
	MARK_ID_DESCRIPTOR_OFFSET_100,
	MARK_ID_DESCRIPTOR_OFFSET_101,
	MARK_ID_DESCRIPTOR_OFFSET_102,
	MARK_ID_DESCRIPTOR_OFFSET_103,
	MARK_ID_DESCRIPTOR_OFFSET_104,
	MARK_ID_DESCRIPTOR_OFFSET_105,
	MARK_ID_DESCRIPTOR_OFFSET_106,
	MARK_ID_DESCRIPTOR_OFFSET_107,
	MARK_ID_DESCRIPTOR_OFFSET_108,
	MARK_ID_DESCRIPTOR_OFFSET_109,
	MARK_ID_DESCRIPTOR_OFFSET_110,
	MARK_ID_DESCRIPTOR_OFFSET_111,
	MARK_ID_DESCRIPTOR_OFFSET_112,
	MARK_ID_DESCRIPTOR_OFFSET_113,
	MARK_ID_DESCRIPTOR_OFFSET_114,
	MARK_ID_DESCRIPTOR_OFFSET_115,
	MARK_ID_DESCRIPTOR_OFFSET_116,
	MARK_ID_DESCRIPTOR_OFFSET_117,
	MARK_ID_DESCRIPTOR_OFFSET_118,
	MARK_ID_DESCRIPTOR_OFFSET_119,
	MARK_ID_DESCRIPTOR_OFFSET_120,
	MARK_ID_DESCRIPTOR_OFFSET_121,
	MARK_ID_DESCRIPTOR_OFFSET_122,
	MARK_ID_DESCRIPTOR_OFFSET_123,
	MARK_ID_DESCRIPTOR_OFFSET_124,
	MARK_ID_DESCRIPTOR_OFFSET_125,
	MARK_ID_DESCRIPTOR_OFFSET_126,
	MARK_ID_DESCRIPTOR_OFFSET_127,
	MARK_ID_DESCRIPTOR_OFFSET_128,
	MARK_ID_DESCRIPTOR_OFFSET_129,
	MARK_ID_DESCRIPTOR_OFFSET_130,
	MARK_ID_DESCRIPTOR_OFFSET_131,
	MARK_ID_DESCRIPTOR_OFFSET_132,
	MARK_ID_DESCRIPTOR_OFFSET_133,
	MARK_ID_DESCRIPTOR_OFFSET_134,
	MARK_ID_DESCRIPTOR_OFFSET_135,
	MARK_ID_DESCRIPTOR_OFFSET_136,
	MARK_ID_DESCRIPTOR_OFFSET_137,
	MARK_ID_DESCRIPTOR_OFFSET_138,
	MARK_ID_DESCRIPTOR_OFFSET_139,
	MARK_ID_DESCRIPTOR_OFFSET_140,
	MARK_ID_DESCRIPTOR_OFFSET_141,
	MARK_ID_DESCRIPTOR_OFFSET_142,
	MARK_ID_DESCRIPTOR_OFFSET_143,
	MARK_ID_DESCRIPTOR_OFFSET_144,
	MARK_ID_DESCRIPTOR_OFFSET_145,
	MARK_ID_DESCRIPTOR_OFFSET_146,
	MARK_ID_DESCRIPTOR_OFFSET_147,
	MARK_ID_DESCRIPTOR_OFFSET_148,
	MARK_ID_DESCRIPTOR_OFFSET_149,
	MARK_ID_DESCRIPTOR_OFFSET_150,
	MARK_ID_DESCRIPTOR_OFFSET_151,
	MARK_ID_DESCRIPTOR_OFFSET_152,
	MARK_ID_DESCRIPTOR_OFFSET_153,
	MARK_ID_DESCRIPTOR_OFFSET_154,
	MARK_ID_DESCRIPTOR_OFFSET_155,
	MARK_ID_DESCRIPTOR_OFFSET_156,
	MARK_ID_DESCRIPTOR_OFFSET_157,
	MARK_ID_DESCRIPTOR_OFFSET_158,
	MARK_ID_DESCRIPTOR_OFFSET_159,
	MARK_ID_DESCRIPTOR_OFFSET_160,
	MARK_ID_DESCRIPTOR_OFFSET_161,
	MARK_ID_DESCRIPTOR_OFFSET_162,
	MARK_ID_DESCRIPTOR_OFFSET_163,
	MARK_ID_DESCRIPTOR_OFFSET_164,
	MARK_ID_DESCRIPTOR_OFFSET_165,
	MARK_ID_DESCRIPTOR_OFFSET_166,
	MARK_ID_DESCRIPTOR_OFFSET_167,
	MARK_ID_DESCRIPTOR_OFFSET_168,
	MARK_ID_DESCRIPTOR_OFFSET_169,
	MARK_ID_DESCRIPTOR_OFFSET_170,
	MARK_ID_DESCRIPTOR_OFFSET_171,
	MARK_ID_DESCRIPTOR_OFFSET_172,
	MARK_ID_DESCRIPTOR_OFFSET_173,
	MARK_ID_DESCRIPTOR_OFFSET_174,
	MARK_ID_DESCRIPTOR_OFFSET_175,
	MARK_ID_DESCRIPTOR_OFFSET_176,
	MARK_ID_DESCRIPTOR_OFFSET_177,
	MARK_ID_DESCRIPTOR_OFFSET_178,
	MARK_ID_DESCRIPTOR_OFFSET_179,
	MARK_ID_DESCRIPTOR_OFFSET_180,
	MARK_ID_DESCRIPTOR_OFFSET_181,
	MARK_ID_DESCRIPTOR_OFFSET_182,
	MARK_ID_DESCRIPTOR_OFFSET_183,
	MARK_ID_DESCRIPTOR_OFFSET_184,
	MARK_ID_DESCRIPTOR_OFFSET_185,
	MARK_ID_DESCRIPTOR_OFFSET_186,
	MARK_ID_DESCRIPTOR_OFFSET_187,
	MARK_ID_DESCRIPTOR_OFFSET_188,
	MARK_ID_DESCRIPTOR_OFFSET_189,
	MARK_ID_DESCRIPTOR_OFFSET_190,
	MARK_ID_DESCRIPTOR_OFFSET_191,
	MARK_ID_DESCRIPTOR_OFFSET_192,
	MARK_ID_DESCRIPTOR_OFFSET_193,
	MARK_ID_DESCRIPTOR_OFFSET_194,
	MARK_ID_DESCRIPTOR_OFFSET_195,
	MARK_ID_DESCRIPTOR_OFFSET_196,
	MARK_ID_DESCRIPTOR_OFFSET_197,
	MARK_ID_DESCRIPTOR_OFFSET_198,
	MARK_ID_DESCRIPTOR_OFFSET_199,
	MARK_ID_DESCRIPTOR_OFFSET_200,
	MARK_ID_DESCRIPTOR_OFFSET_201,
	MARK_ID_DESCRIPTOR_OFFSET_202,
	MARK_ID_DESCRIPTOR_OFFSET_203,
	MARK_ID_DESCRIPTOR_OFFSET_MAX, 	// 205

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
	MARK_ID_SPIRV_OFFSET_13,
	MARK_ID_SPIRV_OFFSET_14,
	MARK_ID_SPIRV_OFFSET_15,
	MARK_ID_SPIRV_OFFSET_16,
	MARK_ID_SPIRV_OFFSET_17,
	MARK_ID_SPIRV_OFFSET_18,
	MARK_ID_SPIRV_OFFSET_19,
	MARK_ID_SPIRV_OFFSET_20,
	MARK_ID_SPIRV_OFFSET_21,
	MARK_ID_SPIRV_OFFSET_23,
	MARK_ID_SPIRV_OFFSET_24,
	MARK_ID_SPIRV_OFFSET_25,
	MARK_ID_SPIRV_OFFSET_26,
	MARK_ID_SPIRV_OFFSET_27,
	MARK_ID_SPIRV_OFFSET_28,
	MARK_ID_SPIRV_OFFSET_29,
	MARK_ID_SPIRV_OFFSET_30,
	MARK_ID_SPIRV_OFFSET_31,
	MARK_ID_SPIRV_OFFSET_32,
	MARK_ID_SPIRV_OFFSET_MAX,	// 34

	MARK_ID_PIPELINE_OFFSET_0,
	MARK_ID_PIPELINE_OFFSET_1,
	MARK_ID_PIPELINE_OFFSET_2,
	MARK_ID_PIPELINE_OFFSET_3,
	MARK_ID_PIPELINE_OFFSET_4,
	MARK_ID_PIPELINE_OFFSET_5,
	MARK_ID_PIPELINE_OFFSET_6,
	MARK_ID_PIPELINE_OFFSET_7,
	MARK_ID_PIPELINE_OFFSET_8,
	MARK_ID_PIPELINE_OFFSET_9,
	MARK_ID_PIPELINE_OFFSET_10,
	MARK_ID_PIPELINE_OFFSET_11,
	MARK_ID_PIPELINE_OFFSET_12,
	MARK_ID_PIPELINE_OFFSET_MAX, // 14, 265

	MARK_ID_SPIRV_LENGTH_0,
	MARK_ID_SPIRV_LENGTH_1,
	MARK_ID_SPIRV_LENGTH_2,
	MARK_ID_SPIRV_LENGTH_3,
	MARK_ID_SPIRV_LENGTH_4,
	MARK_ID_SPIRV_LENGTH_5,
	MARK_ID_SPIRV_LENGTH_6,
	MARK_ID_SPIRV_LENGTH_7,
	MARK_ID_SPIRV_LENGTH_8,
	MARK_ID_SPIRV_LENGTH_9,
	MARK_ID_SPIRV_LENGTH_10,
	MARK_ID_SPIRV_LENGTH_11,
	MARK_ID_SPIRV_LENGTH_12,
	MARK_ID_SPIRV_LENGTH_13,
	MARK_ID_SPIRV_LENGTH_14,
	MARK_ID_SPIRV_LENGTH_15,
	MARK_ID_SPIRV_LENGTH_16,
	MARK_ID_SPIRV_LENGTH_17,
	MARK_ID_SPIRV_LENGTH_18,
	MARK_ID_SPIRV_LENGTH_19,
	MARK_ID_SPIRV_LENGTH_20,
	MARK_ID_SPIRV_LENGTH_21,
	MARK_ID_SPIRV_LENGTH_23,
	MARK_ID_SPIRV_LENGTH_24,
	MARK_ID_SPIRV_LENGTH_25,
	MARK_ID_SPIRV_LENGTH_26,
	MARK_ID_SPIRV_LENGTH_27,
	MARK_ID_SPIRV_LENGTH_28,
	MARK_ID_SPIRV_LENGTH_29,
	MARK_ID_SPIRV_LENGTH_30,
	MARK_ID_SPIRV_LENGTH_31,
	MARK_ID_SPIRV_LENGTH_32,
	MARK_ID_SPIRV_LENGTH_MAX, // 34, 299

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
	MARK_ID_IDENTIFIER_NAME_71,	
	MARK_ID_IDENTIFIER_NAME_72,	
	MARK_ID_IDENTIFIER_NAME_73,	
	MARK_ID_IDENTIFIER_NAME_74,	
	MARK_ID_IDENTIFIER_NAME_75,	
	MARK_ID_IDENTIFIER_NAME_76,	
	MARK_ID_IDENTIFIER_NAME_77,	
	MARK_ID_IDENTIFIER_NAME_78,	
	MARK_ID_IDENTIFIER_NAME_79,	
	MARK_ID_IDENTIFIER_NAME_80,	
	MARK_ID_IDENTIFIER_NAME_81,	
	MARK_ID_IDENTIFIER_NAME_82,	
	MARK_ID_IDENTIFIER_NAME_83,	
	MARK_ID_IDENTIFIER_NAME_84,	
	MARK_ID_IDENTIFIER_NAME_85,	
	MARK_ID_IDENTIFIER_NAME_86,	
	MARK_ID_IDENTIFIER_NAME_87,	
	MARK_ID_IDENTIFIER_NAME_88,	
	MARK_ID_IDENTIFIER_NAME_89,	
	MARK_ID_IDENTIFIER_NAME_90,	
	MARK_ID_IDENTIFIER_NAME_91,	
	MARK_ID_IDENTIFIER_NAME_92,	
	MARK_ID_IDENTIFIER_NAME_93,	
	MARK_ID_IDENTIFIER_NAME_94,	
	MARK_ID_IDENTIFIER_NAME_95,	
	MARK_ID_IDENTIFIER_NAME_96,	
	MARK_ID_IDENTIFIER_NAME_97,	
	MARK_ID_IDENTIFIER_NAME_98,	
	MARK_ID_IDENTIFIER_NAME_99,	
	MARK_ID_IDENTIFIER_NAME_100,
	MARK_ID_IDENTIFIER_NAME_101,
	MARK_ID_IDENTIFIER_NAME_102,
	MARK_ID_IDENTIFIER_NAME_103,
	MARK_ID_IDENTIFIER_NAME_104,
	MARK_ID_IDENTIFIER_NAME_105,
	MARK_ID_IDENTIFIER_NAME_106,
	MARK_ID_IDENTIFIER_NAME_107,
	MARK_ID_IDENTIFIER_NAME_108,
	MARK_ID_IDENTIFIER_NAME_109,
	MARK_ID_IDENTIFIER_NAME_110,
	MARK_ID_IDENTIFIER_NAME_111,
	MARK_ID_IDENTIFIER_NAME_112,
	MARK_ID_IDENTIFIER_NAME_113,
	MARK_ID_IDENTIFIER_NAME_114,
	MARK_ID_IDENTIFIER_NAME_115,
	MARK_ID_IDENTIFIER_NAME_116,
	MARK_ID_IDENTIFIER_NAME_117,
	MARK_ID_IDENTIFIER_NAME_118,
	MARK_ID_IDENTIFIER_NAME_119,
	MARK_ID_IDENTIFIER_NAME_120,
	MARK_ID_IDENTIFIER_NAME_121,
	MARK_ID_IDENTIFIER_NAME_122,
	MARK_ID_IDENTIFIER_NAME_123,
	MARK_ID_IDENTIFIER_NAME_124,
	MARK_ID_IDENTIFIER_NAME_125,
	MARK_ID_IDENTIFIER_NAME_126,
	MARK_ID_IDENTIFIER_NAME_127,
	MARK_ID_IDENTIFIER_NAME_128,
	MARK_ID_IDENTIFIER_NAME_129,
	MARK_ID_IDENTIFIER_NAME_130,
	MARK_ID_IDENTIFIER_NAME_131,
	MARK_ID_IDENTIFIER_NAME_132,
	MARK_ID_IDENTIFIER_NAME_133,
	MARK_ID_IDENTIFIER_NAME_134,
	MARK_ID_IDENTIFIER_NAME_135,
	MARK_ID_IDENTIFIER_NAME_136,
	MARK_ID_IDENTIFIER_NAME_137,
	MARK_ID_IDENTIFIER_NAME_138,
	MARK_ID_IDENTIFIER_NAME_139,
	MARK_ID_IDENTIFIER_NAME_140,
	MARK_ID_IDENTIFIER_NAME_141,
	MARK_ID_IDENTIFIER_NAME_142,
	MARK_ID_IDENTIFIER_NAME_143,
	MARK_ID_IDENTIFIER_NAME_144,
	MARK_ID_IDENTIFIER_NAME_145,
	MARK_ID_IDENTIFIER_NAME_146,
	MARK_ID_IDENTIFIER_NAME_147,
	MARK_ID_IDENTIFIER_NAME_148,
	MARK_ID_IDENTIFIER_NAME_149,
	MARK_ID_IDENTIFIER_NAME_150,
	MARK_ID_IDENTIFIER_NAME_151,
	MARK_ID_IDENTIFIER_NAME_152,
	MARK_ID_IDENTIFIER_NAME_153,
	MARK_ID_IDENTIFIER_NAME_154,
	MARK_ID_IDENTIFIER_NAME_155,
	MARK_ID_IDENTIFIER_NAME_156,
	MARK_ID_IDENTIFIER_NAME_157,
	MARK_ID_IDENTIFIER_NAME_158,
	MARK_ID_IDENTIFIER_NAME_159,
	MARK_ID_IDENTIFIER_NAME_160,
	MARK_ID_IDENTIFIER_NAME_161,
	MARK_ID_IDENTIFIER_NAME_162,
	MARK_ID_IDENTIFIER_NAME_163,
	MARK_ID_IDENTIFIER_NAME_164,
	MARK_ID_IDENTIFIER_NAME_165,
	MARK_ID_IDENTIFIER_NAME_166,
	MARK_ID_IDENTIFIER_NAME_167,
	MARK_ID_IDENTIFIER_NAME_168,
	MARK_ID_IDENTIFIER_NAME_169,
	MARK_ID_IDENTIFIER_NAME_170,
	MARK_ID_IDENTIFIER_NAME_171,
	MARK_ID_IDENTIFIER_NAME_172,
	MARK_ID_IDENTIFIER_NAME_173,
	MARK_ID_IDENTIFIER_NAME_174,
	MARK_ID_IDENTIFIER_NAME_175,
	MARK_ID_IDENTIFIER_NAME_176,
	MARK_ID_IDENTIFIER_NAME_177,
	MARK_ID_IDENTIFIER_NAME_178,
	MARK_ID_IDENTIFIER_NAME_179,
	MARK_ID_IDENTIFIER_NAME_180,
	MARK_ID_IDENTIFIER_NAME_181,
	MARK_ID_IDENTIFIER_NAME_182,
	MARK_ID_IDENTIFIER_NAME_183,
	MARK_ID_IDENTIFIER_NAME_184,
	MARK_ID_IDENTIFIER_NAME_185,
	MARK_ID_IDENTIFIER_NAME_186,
	MARK_ID_IDENTIFIER_NAME_187,
	MARK_ID_IDENTIFIER_NAME_188,
	MARK_ID_IDENTIFIER_NAME_189,
	MARK_ID_IDENTIFIER_NAME_190,
	MARK_ID_IDENTIFIER_NAME_191,
	MARK_ID_IDENTIFIER_NAME_192,
	MARK_ID_IDENTIFIER_NAME_193,
	MARK_ID_IDENTIFIER_NAME_194,
	MARK_ID_IDENTIFIER_NAME_195,
	MARK_ID_IDENTIFIER_NAME_196,
	MARK_ID_IDENTIFIER_NAME_197,
	MARK_ID_IDENTIFIER_NAME_198,
	MARK_ID_IDENTIFIER_NAME_199,
	MARK_ID_IDENTIFIER_NAME_200,
	MARK_ID_IDENTIFIER_NAME_201,
	MARK_ID_IDENTIFIER_NAME_202,
	MARK_ID_IDENTIFIER_NAME_203,
	MARK_ID_IDENTIFIER_NAME_204,
	MARK_ID_IDENTIFIER_NAME_205,
	MARK_ID_IDENTIFIER_NAME_206,
	MARK_ID_IDENTIFIER_NAME_207,
	MARK_ID_IDENTIFIER_NAME_208,
	MARK_ID_IDENTIFIER_NAME_209,
	MARK_ID_IDENTIFIER_NAME_210,
	MARK_ID_IDENTIFIER_NAME_211,
	MARK_ID_IDENTIFIER_NAME_212,
	MARK_ID_IDENTIFIER_NAME_213,
	MARK_ID_IDENTIFIER_NAME_214,
	MARK_ID_IDENTIFIER_NAME_215,
	MARK_ID_IDENTIFIER_NAME_216,
	MARK_ID_IDENTIFIER_NAME_217,
	MARK_ID_IDENTIFIER_NAME_218,
	MARK_ID_IDENTIFIER_NAME_219,
	MARK_ID_IDENTIFIER_NAME_220,
	MARK_ID_IDENTIFIER_NAME_221,
	MARK_ID_IDENTIFIER_NAME_222,
	MARK_ID_IDENTIFIER_NAME_223,
	MARK_ID_IDENTIFIER_NAME_224,
	MARK_ID_IDENTIFIER_NAME_225,
	MARK_ID_IDENTIFIER_NAME_226,
	MARK_ID_IDENTIFIER_NAME_227,
	MARK_ID_IDENTIFIER_NAME_228,
	MARK_ID_IDENTIFIER_NAME_229,
	MARK_ID_IDENTIFIER_NAME_230,
	MARK_ID_IDENTIFIER_NAME_231,
	MARK_ID_IDENTIFIER_NAME_232,
	MARK_ID_IDENTIFIER_NAME_233,
	MARK_ID_IDENTIFIER_NAME_234,
	MARK_ID_IDENTIFIER_NAME_235,
	MARK_ID_IDENTIFIER_NAME_236,
	MARK_ID_IDENTIFIER_NAME_237,
	MARK_ID_IDENTIFIER_NAME_238,
	MARK_ID_IDENTIFIER_NAME_239,
	MARK_ID_IDENTIFIER_NAME_240,
	MARK_ID_IDENTIFIER_NAME_241,
	MARK_ID_IDENTIFIER_NAME_242,
	MARK_ID_IDENTIFIER_NAME_243,
	MARK_ID_IDENTIFIER_NAME_244,
	MARK_ID_IDENTIFIER_NAME_245,
	MARK_ID_IDENTIFIER_NAME_246,
	MARK_ID_IDENTIFIER_NAME_247,
	MARK_ID_IDENTIFIER_NAME_248,
	MARK_ID_IDENTIFIER_NAME_249,
	MARK_ID_IDENTIFIER_NAME_250,
	MARK_ID_IDENTIFIER_NAME_251,
	MARK_ID_IDENTIFIER_NAME_252,
	MARK_ID_IDENTIFIER_NAME_253,
	MARK_ID_IDENTIFIER_NAME_254,
	MARK_ID_IDENTIFIER_NAME_255,
	MARK_ID_IDENTIFIER_NAME_256,
	MARK_ID_IDENTIFIER_NAME_257,
	MARK_ID_IDENTIFIER_NAME_258,
	MARK_ID_IDENTIFIER_NAME_259,
	MARK_ID_IDENTIFIER_NAME_260,
	MARK_ID_IDENTIFIER_NAME_MAX, // 205, 504

	MARK_ID_FIELD_COUNT_0,
	MARK_ID_FIELD_COUNT_1,
	MARK_ID_FIELD_COUNT_2,
	MARK_ID_FIELD_COUNT_3,
	MARK_ID_FIELD_COUNT_4,
	MARK_ID_FIELD_COUNT_5,
	MARK_ID_FIELD_COUNT_6,
	MARK_ID_FIELD_COUNT_7,
	MARK_ID_FIELD_COUNT_8,
	MARK_ID_FIELD_COUNT_9,
	MARK_ID_FIELD_COUNT_10,
	MARK_ID_FIELD_COUNT_11,
	MARK_ID_FIELD_COUNT_12,
	MARK_ID_FIELD_COUNT_13,
	MARK_ID_FIELD_COUNT_14,
	MARK_ID_FIELD_COUNT_15,
	MARK_ID_FIELD_COUNT_16,
	MARK_ID_FIELD_COUNT_17,
	MARK_ID_FIELD_COUNT_18,
	MARK_ID_FIELD_COUNT_19,
	MARK_ID_FIELD_COUNT_20,
	MARK_ID_FIELD_COUNT_21,
	MARK_ID_FIELD_COUNT_22,
	MARK_ID_FIELD_COUNT_23,
	MARK_ID_FIELD_COUNT_24,
	MARK_ID_FIELD_COUNT_25,
	MARK_ID_FIELD_COUNT_26,
	MARK_ID_FIELD_COUNT_27,
	MARK_ID_FIELD_COUNT_28,
	MARK_ID_FIELD_COUNT_29,
	MARK_ID_FIELD_COUNT_30,
	MARK_ID_FIELD_COUNT_31,
	MARK_ID_FIELD_COUNT_32,	
	MARK_ID_FIELD_COUNT_33,	
	MARK_ID_FIELD_COUNT_34,	
	MARK_ID_FIELD_COUNT_35,	
	MARK_ID_FIELD_COUNT_36,	
	MARK_ID_FIELD_COUNT_37,	
	MARK_ID_FIELD_COUNT_38,	
	MARK_ID_FIELD_COUNT_39,	
	MARK_ID_FIELD_COUNT_40,	
	MARK_ID_FIELD_COUNT_41,	
	MARK_ID_FIELD_COUNT_42,	
	MARK_ID_FIELD_COUNT_43,	
	MARK_ID_FIELD_COUNT_44,	
	MARK_ID_FIELD_COUNT_45,	
	MARK_ID_FIELD_COUNT_46,	
	MARK_ID_FIELD_COUNT_47,	
	MARK_ID_FIELD_COUNT_48,	
	MARK_ID_FIELD_COUNT_49,	
	MARK_ID_FIELD_COUNT_50,	
	MARK_ID_FIELD_COUNT_51,	
	MARK_ID_FIELD_COUNT_52,	
	MARK_ID_FIELD_COUNT_53,	
	MARK_ID_FIELD_COUNT_54,	
	MARK_ID_FIELD_COUNT_55,	
	MARK_ID_FIELD_COUNT_56,	
	MARK_ID_FIELD_COUNT_57,	
	MARK_ID_FIELD_COUNT_58,	
	MARK_ID_FIELD_COUNT_59,	
	MARK_ID_FIELD_COUNT_60,	
	MARK_ID_FIELD_COUNT_61,	
	MARK_ID_FIELD_COUNT_62,	
	MARK_ID_FIELD_COUNT_63,	
	MARK_ID_FIELD_COUNT_64,	
	MARK_ID_FIELD_COUNT_65,	
	MARK_ID_FIELD_COUNT_66,	
	MARK_ID_FIELD_COUNT_67,	
	MARK_ID_FIELD_COUNT_68,	
	MARK_ID_FIELD_COUNT_69,	
	MARK_ID_FIELD_COUNT_70,	
	MARK_ID_FIELD_COUNT_71,	
	MARK_ID_FIELD_COUNT_72,	
	MARK_ID_FIELD_COUNT_73,	
	MARK_ID_FIELD_COUNT_74,	
	MARK_ID_FIELD_COUNT_75,	
	MARK_ID_FIELD_COUNT_76,	
	MARK_ID_FIELD_COUNT_77,	
	MARK_ID_FIELD_COUNT_78,	
	MARK_ID_FIELD_COUNT_79,	
	MARK_ID_FIELD_COUNT_80,	
	MARK_ID_FIELD_COUNT_81,	
	MARK_ID_FIELD_COUNT_82,	
	MARK_ID_FIELD_COUNT_83,	
	MARK_ID_FIELD_COUNT_84,	
	MARK_ID_FIELD_COUNT_85,	
	MARK_ID_FIELD_COUNT_86,	
	MARK_ID_FIELD_COUNT_87,	
	MARK_ID_FIELD_COUNT_88,	
	MARK_ID_FIELD_COUNT_89,	
	MARK_ID_FIELD_COUNT_90,	
	MARK_ID_FIELD_COUNT_91,	
	MARK_ID_FIELD_COUNT_92,	
	MARK_ID_FIELD_COUNT_93,	
	MARK_ID_FIELD_COUNT_94,	
	MARK_ID_FIELD_COUNT_95,	
	MARK_ID_FIELD_COUNT_96,	
	MARK_ID_FIELD_COUNT_97,	
	MARK_ID_FIELD_COUNT_98,	
	MARK_ID_FIELD_COUNT_99,	
	MARK_ID_FIELD_COUNT_100,
	MARK_ID_FIELD_COUNT_101,
	MARK_ID_FIELD_COUNT_102,
	MARK_ID_FIELD_COUNT_103,
	MARK_ID_FIELD_COUNT_104,
	MARK_ID_FIELD_COUNT_105,
	MARK_ID_FIELD_COUNT_106,
	MARK_ID_FIELD_COUNT_107,
	MARK_ID_FIELD_COUNT_108,
	MARK_ID_FIELD_COUNT_109,
	MARK_ID_FIELD_COUNT_110,
	MARK_ID_FIELD_COUNT_111,
	MARK_ID_FIELD_COUNT_112,
	MARK_ID_FIELD_COUNT_113,
	MARK_ID_FIELD_COUNT_114,
	MARK_ID_FIELD_COUNT_115,
	MARK_ID_FIELD_COUNT_116,
	MARK_ID_FIELD_COUNT_117,
	MARK_ID_FIELD_COUNT_118,
	MARK_ID_FIELD_COUNT_119,
	MARK_ID_FIELD_COUNT_120,
	MARK_ID_FIELD_COUNT_121,
	MARK_ID_FIELD_COUNT_122,
	MARK_ID_FIELD_COUNT_123,
	MARK_ID_FIELD_COUNT_124,
	MARK_ID_FIELD_COUNT_125,
	MARK_ID_FIELD_COUNT_126,
	MARK_ID_FIELD_COUNT_127,
	MARK_ID_FIELD_COUNT_128,
	MARK_ID_FIELD_COUNT_129,
	MARK_ID_FIELD_COUNT_130,
	MARK_ID_FIELD_COUNT_131,
	MARK_ID_FIELD_COUNT_132,
	MARK_ID_FIELD_COUNT_133,
	MARK_ID_FIELD_COUNT_134,
	MARK_ID_FIELD_COUNT_135,
	MARK_ID_FIELD_COUNT_136,
	MARK_ID_FIELD_COUNT_137,
	MARK_ID_FIELD_COUNT_138,
	MARK_ID_FIELD_COUNT_139,
	MARK_ID_FIELD_COUNT_140,
	MARK_ID_FIELD_COUNT_141,
	MARK_ID_FIELD_COUNT_142,
	MARK_ID_FIELD_COUNT_143,
	MARK_ID_FIELD_COUNT_144,
	MARK_ID_FIELD_COUNT_145,
	MARK_ID_FIELD_COUNT_146,
	MARK_ID_FIELD_COUNT_147,
	MARK_ID_FIELD_COUNT_148,
	MARK_ID_FIELD_COUNT_149,
	MARK_ID_FIELD_COUNT_150,
	MARK_ID_FIELD_COUNT_151,
	MARK_ID_FIELD_COUNT_152,
	MARK_ID_FIELD_COUNT_153,
	MARK_ID_FIELD_COUNT_154,
	MARK_ID_FIELD_COUNT_155,
	MARK_ID_FIELD_COUNT_156,
	MARK_ID_FIELD_COUNT_157,
	MARK_ID_FIELD_COUNT_158,
	MARK_ID_FIELD_COUNT_159,
	MARK_ID_FIELD_COUNT_160,
	MARK_ID_FIELD_COUNT_161,
	MARK_ID_FIELD_COUNT_162,
	MARK_ID_FIELD_COUNT_163,
	MARK_ID_FIELD_COUNT_164,
	MARK_ID_FIELD_COUNT_165,
	MARK_ID_FIELD_COUNT_166,
	MARK_ID_FIELD_COUNT_167,
	MARK_ID_FIELD_COUNT_168,
	MARK_ID_FIELD_COUNT_169,
	MARK_ID_FIELD_COUNT_170,
	MARK_ID_FIELD_COUNT_171,
	MARK_ID_FIELD_COUNT_172,
	MARK_ID_FIELD_COUNT_173,
	MARK_ID_FIELD_COUNT_174,
	MARK_ID_FIELD_COUNT_175,
	MARK_ID_FIELD_COUNT_176,
	MARK_ID_FIELD_COUNT_177,
	MARK_ID_FIELD_COUNT_178,
	MARK_ID_FIELD_COUNT_179,
	MARK_ID_FIELD_COUNT_180,
	MARK_ID_FIELD_COUNT_181,
	MARK_ID_FIELD_COUNT_182,
	MARK_ID_FIELD_COUNT_183,
	MARK_ID_FIELD_COUNT_184,
	MARK_ID_FIELD_COUNT_185,
	MARK_ID_FIELD_COUNT_186,
	MARK_ID_FIELD_COUNT_187,
	MARK_ID_FIELD_COUNT_188,
	MARK_ID_FIELD_COUNT_189,
	MARK_ID_FIELD_COUNT_190,
	MARK_ID_FIELD_COUNT_191,
	MARK_ID_FIELD_COUNT_192,
	MARK_ID_FIELD_COUNT_193,
	MARK_ID_FIELD_COUNT_194,
	MARK_ID_FIELD_COUNT_195,
	MARK_ID_FIELD_COUNT_196,
	MARK_ID_FIELD_COUNT_197,
	MARK_ID_FIELD_COUNT_198,
	MARK_ID_FIELD_COUNT_199,
	MARK_ID_FIELD_COUNT_200,
	MARK_ID_FIELD_COUNT_201,
	MARK_ID_FIELD_COUNT_202,
	MARK_ID_FIELD_COUNT_203,
	MARK_ID_FIELD_COUNT_MAX,

	MARK_ID_SHADER_MASK_0,
	MARK_ID_SHADER_MASK_1,
	MARK_ID_SHADER_MASK_2,
	MARK_ID_SHADER_MASK_3,
	MARK_ID_SHADER_MASK_4,
	MARK_ID_SHADER_MASK_5,
	MARK_ID_SHADER_MASK_6,
	MARK_ID_SHADER_MASK_7,
	MARK_ID_SHADER_MASK_8,
	MARK_ID_SHADER_MASK_MAX,

	MARK_ID_MTL_RSC_DSCPT,
	MARK_ID_VTX_BFR_LYT_DSCPT,
	MARK_ID_GFX_PPLN_DSCPT,
	MARK_ID_RDPS_DSCPT,

	MARK_ID_MAX,

	MARK_ID_SHADER_MASK = MARK_ID_SHADER_MASK_0,
	MARK_ID_SPIRV_LENGTH = MARK_ID_SPIRV_LENGTH_0,
	MARK_ID_DESCRIPTOR_COUNT = MARK_ID_DESCRIPTOR_COUNT_0,
	MARK_ID_IDENTIFIER_NAME = MARK_ID_IDENTIFIER_NAME_0,
	MARK_ID_SPIRV_OFFSET = MARK_ID_SPIRV_OFFSET_0,
	MARK_ID_PIPELINE_OFFSET = MARK_ID_PIPELINE_OFFSET_0,
	MARK_ID_OFFSET = MARK_ID_DESCRIPTOR_OFFSET_0,
	MARK_ID_OFFSET_MAX = MARK_ID_PIPELINE_OFFSET_MAX,
	MARK_ID_DESCRIPTOR_OFFSET = MARK_ID_DESCRIPTOR_OFFSET_0,
	MARK_ID_FIELD_COUNT = MARK_ID_FIELD_COUNT_0
};

/* abstract buffer used to store the compiled shader binary */
typedef struct codegen_buffer_t
{
	/* main section writer is used to write the descriptions to the data section */
	binary_writer_t* main;
	/* data section writer is used to write in the data section, such as gfx pipeline descriptions */
	binary_writer_t* data;
	/* lazily allocated flat buffer, it will point to a valid flat buffer after calling codegen_buffer_flatten() */
	BUFFER* flat;
} codegen_buffer_t;

BEGIN_CPP_COMPATIBLE

/* constructors and destructors */
SC_API codegen_buffer_t* codegen_buffer_new();
SC_API codegen_buffer_t* codegen_buffer_create();
SC_API void codegen_destroy(codegen_buffer_t* buffer);
SC_API void codegen_release_resources(codegen_buffer_t* buffer);

/* 	flattens the codegen buffer and returns pointer to the BUFFER instance 
	buffer: the codegen_buffer_t object to be flattened
	returns pointer to the BUFFER object containing flat binaries (bytes)
 */
SC_API BUFFER* codegen_buffer_flatten(codegen_buffer_t* buffer);

END_CPP_COMPATIBLE
