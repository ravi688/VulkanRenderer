/*BUFFERlib, this library is written in standard C language.
  Author: Ravi Prakash Singh

  Introduction: 

  This libraray is developed for implementing other High Level and Advanced Data Structures, 
  You will find something "BUFFER" , which is not directly instantiable because it contains some private data that must not be manipulated
  by the user but don't worry you will have the full control over the members of this structure with the help of getters and setters.

  This "BUFFER" can act as List, Queue, Stack, Binary Tree, Heap, Graph, Linked List etc. 
  But you must have implementation level understanding of these Data Structures to use this library.

  You may wonder, "Why you are making this kind of data structurs which can behavious like all the data structures?".
  The answer to this question is: 

  "This "BUFFER" Data structure is not limit to multi-functional, but it's main GOAL is to let the user, have a Powerful tool to create Many other
   Advaned Data Structures"
	
  (TODO)
  But other benefits of using this library are: 
  1. Better memory management
  2. High performance traversal and copying of memory objects
  3. Multithreaded traversal/iterations and can use your GPU to get more performance
  4. Better Cache management
  5. Can use your CPU and GPU simultaneously to get very high performance
  6. Neural Networks/Machine Learning Models can be implemented
  7. Memory partitions without degrading cache performance
  8. 
*/


/*Version history: 
	1_0
	1_1 
	1_2
	1_3
	1_4
	1_5
	1_6 (this)
*/

/*
	Dependencies: 

	CommonUtilites.h -> ASSERTIONS but can be removed by defining the macro BUF_NOT_INCLUDE_COMMON_UTILITIES
	CommonUtilites.o
	stdlib.h
	stdio.h
	stdint.h
	string.h
	stdbool.h
*/

/*
	Changes:
In version 1_1: 
	1. Support for LIST macros
	2. BUFget_version()
	3. Addition of BUFFER_LIB_VERSION macro
	4. Support for adjusting Size increase algorithms
	5. BUFcreate() is deprecated, use BUFcreate_new() instead
	6. BUFcreate_object() is deprecated, use BUFcreate_object_new() instead
	7. BUFget_binded_buffer() is deprecated, use BUFget_binded() instead

In version 1_2:
	1. Introduced new macros: 
		BUFpush_int8()
		BUFpush_int16()
		BUFpush_int32()
		BUFpush_int64()
		BUFpush_uint8()
		BUFpush_uint16()
		BUFpush_uint32()
		BUFpush_uint64()
		BUFget_ptr_at_typeof()
		BUFget_value_at_typeof()
	2. BUFgetptr_at() is deprecated, use BUFget_ptr_at() instead

 In version 1_3:
 	1. Introduces new macros:
 		BUFcontains(void*, comparer_t) //returns bool
 		BUFclear_int8(void*)
 		BUFclear_int16(void*)
 		BUFclear_int32(void*)
 		BUFclear_int64(void*)
 		BUFclear_uint8(void*)
 		BUFclear_uint16(void*)
 		BUFclear_uint32(void*)
 		BUFclear_uint64(void*)
 		BUFclear_float(void*)
 		BUFclear_double(void*)
 		LIST_CONTAINS(object ptr)
 		LIST_REMOVE(object ptr)
 	2. Bug Fixes: 
 		1. return value of BUFfind_index_of() is changed from buf_ucount_t to buf_count_t becuase this function returns -1 (signed value)
		2. now you can pass 0 capacity when calling BUFcreate_new(NULL, size of element, capacity = 0, offset) or LIST_NEW(type, capacity = 0)
	3. New Features Added
		BUFpush_binded() , call this before doing anything in any function to work with BUFFER* objects
		BUFpop_binded(), call this at the end of function definition after BUFpush_binded()

In version 1_4: 
	1. Modifications: 
		BUFinsert() macro is defined as BUFinsert_at()
		BUFpush_binded() and BUFpop_binded() is now implemend with Macro, 
		now you can embedd this call into recursive functions unlike in version 1.3
		BUF_BUFFER_OBJECT_SIZE is now defined as BUFget_buffer_object_size()
	
	2. New Features: 
		1.New Macros: 
		 	BUFget_bottom() 	//defined as BUFget_ptr()
		 	BUFget_top()		//defined as BUFpeek_ptr()
		 	BUF_INVALID_INDEX   //defined as (buf_ucount_t(-1)) -> NOTE: This is valid only for buf_ucount_t typedefinitions

		2.Customizable typedefintion for buf_count_t and buf_ucount_t,
		 for example: if you need to store more than the max value of uint32_t objects then you can define buf_ucount_t for uint64_t and buf_count_t for int64_t
		 like this: 
		  #define BUF_UCOUNT_TYPE
		  #define BUF_COUNT_TYPE
		  typedef uint64_t buf_ucount_t; 
		  typedef int64_t count;
		 NOTE: You will need to recompile this library at every change in typedefinition for buf_count_t or buf_ucount_t

		3.Now you can query the BUFFER object size whenever you change the typedefinition of buf_ucount_t or buf_count_t
			BUFget_buffer_object_size(); //returns the sizeof(struct BUFFER)
		
		4.Set of New functions for manipulating the continguous buffer: 
		  BUFpush_pseudo(buf_ucount_t count);						//pushes 'count' empty elements into the buffer
		  BUFpop_pseudo(buf_ucount_t count); 						//pops 'count' elements from the buffer
		  BUFinsert_pseudo(buf_ucount_t index, buf_ucount_t count); //inserts 'count' empty elements starting from 'index' to 'index' + 'count'
		  BUremove_pseudo(buf_ucount_t index, buf_ucount_t count);	//removes 'count' elements starting from 'index' to 'index' + 'count' from the buffer
		
		5.Added special callbacks: 
			void (on_post_resize*)(void)
			void (on_pre_resize*)(void)
		6.Added new setters for these callbacks
			void BUFset_on_post_resize(on_post_resize); 	//will be called after, whenever there is a resizing of the underlying buffer
			void BUFset_on_pre_resize(on_pre_resize); 		//will be called before, whenever there is a resizing of the underlying buffer

		7.New Macro Switches are added
			BUF_DEBUG and BUF_RELEASE, NOTE: BUF_DEBUG is already defined

	3. Bux Fixes: 
		BUFget_at() now throws an exception "Index Out of Range" if the passed index is out of range (elmeent count - 1)
		BUFgetptr_at() now throws an exception "Index Out of Range" if the passed index is out of range (element count - 1)
		BUFset_at() now throws an exception "Index Out of Range" if the passed index is out of range (element count - 1)
		BUFinsert_at_noalloc() now throws an exception "Buffer Overflow" if the passed index is out of range (capacity - 1)
		BUFinsert_at() now throws an exception "Buffer Overflow" if the passed index is out of range (capacity - 1)
		BUFremove_at_noshift() now throws an exception "Index Out Of Range" if the passed index is out of range (element count - 1)
		BUFremove_at() now throws an exception "Index Out Of Range" if the passed index is out of range (element count - 1)

	4. Performance Improvements
	
	5. Security Improvements
		Now each function can repond to Memory Allocation Failure Exception
In version 1_5: 
	1. Bug fixes: 
		1. Bug: when BUF_RELEASE and BUF_DEBUG both defined then BUF_DEBUG wins; 
		   Fix: so now BUF_RELEASE will win
		2. Bug: when BUF_NOT_INCLUDE_COMMON_UTILITIES is not giving expected results
		   Fix: by defining BUF_NOT_INCLUDE_COMMON_UTILITIES, you are replacing BUF_ASSERT and BUF_ASSERT_NOPRINT with default assertions
		   		by undefining BUF_NOT_INCLUDE_COMMON_UTILITIES, you are replacing BUF_ASSERT and BUF_ASSERT_NOPRINT with ASSERT and ASSERT_NOPRINT respectively, defined in CommonUtilities.h
	2. Modifications: 
		1. if none of the BUF_RELEASE and BUF_DEBUG is defined then BUF_DEBUG will be defined automatically
		2. return type of BUFremove_at_noshift is changed from void to bool
		3. return type of BUFremove_at is changed from void to bool
	3. New features: 
		1. BUFget_clone: returns the clone of a binded buffer

In version 1_6: 
	1. Buf fixes: 
		1. Bug: BUFpush doesn't pushes correct value at the index zero; 
			 Fix: BUFpush now uses BUFresize and BUFset_at internally to fix this bug.
	2. New features / API improvements: 
		1. New set of functions to make your task easier
				1.BUFcopy_to
					copies the buffer content to the destination buffer.
					NOTE: if destination buffer doesn't have enough capacity then it resizes the destination buffer to BUFget_buffer_size()
					NOTE: it also sets the element_count of destination buffer to that of binded_buffer.
				2.BUFcopy_construct
					This is nothing but BUFcreate followed by BUFcopy_to
				3.BUFmove_to
					This is nothing but BUFcopy_to to destination-> BUFfree the source buffer (binded_buffer).
		2. Now it is possible to get the exact location of an exception thrown from BUF prefixed functions
		3. It is not compulsory to bind buffers before using them, you can use new set of functions to simply pass the buffer pointer at the time of function call.
			followings are the new set of functions: 
				1. buf_free()
				2. buf_create()
				3. buf_push()
				4. buf_fit()
				5. buf_contains()
				6. buf_clear()
				7. buf_remove()
				8. buf_insert()
				9. buf_push_pseudo()
				10. buf_pop_pseudo()
				11. buf_get_buffer_object_size()
				12. buf_get_top()
				13. buf_get_bottom()
				14. buf_insert_pseudo()
				15. buf_remove_pseudo()
				16. buf_set_on_post_resize()
				17. buf_set_on_pre_resize()
				18. buf_push_char()
				19. buf_push_float()
				20. buf_push_double()
				21. buf_push_int()
				22. buf_push_ptr()
				23. buf_push_value()
				24. buf_clear_int()
				25. buf_clear_uint()
				26. buf_clear_short()
				27. buf_clear_ushort()
				28. buf_clear_long()
				29. buf_clear_ulong()
				30. buf_clear_int8()
				31. buf_clear_int16()
				32. buf_clear_int32()
				33. buf_clear_int64()
				34. buf_clear_uint8()
				35. buf_clear_uint16()
				36. buf_clear_uint32()
				37. buf_clear_uint64()
				38. buf_clear_float()
				39. buf_clear_double()
				40. buf_push_binded()
				41. buf_pop_binded()
				42. buf_push_int8()
				43. buf_push_int16()
				44. buf_push_int32()
				45. buf_push_int64()
				46. buf_push_uint8()
				47. buf_push_uint16()
				48. buf_push_uint32()
				49. buf_push_uint64()
				50. buf_get_ptr_at_typeof()
				51. buf_get_ptr_at()
				52. buf_get_value_at_typeof()
				53. buf_remove_char()
				54. buf_remove_float()
				55. buf_remove_double()
				56. buf_remove_int()
				57. buf_remove_ptr()
				58. buf_remove_value()
				59. buf_insert_char()
				60. buf_insert_float()
				61. buf_insert_double()
				62. buf_insert_int()
				63. buf_insert_ptr()
				64. buf_insert_value()
				63. buf_pop_char()
				64. buf_pop_float()
				65. buf_pop_double()
				66. buf_pop_int()
				67. buf_pop_ptr()
				68. buf_pop_value()
				69. buf_contains_char()
				70. buf_contains_float()
				71. buf_contains_double()
				72. buf_contains_int()
				73. buf_contains_ptr()
				74. buf_contains_value()
				75. buf_create_new()
				76. buf_remove_at()
				77. buf_insert_at()
				78. buf_remove_noshift()
				79. buf_remove_at_noshift()
				80. buf_insert_at_noalloc()
				81. buf_log()
				82. buf_create_object_new()
				83. buf_get_binded()
				84. buf_get_version()
				85. buf_peek()
				86. buf_peek_ptr()
				87. buf_get_at()
				88. buf_set_at()
				89. buf_find_index_of()
				90. buf_get_element_count()
				91. buf_get_element_size()
				92. buf_get_capacity()
				93. buf_get_ptr()
				99. buf_get_data() //#define buf_get_data buf_get_ptr
				100. buf_set_element_count()
				101. buf_set_element_size()
				102. buf_set_ptr()
				103. buf_set_data() //#define buf_set_data buf_set_ptr
				104. buf_set_ptr()
				105. buf_set_auto_managed()
				106. buf_is_stack_allocated()
				107. buf_is_heap_allocated()
*/

/*Future modifications:

New Feature and Performance improvement request, must be implemented in BUFFERlib version 1.2
	1. contiguous and same type of data memory blocks defragmentation
	2. support for multiple value pushes in one function call, better utilization of cache memory

*/


#ifndef _BUFFER_H_
#define _BUFFER_H_

/*********************************************************************/
/*Configuration, included in version 1.5*/
/*included in version 1.5*/
#define UCOUNT_TYPE uint64_t
/*included in version 1.5*/
#define COUNT_TYPE int64_t
/*included in version 1.5*/
#define BUF_NOT_INCLUDE_COMMON_UTILITIES
/*included in version 1.5*/
#define BUF_DEBUG 

/*********************************************************************/




#define BUFFER_LIB_VERSION 0x0016 //1.6 (this)


#if BUFFER_LIB_VERSION >= 0x0011
#	define BUF_GET_VERSION_SUPPROTED
#	define BUF_SIZE_INCREASE_MODE_SUPPORTED
#	define BUF_CREATE_NEW_SUPPORTED
#	define BUF_CREATE_OBJECT_NEW_SUPPORTED
#	define BUF_LIST_SUPPORTED
#	define BUF_GET_BINDED_SUPPORTED
#	define BUF_INVALID_INDEX ((buf_ucount_t)(-1))
#endif

#if BUFFER_LIB_VERSION >= 0x0013
#  define BUF_CLEAR_OVERLOADS_SUPPORTED
#  define BUF_CONTAINS_SUPPORTED
#  define BUF_PUSH_POP_BINDED_SUPPORTED
#  undef BUF_INVALID_INDEX
#  define BUF_INVALID_INDEX  (-1)
#endif

#if BUFFER_LIB_VERSION >= 0x0012
#	define BUF_PUSH_XINTX_SUPPORTED
#	define BUF_GET_PTR_AT_TYPEOF_SUPPORTED
#   define BUF_GETPTR_DEPRECATED
# 	define BUF_GET_VALUE_AT_SUPPROTED
#	undef BUF_INVALID_INDEX
#	define BUF_INVALID_INDEX ((buf_ucount_t)(-1))
#endif

#if BUFFER_LIB_VERSION >= 0x0014
#	define BUF_TYPE_DEFINITION_CUSTOMIZATION_SUPPORTED
#	define BUF_GET_BUFFER_OBJECT_SIZE_FUNCTION_SUPPORTED
#	define BUF_GET_BOTTOM_AND_TOP_SUPPORTED
# 	define BUF_PSEUDO_INSERT_REMOVE_PUSH_AND_POP_SUPPORTED
#	define BUF_INSERT_SUPPORTED
#	define BUF_CALLBACKS_SUPPORTED
#   undef BUF_INVALID_INDEX
#	define BUF_INVALID_INDEX ((buf_ucount_t)(-1))
#endif

#if BUFFER_LIB_VERSION < 0x0016
#	ifndef USE_LEGACY
#		define USE_LEGACY
#	endif
#endif

#ifdef BUF_RELEASE
#	if defined(BUF_DEBUG)
#		undef(BUF_DEBUG)
#	endif
#endif

#if !defined(BUF_RELEASE) && !defined(BUF_DEBUG)
#	define BUF_DEBUG
#endif

#	ifndef USE_LEGACY
#		ifdef BUF_DEBUG
#			define function_signature_void(return_type, function_name) return_type __##function_name(uint64_t line_no, const char* caller_name, const char* file_name)
#			define function_signature(return_type, function_name, ...)  return_type __##function_name(__VA_ARGS__, uint64_t line_no, const char* caller_name, const char* file_name)
#			define define_alias_function_void_macro(function_name) __##function_name(__LINE__, __FUNCTION__, __FILE__)
#			define define_alias_function_macro(function_name, ...) __##function_name(__VA_ARGS__, __LINE__, __FUNCTION__, __FILE__)
#			define GOOD_ASSERT(bool_value, string, ...) do { if(!(bool_value)) {  printf("[Error] %s, at %lu, %s, %s\n", string, line_no, caller_name, file_name); exit(-1); } } while(false)
#		elif defined(BUF_RELEASE)
#			define function_signature_void(return_type, function_name) return_type function_name()
#			define function_signature(return_type, function_name, ...) return_type function_name(__VA_ARGS__)
#			define define_alias_function_macro(function_name, ...) function_name(__VA_ARGS__)
#			define define_alias_function_void_macro(function_name) function_name()
#			define GOOD_ASSERT(bool_value, string, ...) do { if(!(bool_value)) return __VA_ARGS__; } while(false)
#		endif
#	elif defined(USE_LEGACY)
#			define function_signature_void(return_type, function_name) return_type function_name()
#			define function_signature(return_type, function_name, ...) return_type function_name(__VA_ARGS__)
#			define define_alias_function_macro(function_name, ...) function_name(__VA_ARGS__)
#			define define_alias_function_void_macro(function_name) function_name()
#			define GOOD_ASSERT BUF_ASSERT
#	endif/*USE_LEGACY*/



//Modified in version 1.5 (upgraded from 1.4)
#ifdef BUF_DEBUG
#	ifdef BUF_NOT_INCLUDE_COMMON_UTILITIES
//#		define BUF_ASSERT(boolean, return_value, string) __BUF_ASSERT(boolean, string, return_value)
//#		define BUF_ASSERT_NOPRINT(boolean, return_value) __BUF_ASSERT_NOPRINT(boolean, return_value)
#		define BUF_ASSERT(boolean, string, ...) do { if(!(boolean)) { printf("[Error]: Assertion Failed, %s, at %s, line: %lu\n", string, __FUNCTION__, __LINE__); return __VA_ARGS__; } } while(false)
#		define BUF_ASSERT_NOPRINT(boolean, ...) do { if(!(boolean)) { return __VA_ARGS__; } } while(false)
#	else
#		define BUF_ASSERT ASSERT
#		define BUF_ASSERT_NOPRINT ASSERT_NOPRINT
#	endif/*BUF_NOT_INCLUDE_COMMON_UTILITIES*/
#else
#	define BUF_ASSERT(boolean, string, ...) //nothing
#	define BUF_ASSERT_NOPRINT(boolean, ...) //nothing
#endif/*BUF_DEBUG*/

#include <stdint.h>
#include <stdbool.h>

#define BUF_8BIT   /*1 byte*/
#define BUF_16BIT  /*2 byte*/
#define BUF_32BIT  /*4 byte*/
#define BUF_64BIT  /*8 byte*/
#define BUF_128BIT /*16 byte*/

#if BUFFER_LIB_VERSION >= 0x0015
#	define BUF_BUFFER_OBJECT_SIZE BUFget_buffer_object_size()
#else
#error "BUF_BUFFER_OBJECT_SIZE 40 define is no longer valid, You are using lower version ( < 1.5 ) of BUFFERlib, it is modified to correct value in version 1.5"
#	define BUF_BUFFER_OBJECT_SIZE 40 /*bytes*/
#endif/*BUFFER_LIB_VERSION*/

#ifdef BUF_TYPE_DEFINITION_CUSTOMIZATION_SUPPORTED
#ifndef BUF_UCOUNT_TYPE
#	define BUF_UCOUNT_TYPE
	#if BUFFER_LIB_VERSION >= 0x0015
		typedef UCOUNT_TYPE buf_ucount_t;
	#else
		typedef uint64_t buf_ucount_t;
	#endif/*BUFFER_LIB_VERSION*/
#endif/*BUF_UCOUNT_TYPE*/
#ifndef BUF_COUNT_TYPE
#	define BUF_COUNT_TYPE
	#if BUFFER_LIB_VERSION >= 0x0015
		typedef COUNT_TYPE buf_count_t;
	#else
		typedef int64_t buf_count_t;
	#endif/*BUFFER_LIB_VERSION*/
#endif/*BUF_COUNT_TYPE*/
#else
#	define buf_ucount_t uint32_t
# 	define buf_count_t  int32_t
#endif/*BUF_TYPE_DEFINITION_CUSTOMIZATION_SUPPORTED*/

#ifdef BUF_GET_BUFFER_OBJECT_SIZE_FUNCTION_SUPPORTED
#	undef BUF_BUFFER_OBJECT_SIZE 
#	define BUF_BUFFER_OBJECT_SIZE BUFget_buffer_object_size()
	#ifndef USE_LEGACY
		function_signature_void(uint64_t, BUFget_buffer_object_size);
		#define BUFget_buffer_object_size() define_alias_function_void_macro(BUFget_buffer_object_size)
	#else
		uint64_t BUFget_buffer_object_size();
	#endif/*USE_LEGACY*/
#endif/*BUF_GET_BUFFER_OBJECT_SIZE_FUNCTION_SUPPORTED*/

#ifdef BUF_GET_BOTTOM_AND_TOP_SUPPORTED
# 	define BUFget_bottom() BUFget_ptr()
#	define BUFget_top() BUFpeek_ptr()
#endif/*BUF_GET_BOTTOM_AND_TOP_SUPPORTED*/

#ifdef BUF_PSEUDO_INSERT_REMOVE_PUSH_AND_POP_SUPPORTED
	function_signature(void, BUFpush_pseudo, buf_ucount_t count);
	function_signature(void, BUFpop_pseudo, buf_ucount_t count);
	function_signature(void, BUFinsert_pseudo, buf_ucount_t index, buf_ucount_t count);
	function_signature(void, BUFremove_pseudo, buf_ucount_t index, buf_ucount_t count);

	#define BUFpush_pseudo(...) define_alias_function_macro(BUFpush_pseudo, __VA_ARGS__)
	#define BUFpop_pseudo(...) define_alias_function_macro(BUFpop_pseudo, __VA_ARGS__)
	#define BUFinsert_pseudo(...) define_alias_function_macro(BUFinsert_pseudo, __VA_ARGS__)
	#define BUFremove_pseudo(...) define_alias_function_macro(BUFremove_pseudo, __VA_ARGS__)

#endif/*BUF_PSEUDO_INSERT_REMOVE_PUSH_AND_POP_SUPPORTED*/

#ifdef BUF_CALLBACKS_SUPPORTED
	function_signature(void, BUFset_on_post_resize, void (*on_post_resize)(void));
	function_signature(void, BUFset_on_pre_resize, void (*on_pre_resize)(void));

	#define BUFset_on_post_resize(...) define_alias_function_macro(BUFset_on_post_resize, __VA_ARGS__)
	#define BUFset_on_pre_resize(...) define_alias_function_macro(BUFset_on_pre_resize, __VA_ARGS__)

#endif/*BUF_CALLBACKS_SUPPORTED*/

#ifdef BUF_INSERT_SUPPORTED
#	define BUFinsert(index, value_ptr) BUFinsert_at(index, value_ptr)
#endif/*BUF_INSERT_SUPPORTED*/

#define BUFpush_char(value) { char _char = value; BUFpush(&_char); }
#define BUFpush_float(value) { float _float = value; BUFpush(&_float); }
#define BUFpush_int(value) { int _int = value; BUFpush(&_int); }
#define BUFpush_ptr(value) { void* _vd = value; BUFpush(&_vd); }
#define BUFpush_value(value) BUFpush(&value)

#ifdef BUF_CLEAR_OVERLOADS_SUPPORTED
#	define BUFclear_int(value) BUFclear_int32(value)
#	define BUFclear_uint(value) BUFclear_uint32(value)
#	define BUFclear_short(value) BUFclear_int16(value)
#	define BUFclear_ushort(value) BUFclear_uint16(value)
#	define BUFclear_long(value) BUFclear_int64(value)
#	define BUFclear_ulong(value) BUFclear_uint16(value)
#	define BUFclear_int8(/*int8_t*/ value)  { int8_t _v = value; BUFclear(&_v); }
#	define BUFclear_int16(/*int16_t*/ value) { int16_t _v = value; BUFclear(&_v); }
#	define BUFclear_int32(/*int32_t*/ value) { int32_t v = value; BUFclear(&v); } 
#	define BUFclear_int64(/*int64_t*/ value) { int64_t v = value; BUFclear(&v); } 
#	define BUFclear_uint8(/*uint8_t*/ value) { uint8_t v = value; BUFclear(&v); } 
#	define BUFclear_uint16(/*uint16_t*/ value) { uint16_t v = value; BUFclear(&v); }
#	define BUFclear_uint32(/*uint32_t*/ value) { uint32_t v = value; BUFclear(&v); } 
#	define BUFclear_uint64(/*uint64_t*/ value) { uint64_t v = value; BUFclear(&v); }
#	define BUFclear_float(/*float*/ value) { float v = value; BUFclear(&v); }
#	define BUFclear_double(/*double*/ value) { double v = value; BUFclear(&v); }
#endif

#ifdef BUF_CONTAINS_SUPPORTED
#	define BUFcontains(void_ptr_value, comparer_t_c) (BUF_INVALID_INDEX != BUFfind_index_of(void_ptr_value, comparer_t_c))
#endif

#ifdef BUF_PUSH_POP_BINDED_SUPPORTED

/*Example*/
/*

void some_function()
{
	//this function will the save previous BUFFER* pointer
	BUFpush_binded();

	BUFbind(some_other_buffer);

	BUFpush(&some_value);

	//this function will reassign the previous BUFFER* pointer
	BUFpop_binded();
}

*/
#if BUFFER_LIB_VERSION >= 0x0014
#	define BUFpush_binded() BUFFER* _buf__ = BUFget_binded()
# 	define BUFpop_binded() BUFbind(_buf__)
#else
	void BUFpush_binded();
	void BUFpop_binded();
#endif
#endif

#ifdef BUF_PUSH_XINTX_SUPPORTED
#	define BUFpush_int8(value) { int8_t _int= value; BUFpush(&_int); }
#	define BUFpush_int16(value) { int16_t _int= value; BUFpush(&_int); }
#	define BUFpush_int32(value) { int32_t _int= value; BUFpush(&_int); }
#	define BUFpush_int64(value) { int64_t _int= value; BUFpush(&_int); }
#	define BUFpush_uint8(value) { uint8_t _int= value; BUFpush(&_int); }
#	define BUFpush_uint16(value) { uint16_t _int= value; BUFpush(&_int); }
#	define BUFpush_uint32(value) { uint32_t _int= value; BUFpush(&_int); }
#	define BUFpush_uint64(value) { uint64_t _int= value; BUFpush(&_int); }
#endif

#ifdef BUF_GET_PTR_AT_TYPEOF_SUPPORTED
#	define BUFget_ptr_at_typeof(type, index) ((type*)BUFgetptr_at(index))
#endif

#ifdef BUF_GETPTR_DEPRECATED
# 	define BUFget_ptr_at(index) BUFgetptr_at(index)
#endif

#ifdef BUF_GET_VALUE_AT_SUPPROTED
# 	define BUFget_value_at_typeof(type, index) (*BUFget_ptr_at_typeof(type, index))
#endif

#define BUFremove_char(value, comparer) { char _char = value; BUFremove(&_char, comparer); }
#define BUFremove_float(value, comparer) { float _float = value; BUFremove(&_float, comparer); }
#define BUFremove_int(value, comparer) { int _int = value; BUFremove(&_int, comparer); }
#define BUFremove_value(value, comparer) BUFremove(&value, comparer)

#ifdef BUF_LIST_SUPPORTED

#define LIST(type) BUFFER*
#define LIST_NEW(type, capacity) BUFcreate_new(NULL, sizeof(type), capacity, 0)
#define LIST_BIND(list) BUFbind(list)
#define LIST_UNBIND() BUFunbind()
#define LIST_GET_BINDED() BUFget_binded_buffer()
#define LIST_CONTAINS(object) BUFcontains(object)
#define LIST_REMOVE(object, comparer) BUFremove(object, comparer)
#define LIST_DESTROY() BUFfree()
#define LIST_ADD(object) BUFpush(object)
#define LIST_GET(index, out_value_ptr) BUFget_at(index , out_value_ptr)
#define LIST_GET_PTR(type,index) ((type*)BUFgetptr_at(index))
#define LIST_SET(index, in_value_ptr) BUFset_at(index, in_value_ptr)
#define LIST_CLEAR() BUFclear(NULL)
#define LIST_FIT() BUFfit()
#define LIST_PEEK_PTR(type) ((type*)BUFpeek_ptr()) 
#define LIST_PEEK(out_value_ptr) BUFpeek(out_value_ptr)
#define LIST_POP(out_value_ptr) BUFpop(out_value_ptr)
#define LIST_GET_COUNT() BUFget_element_count()
#define LIST_GET_DATA(type) ((type*)BUFget_ptr())
#endif

typedef struct __BUFFER BUFFER;

#ifdef __cplusplus
extern "C" { 
#endif

#ifdef BUF_GET_VERSION_SUPPORTED
int BUFget_version();
#endif
#ifdef BUF_SIZE_INCREASE_MODE_SUPPORTED
typedef enum 
{
	BUF_SIZE_INCRESE_MODE_100,
	BUF_SIZE_INCREASE_MODE_50,
	BUF_SIZE_INCREASE_MODE_25,
	BUF_SIZE_INCREASE_MODE_15,
	BUF_SIZE_INCREASE_MODE_5,
	BUF_SIZE_INCREASE_MODE_ONE_ELEMENT,
	BUF_SIZE_INCREASE_MODE_AUTO
} BUF_SIZE_INCREASE_MODE; 

void BUFset_size_increase_mode(BUF_SIZE_INCREASE_MODE mode); 
#endif

#ifdef BUF_CREATE_NEW_SUPPORTED
#define BUFcreate_new(__buffer_ptr, element_size, capacity, offset) BUFcreate(__buffer_ptr, element_size, capacity, offset)
#endif
#ifdef BUF_CREATE_OBJECT_NEW_SUPPORTED
#define BUFcreate_object_new(uint8_t_bytes_ptr) BUFcreate_object(uint8_t_bytes_ptr)
#endif
#ifdef BUF_GET_BINDED_SUPPORTED
#define BUFget_binded() BUFget_binded_buffer()
#endif

#ifndef USE_LEGACY
function_signature(void, BUFreverseb, void* ptr_to_buffer, buf_ucount_t element_size, buf_ucount_t element_count);
function_signature(void, BUFbind, BUFFER* buffer);
function_signature_void(void, BUFunbind);
function_signature_void(void, BUFlog);
function_signature(void, BUFtraverse_elements, buf_ucount_t start, buf_ucount_t end, void (*func)(void* /*element ptr*/, void* /*args ptr*/), void* args);
function_signature(bool, BUFisStackAllocated, BUFFER* buffer); 
function_signature(bool, BUFisHeapAllocated, BUFFER* buffer);
function_signature_void(void, BUFfree); 
function_signature(BUFFER*, BUFcreate_object, uint8_t* bytes);
function_signature(BUFFER*, BUFcreate, BUFFER* __buffer, buf_ucount_t element_size, buf_ucount_t capacity, buf_ucount_t offset);
function_signature_void(void, BUFfit);
function_signature(void, BUFpeek, void* out_value);  
function_signature_void(void*, BUFpeek_ptr); 
function_signature(void, BUFpop, void* out_value); 
function_signature(void, BUFpush, void* in_value); 
function_signature(bool, BUFremove, void* object, bool (*comparer)(void*, void*));
function_signature(bool, BUFremove_noshift, void* object, bool (*comparer)(void*, void*));
function_signature(void, BUFclear, void* clear_value); 
#if BUFFER_LIB_VERSION >= 0x0015
function_signature(bool, BUFremove_at_noshift, buf_ucount_t index , void* out_value);
#else
function_signature(void, BUFremove_at_noshift, buf_ucount_t index , void* out_value);
#endif
#if BUFFER_LIB_VERSION >= 0x0015
function_signature(bool, BUFremove_at, buf_ucount_t index , void* out_value);
#else
function_signature(void, BUFremove_at, buf_ucount_t index , void* out_value);
#endif
function_signature(void, BUFinsert_at, buf_ucount_t index , void* in_value);
function_signature(void, BUFinsert_at_noalloc, buf_ucount_t index , void* in_value , void* out_value);
#if BUFFER_LIB_VERSION >= 0x0014
	function_signature(buf_ucount_t, BUFfind_index_of, void* value, bool (*comparer)(void*, void*));
#elif BUFFER_LIB_VERSION >= 0x0013
	function_signature(buf_count_t, BUFfind_index_of, void* value, bool (*comparer)(void*, void*)); 
#else
	function_signature(buf_ucount_t, BUFfind_index_of, void* value, bool (*comparer)(void*, void*)); 
#endif

function_signature(void, BUFset_at, buf_ucount_t index , void* in_value);
function_signature(void, BUFset_capacity, buf_ucount_t capacity);
function_signature(void, BUFset_ptr, uint8_t* ptr); 
function_signature(void, BUFset_element_count, buf_ucount_t element_count); 
function_signature(void, BUFset_offset, buf_ucount_t offset);
function_signature(void, BUFset_offset_bytes, uint8_t* offset_bytes);
function_signature(void, BUFset_element_size, buf_ucount_t element_size);
function_signature(void, BUFset_auto_managed, bool value);
function_signature(void, BUFget_at, buf_ucount_t index, void* out_value);
function_signature(void*, BUFgetptr_at, buf_ucount_t index);
function_signature(BUFFER*, BUFcopy_to, BUFFER* destination);
function_signature(BUFFER*, BUFmove_to, BUFFER* destination);
function_signature(BUFFER*, BUFcopy_construct, BUFFER* source);
function_signature(void, BUFset_on_free, void (*free)(void*));

function_signature(void, BUFresize, buf_ucount_t new_capacity);
function_signature(void, BUFclear_buffer, void* clear_value);
function_signature_void(bool, BUFis_auto_managed);

function_signature_void(buf_ucount_t, BUFget_capacity);
function_signature_void(void*, BUFget_ptr); 
function_signature_void(buf_ucount_t, BUFget_element_count);  
function_signature_void(buf_ucount_t, BUFget_offset);
function_signature_void(void*, BUFget_offset_bytes);
function_signature_void(buf_ucount_t, BUFget_element_size);  
function_signature_void(BUFFER*, BUFget_binded_buffer);
function_signature_void(BUFFER*, BUFget_clone);
function_signature_void(buf_ucount_t, BUFget_buffer_size);

#define BUFreverseb(...) 									define_alias_function_macro(BUFreverseb, __VA_ARGS__)
#define BUFisHeapAllocated(...) 					define_alias_function_macro(BUFisHeapAllocated, __VA_ARGS__)
#define BUFisStackAllocated(...) 					define_alias_function_macro(BUFisStackAllocated, __VA_ARGS__)
#define BUFbind(...) 											define_alias_function_macro(BUFbind, __VA_ARGS__)
#define BUFcreate_object(...) 						define_alias_function_macro(BUFcreate_object, __VA_ARGS__)
#define BUFcreate(...) 										define_alias_function_macro(BUFcreate, __VA_ARGS__)
#define BUFget_at(...) 										define_alias_function_macro(BUFget_at, __VA_ARGS__)
#define BUFset_at(...) 										define_alias_function_macro(BUFset_at, __VA_ARGS__)
#define BUFgetptr_at(...) 								define_alias_function_macro(BUFgetptr_at, __VA_ARGS__)
#define BUFpop(...) 											define_alias_function_macro(BUFpop, __VA_ARGS__)
#define BUFpush(...) 											define_alias_function_macro(BUFpush, __VA_ARGS__)
#define BUFremove(...) 										define_alias_function_macro(BUFremove, __VA_ARGS__)
#define BUFremove_noshift(...) 						define_alias_function_macro(BUFremove_noshift, __VA_ARGS__)
#define BUFclear(...) 										define_alias_function_macro(BUFclear, __VA_ARGS__)
#define BUFremove_at_noshift(...) 				define_alias_function_macro(BUFremove_at_noshift, __VA_ARGS__)
#define BUFremove_at(...) 								define_alias_function_macro(BUFremove_at, __VA_ARGS__)
#define BUFinsert_at(...) 								define_alias_function_macro(BUFinsert_at, __VA_ARGS__)
#define BUFinsert_at_noalloc(...) 				define_alias_function_macro(BUFinsert_at_noalloc, __VA_ARGS__)
#define BUFfind_index_of(...) 						define_alias_function_macro(BUFfind_index_of, __VA_ARGS__)
#define BUFset_ptr(...) 									define_alias_function_macro(BUFset_ptr, __VA_ARGS__)
#define BUFset_auto_managed(...) 					define_alias_function_macro(BUFset_auto_managed, __VA_ARGS__)
#define BUFset_offset(...) 								define_alias_function_macro(BUFset_offset, __VA_ARGS__)
#define BUFresize(...) 										define_alias_function_macro(BUFresize, __VA_ARGS__)
#define BUFclear_buffer(...) 							define_alias_function_macro(BUFclear_buffer, __VA_ARGS__)
#define BUFtraverse_elements(...) 				define_alias_function_macro(BUFtraverse_elements, __VA_ARGS__)
#define BUFset_offset_bytes(...) 					define_alias_function_macro(BUFset_offset_bytes, __VA_ARGS__)
#define BUFget_offset_bytes()							define_alias_function_void_macro(BUFget_offset_bytes)
#define BUFset_capacity(...) 							define_alias_function_macro(BUFset_capacity, __VA_ARGS__)
#define BUFset_element_count(...)  				define_alias_function_macro(BUFset_element_count, __VA_ARGS__)
#define BUFset_element_size(...)   				define_alias_function_macro(BUFset_element_size, __VA_ARGS__)
#define BUFset_ptr(...)   								define_alias_function_macro(BUFset_ptr, __VA_ARGS__)
#define BUFget_ptr()    									define_alias_function_void_macro(BUFget_ptr)
#define BUFpeek(...) 											define_alias_function_macro(BUFpeek, __VA_ARGS__)
#define BUFcopy_to(...)  									define_alias_function_macro(BUFcopy_to, __VA_ARGS__)
#define BUFmove_to(...)    								define_alias_function_macro(BUFmove_to, __VA_ARGS__)
#define BUFcopy_construct(...)      			define_alias_function_macro(BUFcopy_construct, __VA_ARGS__)
#define BUFset_on_free(...)          			define_alias_function_macro(BUFset_on_free, __VA_ARGS__)

#define BUFfit() 													define_alias_function_void_macro(BUFfit)
#define BUFget_capacity() 								define_alias_function_void_macro(BUFget_capacity)
#define BUFget_binded_buffer()				 		define_alias_function_void_macro(BUFget_binded_buffer)
#define BUFget_buffer_size() 							define_alias_function_void_macro(BUFget_buffer_size)
#define BUFpeek_ptr() 										define_alias_function_void_macro(BUFpeek_ptr)
#define BUFunbind() 											define_alias_function_void_macro(BUFunbind)
#define BUFlog() 													define_alias_function_void_macro(BUFlog)
#define BUFfree() 												define_alias_function_void_macro(BUFfree)
#define BUFget_element_count() 						define_alias_function_void_macro(BUFget_element_count)
#define BUFget_element_size() 						define_alias_function_void_macro(BUFget_element_size)
#define BUFget_clone() 										define_alias_function_void_macro(BUFget_clone)
#define BUFget_offset() 									define_alias_function_void_macro(BUFget_offset)
#define BUFis_auto_managed()  						define_alias_function_void_macro(BUFis_auto_managed)

#endif/*USE_LEGACY*/

#ifdef USE_LEGACY
/*Begin: BUFFER*/
void BUFreverseb(void* ptr_to_buffer , buf_ucount_t element_size, buf_ucount_t element_count); 

bool BUFisHeapAllocated(BUFFER* buffer); 
bool BUFisStackAllocated(BUFFER* buffer); 

/* Binds the buffer to the binded buffer*/
void BUFbind(BUFFER* buffer) ; 
/* Unbinds the current binded buffer*/
void BUFunbind();  
/* Logs the details of buffer*/
void BUFlog();
/* Frees all the memory blocks allocated in the buffer as well as buffer object*/
void BUFfree();

BUFFER* BUFcreate_object(uint8_t* bytes); 
/* Creates a new buffer , allocates buffer object and with initial element capacity of 'capacity' and each element would have size 'element size' in bytes*/
BUFFER* BUFcreate(BUFFER* __buffer, buf_ucount_t element_size, buf_ucount_t capacity, buf_ucount_t offset);
/* Fills the out_value memory block with the content at index 'index'*/
void BUFget_at(buf_ucount_t index , void* out_value);
/* Fills the memory block referenced by index with value 'in_value' having size buffer's element size in bytes*/
void BUFset_at(buf_ucount_t index , void* in_value) ;  
/* Returns a void pointer to the element at index 'index'*/
void* BUFgetptr_at(buf_ucount_t index); 
/* When buffer becomes too large, since the buffer doubles the element capacity when it i s about to overflow, 
 * When you have done , and you have anticepted that the buffer will not increase further then you can fit the buffer and
 * there will be no free space in the buffer for new element*/
void BUFfit();
/* Fills out_value with the top element but doesn't remove it*/
void BUFpeek(void* out_value);  
/* Returns a pointer to the element which is on the top of the buffer or which is just pushed in the buffer */
void* BUFpeek_ptr(); 
/* Fills the out_value memeory block with the top element in the buffer with size of binded buffer's element size in bytes*/
void BUFpop(void* out_value); 
/* Pushes the in_value with size of bined buffer's element size in bytes on the top of the buffer 
 * When the buffer is about to overflow then it doubles it element capacity*/
void BUFpush(void* in_value); 
/* Returns true when successfully found the element and removed , otherwise returns false*/
bool BUFremove(void* object, bool (*comparer)(void*, void*));
/*Returns true when successfully found the element and removed otherwise returns false, 
 *NOTE  : It only sets the block equal to zero, it doesn't shift the elements to right as BUFremove does.
 */
bool BUFremove_noshift(void* object, bool (*comparer)(void*, void*)); 
/* Fills all the memory blocks having size of binded buffer 's element size in bytes with clear_value */
void BUFclear(void* clear_value) ; 
/* Removes an element with size binded buffer's element size, (actually it sets each byte in the memory block referenced by index to zero)
 * out_value : filled with the removed value
 * index  : the index at which it is requested to remove the element
 * NOTE : There will be no shifting of elements , that means  you can reinsert some value again in that slot without any furthur shifting of elements
 *        Use this if you now that you are removing an element at index 'index' and you would reinsert an element at that index 'index' again.
 * Important note : There will be no effect on element count in the bined buffer
 */ 
#if BUFFER_LIB_VERSION >= 0x0015
bool
#else
void
#endif
BUFremove_at_noshift(buf_ucount_t index , void* out_value) ;  
/* Removes an element with size binded buffers' e lemetn size , (actually it sets each byte int the top most memory block referenced by index to zero)
 * and shifts all the elements  succeeding the block referenced by index to left by one memory block
 * index : at the element to be removed
 * out_value : Filled with the removed value
 */
#if BUFFER_LIB_VERSION >= 0x0015
bool
#else
void
#endif
BUFremove_at(buf_ucount_t index , void* out_value) ;  
/* Inserts an element of size binded buffer's element size at the index 'index'
 * index : at which the value must be inserted
 * in_vaue : pointer to the value
 * NOTE : index must be less than element_count
   And it will alloc extra memory of size element_size if the there will be no space left in the buffer
   And it will shift all the elements beyound the index to right by one element
 */
void BUFinsert_at(buf_ucount_t index , void* in_value) ;  
/* Inserts an element of size binded buffer's element size at the index 'index'
 * index : at which the value must be inserted
 * in_value : pointer to the value
 * out_value : filled with the existing element at index 'index'
 * NOTE : index must be less than element_count //before version 1.4 it was element_size, but it is not replaced with element_count in version 1.5
   And actually it simply replaces the value at index with the value in_value
 */
void BUFinsert_at_noalloc(buf_ucount_t index , void* in_value , void* out_value) ; 

/* value : the pointer to the value , of which we want to retrieve the index
 * comparer : pointer to the function that can comparer that type of elements
 * for example : 
 * if you want to compare two integers then the comparer function would be like this : 
 *  bool comparere(void* int1, void* int2)
 *  {  
 *     return (int*)int1 == (int*)int2;
 *  }
 * 
 * This function returns BUF_INVALID_INDEX if the value is not found otherwise returns a valid non-negative  value.
 */
#if BUFFER_LIB_VERSION >= 0x0014
	//returns valid index if index is found otherwise BUF_INVALID_INDEX
	buf_ucount_t BUFfind_index_of(void* value, bool (*comparer)(void*, void*));
#elif BUFFER_LIB_VERSION >= 0x0013
	buf_count_t BUFfind_index_of(void* value, bool (*comparer)(void*, void*)); 
#else
	buf_ucount_t BUFfind_index_of(void* value, bool (*comparer)(void*, void*)); 
#endif
/* Returns the max count of elements that the buffer can contains, otherwise BUF_INVALID_INDEX*/
buf_ucount_t BUFget_capacity();  
/* Returns the Current element count in the buffer, otherwise BUF_INVALID_INDEX*/
buf_ucount_t BUFget_element_count(); 
/* Returns the element size of the elements residing in the buffer, otherwise BUF_INVALID_INDEX*/
buf_ucount_t BUFget_element_size(); 
/* Returns the unsigned char pointer to the buffer*/
/*uint8_t* is changed to void**/
void* BUFget_ptr(); 

/* Sets the max count of elements that the buffer can contain*/
void BUFset_capacity(buf_ucount_t capacity);  
/* Sets the Current element count in the buffer*/
void BUFset_element_count(buf_ucount_t element_count); 
/* Sets the element size of the elements residing in the buffer*/
void BUFset_element_size(buf_ucount_t element_size); 
/* Sets the unsigned char pointer to the buffer*/
void BUFset_ptr(uint8_t* ptr); 

/*Sets the buffer as a Auto Managed Buffer, 
 *
 * Meaning is that, When you remove an element by calling BUFremove_at_noshift() or BUFremove_noshift(),
 * then the removed block will get registered automatically to the second buffer of pointers
 * and when you call BUFpush(), then this function first look up whether the buffer is auto managed or not, if it is auto managed then 
 * this function will check for whether the buffer of pointers has some elements or not and finally if it has then this function
 * will pop the pointer to the memory block and the writes to it.
 *
 */
void BUFset_auto_managed(bool value); 

/* Added in version 1.5
 *Returns whether the binded buffer is auto managed or not
 */
bool BUFis_auto_managed();

BUFFER* BUFget_binded_buffer();

/* Added in version 1.5
 * Clones the current binded buffer and returns
 * 
 */
BUFFER* BUFget_clone();

/*Returns the offset*/
buf_ucount_t BUFget_offset();
/*Sets the offset*/
void BUFset_offset(buf_ucount_t offset);

/*Resizes the buffer to new_capacity; 
 *NOTE: Here new_capacity means the new number of elements capacity of size element_size
 */
void BUFresize(buf_ucount_t new_capacity);

/*Returns the buffer size in bytes including the offset*/ 
buf_ucount_t BUFget_buffer_size();

/*Clears the entire buffer with value *clear_value, or if it is NULL then with zero*/
void BUFclear_buffer(void* clear_value);

#if BUFFER_LIB_VERSION >= 0x0015
/*Traverses each elements by calling func and passing element pointer and args pointer to the func*/
void BUFtraverse_elements(buf_ucount_t start, buf_ucount_t end, void (*func)(void* /*element ptr*/, void* /*args ptr*/), void* args);

void BUFset_offset_bytes(uint8_t* offset_bytes);
void* BUFget_offset_bytes();

#endif

#if BUFFER_LIB_VERSION >= 0x0016

void BUFcopy_to(BUFFER* destination);
void BUFmove_to(BUFFER* destination);
BUFFER* BUFcopy_construct(BUFFER* source);
void BUFset_on_free(void (*free)(void*));

#endif
#endif


/*End: BUFFER*/

#ifdef __cplusplus
}
#endif

#endif /*_BUFFER_H_*/