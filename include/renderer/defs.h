#ifndef _DEFS_H_
#define _DEFS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <bufferlib/buffer.h>

#include <renderer/defines.h>

#define TOKEN_BUFFER_SIZE 80
#define SINGLE_LINE_COMMENT "\\"
#define MULTIPLE_LINE_COMMENT "/**/"
#define VERSION(__version__)

#define _sizeof(serialized_property) __sizeof(serialized_property)

#ifdef DEBUG
#define GOTO(label) { printf("Returned from %d\n", __LINE__); goto label; }
#else
#define GOTO(label) { goto label; }
#endif

#define INCREMENT_CHAR_PTR(ch, amount)\
do {\
	 if((ch == NULL) || (*ch == 0)) { exit(0); puts("FILE ENDED"); }\
	  ch += amount;\
	 if(*ch == 0) { exit(0); puts("FILE ENDED");} } while (false) 

#ifdef DEBUG
#define throw_error(error_str, line_no, discription)\
do{\
	printf("[ERROR]: Parsing error at line no %d \n%s: %s\n", line_no, error_str, discription);\
	exit(0);\
}while(false)
#else
#define throw_error(error_str, line_no, description)
#endif

typedef enum
{
	STSP_STATIC = 0,
	STSP_REGISTER = 1,
	STSP_EXTERN = 2,
	STSP_NONE = 3 
} _storage_specifiers;

typedef enum
{
	TYPE_SIGNED_INT8 = 0,				//char, signed char
	TYPE_SIGNED_INT16 = 1,				//short, signed short, short int, signed short int
	TYPE_SIGNED_INT32 = 2,				//int, signed int, long, long int
	TYPE_SIGNED_INT64 = 3,				//long long int

	TYPE_UNSIGNED_INT8 = 4,				//unsigned char
	TYPE_UNSIGNED_INT16 = 5,			//unsigned short, unsigned short int
	TYPE_UNSIGNED_INT32 = 6,			//unsigned int, unsigned long, unsigned long int
	TYPE_UNSIGNED_INT64 = 7,			//unsigned long long, unsigned long long int

	TYPE_FLOAT = 8,						//float
	TYPE_DOUBLE = 9,					//double
	TYPE_LONG_DOUBLE = 10,				//long double
	TYPE_STRING = 11,
	TYPE_NONE = 12,
	TYPE_INCOMPLETE = 13,
	TYPE_VOID = 14
} _type_specifiers;

typedef struct 
{
	bool is_pointer;						//true if this property is a pointer 
	bool is_const; 							//true if this property is a constant property
	_storage_specifiers  storage; 			//extern, static, register
	_type_specifiers  type; 				//int, short, long ...etc
	intptr_t address; 						//address of the property
	char name[TOKEN_BUFFER_SIZE];			//name of the property
} SerializedProperty; 

typedef struct
{
	char name[TOKEN_BUFFER_SIZE]; 			//name of this struct
	bool is_valid; 							//true, if this SerializedStruct is a valid serialized struct
	uint32_t size; 							//size of the serialized struct in bytes
	BUFFER* properties;						//list of properties
} SerializedStruct; 

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

RENDERER_API char* defs_load_text_from_file(const char* file_name);
RENDERER_API char* defs_load_text_from_file_exclude_comments(const char* file_name);

RENDERER_API int __sizeof(SerializedProperty* property); 
RENDERER_API bool isstorage(const char* string, _storage_specifiers* storage);
RENDERER_API bool istype(const char* string, _type_specifiers* type);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif