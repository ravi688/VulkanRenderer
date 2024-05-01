#pragma once

#include <renderer/defines.h>			// stdbool.h
#include <renderer/alloc.h>				// memset
#include <renderer/assert.h> 			// _debug_assert__

typedef enum vulkan_object_nationality_t
{
	VULKAN_OBJECT_NATIONALITY_UNDEFINED = 0,
	VULKAN_OBJECT_NATIONALITY_EXTERNAL,
	VULKAN_OBJECT_NATIONALITY_INTERNAL,
	VULKAN_OBJECT_NATIONALITY_MAX
} vulkan_object_nationality_t;

typedef enum vulkan_object_type_t
{
	VULKAN_OBJECT_TYPE_UNDEFINED = 0,
	VULKAN_OBJECT_TYPE_BUFFER,
	VULKAN_OBJECT_TYPE_SHADER_MODULE,
	VULKAN_OBJECT_TYPE_INSTANCE_BUFFER,
	VULKAN_OBJECT_TYPE_HOST_BUFFERED_BUFFER,
	VULKAN_OBJECT_TYPE_MAX
} vulkan_object_type_t;

typedef struct vulkan_object_t
{
	vulkan_object_nationality_t nationality;

	/* type information need not to be retained in release mode */
	IF_DEBUG( vulkan_object_type_t type; )
} vulkan_object_t;

#define VULKAN_OBJECT(typed_ptr) REINTERPRET_CAST(vulkan_object_t*, typed_ptr)
#define VULKAN_OBJECT_CONST(typed_ptr) REINTERPRET_CONST_CAST(vulkan_object_t*, typed_ptr)
#define VULKAN_OBJECT_VOID(void_ptr) CAST_TO(vulkan_object_t*, void_ptr)
#define VULKAN_OBJECT_CONST_VOID(void_ptr) CAST_TO(const vulkan_object_t*, void_ptr)

#define __VULKAN_OBJECT__ vulkan_object_t __vo##__LINE__
#define VULKAN_OBJECT_INIT(typed_ptr, type, nationality) vulkan_object_init(VULKAN_OBJECT(typed_ptr), type, nationality)
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_init(vulkan_object_t* obj, vulkan_object_type_t type, bool nationality)
{
	IF_DEBUG(obj->type = type);
	obj->nationality = nationality;
}
#define VULKAN_OBJECT_MEMZERO(typed_ptr, type) vulkan_object_memzero(CAST_TO(void*, VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr)), sizeof(typed_ptr[0]))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_memzero(void* ptr, u32 size)
{
	_debug_assert__((ptr != NULL) && (size != 0));
	_debug_assert__(size >= sizeof(vulkan_object_t));
	memset(ptr + sizeof(vulkan_object_t), 0, size - sizeof(vulkan_object_t));
}




/* nationality (memory ownership) */
#define VULKAN_OBJECT_IS_INTERNAL(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_INTERNAL)
#define VULKAN_OBJECT_IS_EXTERNAL(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_EXTERNAL)
#define VULKAN_OBJECT_IS_UNDEFINED(typed_ptr) (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) == VULKAN_OBJECT_NATIONALITY_UNDEFINED)
#define VULKAN_OBJECT_IS_NATIONALITY_GARBAGE(typed_ptr) (VULKAN_OBJECT_IS_UNDEFINED(typed_ptr) || (VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) >= VULKAN_OBJECT_NATIONALITY_MAX))

/* type of the object */
#define VULKAN_OBJECT_IS_TYPE_UNDEFINED(typed_ptr) (VULKAN_OBJECT_GET_TYPE(typed_ptr) == VULKAN_OBJECT_TYPE_UNDEFINED)
#define VULKAN_OBJECT_IS_TYPE_GARBAGE(typed_ptr) (VULKAN_OBJECT_IS_TYPE_UNDEFINED(typed_ptr) || (VULKAN_OBJECT_GET_TYPE(typed_ptr) >= VULKAN_OBJECT_TYPE_MAX))

#define VULKAN_OBJECT_SET_NATIONALITY(typed_ptr, nationality) vulkan_object_set_nationality(VULKAN_OBJECT(typed_ptr), nationality)
#define VULKAN_OBJECT_GET_NATIONALITY(typed_ptr) vulkan_object_get_nationality(VULKAN_OBJECT_CONST(typed_ptr))
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_object_nationality_t vulkan_object_get_nationality(const vulkan_object_t* obj) { return obj->nationality; }
static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_set_nationality(vulkan_object_t* obj, bool nationality) { obj->nationality = nationality; }

/* usage of VULKAN_OBJECT_SET_TYPE Or VULKAN_OBJECT_GET_TYPE is not permitted in release mode */
#ifdef GLOBAL_DEBUG
#	define VULKAN_OBJECT_SET_TYPE(typed_ptr, type) vulkan_object_set_type(VULKAN_OBJECT(typed_ptr), type)
#	define VULKAN_OBJECT_GET_TYPE(typed_ptr) vulkan_object_get_type(VULKAN_OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE vulkan_object_type_t vulkan_object_get_type(const vulkan_object_t* obj) { return obj->type; }
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_set_type(vulkan_object_t* obj, vulkan_object_type_t type) { obj->type = type; }
#endif /* GLOBAL_DEBUG */

#ifdef GLOBAL_DEBUG
#	define VULKAN_OBJECT_TYPE_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CAST(dst_ptr_type, VULKAN_OBJECT_TYPE_CHECK_FORWARD(src_typed_ptr, vulkan_object_type))
#	define VULKAN_OBJECT_TYPE_CONST_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CONST_CAST(dst_ptr_type, VULKAN_OBJECT_TYPE_CHECK_CONST_FORWARD(src_typed_ptr, vulkan_object_type))
#	define VULKAN_OBJECT_TYPE_CHECK(type_ptr, vulkan_object_type) vulkan_object_type_check(type_ptr, vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_type_check(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		debug_assert__(obj->type == type, "Vulkan Object type mismatched");
	}
#	define VULKAN_OBJECT_TYPE_CHECK_FORWARD(typed_ptr, vulkan_object_type) vulkan_object_type_check_forward(VULKAN_OBJECT_CONST(typed_ptr), vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void* vulkan_object_type_check_forward(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		vulkan_object_type_check(obj, type);
		return CAST_TO(void*, obj);
	}
#	define VULKAN_OBJECT_TYPE_CHECK_CONST_FORWARD(typed_ptr, vulkan_object_type) vulkan_object_type_check_const_forward(VULKAN_OBJECT_CONST(typed_ptr), vulkan_object_type)
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* vulkan_object_type_check_const_forward(const vulkan_object_t* obj, vulkan_object_type_t type)
	{
		vulkan_object_type_check(obj, type);
		return CAST_TO(const void*, obj);
	}
#else /* GLOBAL_DEBUG */
#	define VULKAN_OBJECT_TYPE_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CAST(dst_ptr_type, src_typed_ptr)
#	define VULKAN_OBJECT_TYPE_CONST_CAST(dst_ptr_type, vulkan_object_type, src_typed_ptr) REINTERPRET_CONST_CAST(dst_ptr_type, src_typed_ptr)
#endif /* GLOBAL_RELEASE */

/* no verification of vulkan objects can be done in release mode */
#ifdef GLOBAL_DEBUG
#	define VULKAN_OBJECT_VERIFY(typed_ptr) vulkan_object_verify(VULKAN_OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE void vulkan_object_verify(const vulkan_object_t* obj)
	{
		debug_assert__(!VULKAN_OBJECT_IS_TYPE_GARBAGE(obj), "Vulkan Object Type is Garbage");
		debug_assert__(!VULKAN_OBJECT_IS_NATIONALITY_GARBAGE(obj), "Vulkan Object Nationality is Garbage");
	}
#	define VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr) vulkan_object_verify_forward(VULKAN_OBJECT_CONST(typed_ptr))
	static CAN_BE_UNUSED_FUNCTION INLINE_IF_RELEASE_MODE const void* vulkan_object_verify_forward(const vulkan_object_t* obj)
	{
		vulkan_object_verify(obj);
		return CAST_TO(const void*, obj);
	}
#else
#	define VULKAN_OBJECT_VERIFY(typed_ptr)
#	define VULKAN_OBJECT_VERIFY_FORWARD(typed_ptr) typed_ptr
#endif
