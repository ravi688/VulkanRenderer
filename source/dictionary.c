
#include <dictionary.h>
#include <memory.h>
#include <renderer/assert.h>

#ifndef GLOBAL_DEBUG
#	define check_pre_condition(dictionary)
#else 
	static void check_pre_condition(dictionary_t* dictionary);
#endif /*GLOBAL_DEBUG*/

// constructors and destructors
RENDERER_API function_signature(dictionary_t, __dictionary_create, u32 key_size, u32 value_size, u64 capacity, bool (*key_comparer)(void* compare_key, void* key))
{
	CALLTRACE_BEGIN();
	assert(key_comparer != NULL);
	assert_wrn((key_size + value_size) < 1024);
	dictionary_t dictionary;
	dictionary.buffer = buf_create(key_size + value_size, capacity, 0);
	dictionary.key_comparer = key_comparer;
	dictionary.key_size = key_size;
	dictionary.value_size = value_size;
	CALLTRACE_RETURN(dictionary);
}

RENDERER_API function_signature(void, dictionary_free, dictionary_t* dictionary)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	buf_free(&dictionary->buffer);
	memset(dictionary, 0, sizeof(dictionary_t));
	CALLTRACE_END();
}

// getters
RENDERER_API function_signature(void, dictionary_get_at, dictionary_t* dictionary, u64 index, void* out_key, void* out_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcpy(out_key, ptr, dictionary->key_size);
	memcpy(out_value, ptr + dictionary->key_size, dictionary->value_size);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, dictionary_get_value, dictionary_t* dictionary, void* key, void* out_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	void* ptr = buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer));
	memcpy(out_value, ptr + dictionary->key_size, dictionary->value_size);
	CALLTRACE_END();
}

RENDERER_API function_signature(bool, dictionary_try_get_value, dictionary_t* dictionary, void* key, void* out_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	buf_ucount_t index = buf_find_index_of(buffer, key, dictionary->key_comparer);
	if(index == BUF_INVALID_INDEX)
		CALLTRACE_RETURN(false);
	void* ptr = buf_get_ptr_at(buffer, index);
	memcpy(out_value, ptr + dictionary->key_size, dictionary->value_size);
	CALLTRACE_RETURN(true);	
}

RENDERER_API function_signature(void, dictionary_get_key_at, dictionary_t* dictionary, u64 index, void* out_key)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	memcpy(out_key, buf_get_ptr_at(&dictionary->buffer, index), dictionary->key_size);
	CALLTRACE_END();	
}

RENDERER_API function_signature(void*, dictionary_get_key_ptr_at, dictionary_t* dictionary, u64 index)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_get_ptr_at(&dictionary->buffer, index));
}

RENDERER_API function_signature(void, dictionary_get_value_at, dictionary_t* dictionary, u64 index, void* out_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	memcpy(out_value, buf_get_ptr_at(&dictionary->buffer, index) + dictionary->key_size, dictionary->value_size);
	CALLTRACE_END();
}

RENDERER_API function_signature(void*, dictionary_get_value_ptr_at, dictionary_t* dictionary, u64 index)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_get_ptr_at(&dictionary->buffer, index) + dictionary->key_size);
}

RENDERER_API function_signature(void*, dictionary_get_value_ptr, dictionary_t* dictionary, void* key)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	CALLTRACE_RETURN(buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer)) + dictionary->key_size);
}

RENDERER_API function_signature(u64, dictionary_get_count, dictionary_t* dictionary)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_get_element_count(&dictionary->buffer));
}

RENDERER_API function_signature(u64, dictionary_get_capacity, dictionary_t* dictionary)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_get_capacity(&dictionary->buffer));
}


// setters
RENDERER_API function_signature(void, dictionary_set_at, dictionary_t* dictionary, u64 index, void* in_key, void* in_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcpy(ptr, in_key, dictionary->key_size);
	memcpy(ptr + dictionary->key_size, in_value, dictionary->value_size);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, dictionary_set_value, dictionary_t* dictionary, void* key, void* in_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	BUFFER* buffer = &dictionary->buffer;
	void* ptr = buf_get_ptr_at(buffer, buf_find_index_of(buffer, key, dictionary->key_comparer));
	memcpy(ptr + dictionary->key_size, in_value, dictionary->value_size);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, dictionary_set_value_at, dictionary_t* dictionary, u64 index, void* in_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	void* ptr = buf_get_ptr_at(&dictionary->buffer, index);
	memcpy(ptr + dictionary->key_size, in_value, dictionary->value_size);
	CALLTRACE_END();
}

// logic functions
RENDERER_API function_signature(void, dictionary_push, dictionary_t* dictionary, void* in_key, void* in_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	// NOTE: buf_push internally uses memcpy, so here we are calling memcpy two times more
	//		 We could have used buf_push_pseudo(buffer, 1) but it internally uses memset(bytes, 0, size) which zeros out the memory
	// TODO: So, create an another version of buf_push_pseudo leaving the memory uninitialized
	u8 bytes[dictionary->key_size + dictionary->value_size];
	memcpy(bytes, in_key, dictionary->key_size);
	memcpy(bytes + dictionary->key_size, in_value, dictionary->value_size);
	buf_push(&dictionary->buffer, bytes);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, dictionary_pop, dictionary_t* dictionary, void* out_key, void* out_value)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	void* ptr = buf_peek_ptr(&dictionary->buffer);
	memcpy(out_key, ptr, dictionary->key_size);
	memcpy(out_value, ptr + dictionary->key_size, dictionary->value_size);
	buf_pop(&dictionary->buffer, NULL);
	CALLTRACE_END();
}

RENDERER_API function_signature(void, dictionary_remove, dictionary_t* dictionary, void* key)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	bool result = buf_remove(&dictionary->buffer, key, dictionary->key_comparer);
	assert(result == true);
	CALLTRACE_END();
}

RENDERER_API function_signature(bool, dictionary_contains, dictionary_t* dictionary, void* key)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_find_index_of(&dictionary->buffer, key, dictionary->key_comparer) != BUF_INVALID_INDEX);
}

RENDERER_API function_signature(void, dictionary_clear, dictionary_t* dictionary)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	buf_clear(&dictionary->buffer, NULL);
	CALLTRACE_END();
}

RENDERER_API function_signature(u64, dictionary_find_index_of, dictionary_t* dictionary, void* key)
{
	CALLTRACE_BEGIN();
	check_pre_condition(dictionary);
	CALLTRACE_RETURN(buf_find_index_of(&dictionary->buffer, key, dictionary->key_comparer));
}

RENDERER_API bool dictionary_key_comparer_u16(void* v1, void* v2)
{
	return (*(u16*)v1) == (*(u16*)v2);
}

RENDERER_API bool dictionary_key_comparer_u32(void* v1, void* v2)
{
	return (*(u32*)v1) == (*(u32*)v2);
}

RENDERER_API bool dictionary_key_comparer_u64(void* v1, void* v2)
{
	return (*(u64*)v1) == (*(u64*)v2);
}

RENDERER_API bool dictionary_key_comparer_u8(void* v1, void* v2)
{
	return (*(u8*)v1) == (*(u8*)v2);
}

RENDERER_API bool dictionary_key_comparer_s16(void* v1, void* v2)
{
	return (*(s16*)v1) == (*(s16*)v2);
}

RENDERER_API bool dictionary_key_comparer_s32(void* v1, void* v2)
{
	return (*(s32*)v1) == (*(s32*)v2);
}

RENDERER_API bool dictionary_key_comparer_s64(void* v1, void* v2)
{
	return (*(s64*)v1) == (*(s64*)v2);
}

RENDERER_API bool dictionary_key_comparer_s8(void* v1, void* v2)
{
	return (*(s8*)v1) == (*(s8*)v2);
}

RENDERER_API bool dictionary_key_comparer_float(void* v1, void* v2)
{
	return (*(float*)v1) == (*(float*)v2);
}

RENDERER_API bool dictionary_key_comparer_double(void* v1, void* v2)
{
	return (*(double*)v1) == (*(double*)v2);
}

RENDERER_API bool dictionary_key_comparer_string(void* v1, void* v2)
{
	return (*(char**)v1) == (*(char**)v2);
}



#ifdef GLOBAL_DEBUG
static void check_pre_condition(dictionary_t* dictionary)
{
	assert(dictionary != NULL);
	assert(dictionary->key_comparer != 0);
	assert_wrn(dictionary->key_size != 0);
	assert_wrn(dictionary->value_size != 0);
	assert_wrn((dictionary->key_size + dictionary->value_size) < 1024);
}
#endif /*GLOBAL_DEBUG*/
