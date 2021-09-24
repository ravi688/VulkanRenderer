
#include "buffer_test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LOG(log_string) printf("[Log] %s, at %s, line: %lu\n", log_string, __FUNCTION__, __LINE__)
#if defined(ASSERT)
#	undef ASSERT 
#endif
#define ASSERT(boolean, return_value, ...) do { if(!(boolean)) { printf("[Error] Assertion Failed at %s, line: %d : %s\n", __FUNCTION__, __LINE__, __VA_ARGS__); return return_value;  } } while(false)
#define VOID
#define TESTED
#define NOT_TESTED

#define LOG_TEST(test_no, test_name) printf("Test: %s -> ", test_name)
#define LOG_RESULT(result_0_or_1) printf("%s\n", (result_0_or_1) ? "Passed" : "Failed")
#define GET_AT(index, stride, ptr) ((void*)(ptr) + (stride) * (index))

static void print_char(void* value, void* args) { printf("%c ", *(char*)value); }
static void print_int(void* value, void* args) { printf("%d ", *(int*)value); }

TESTED
BUFFER* test_BUFcreate(BUFFER* __buffer, buf_ucount_t element_size, buf_ucount_t capacity, buf_ucount_t offset, bool *out_result)
{
	BUFpush_binded();
	BUFFER* buffer = BUFcreate(__buffer, element_size, capacity, offset);
	*out_result = buffer != NULL;
	ASSERT(buffer != NULL, NULL, "BUFFER object creation failed in Heap");
	BUFbind(buffer);
	if((!BUFget_element_count() == 0))
		*out_result = false;
	ASSERT(BUFget_element_count() == 0, NULL, "BUFget_element_count() != 0 when creating the buffer in Heap");
	
	if(!(BUFget_element_size() == element_size))
		*out_result = false;
	ASSERT(BUFget_element_size() == element_size, NULL, "BUFget_element_size() != element_size when creating the buffer in Heap");
	if(!(BUFget_offset() == offset))
		*out_result = false;
	ASSERT(BUFget_offset() == offset, NULL, "BUFget_offset() != offset when creating the buffer in Heap");
	if(!(BUFget_capacity() == capacity))
		*out_result = false;
	ASSERT(BUFget_capacity() == capacity, NULL, "BUFget_capacity() != capacity when creating the buffer in Heap");
	BUFpop_binded();
	return buffer;
}

TESTED
BUFFER* test_BUFcreate_object(uint8_t* bytes, bool* out_result)
{
	BUFFER* buffer = BUFcreate_object(bytes);
	if(buffer == NULL)
		*out_result = false;
	ASSERT(buffer != NULL, NULL, "BUFFER object creation failed with bytes");
	return buffer;
}

TESTED
void test_BUFget_at(buf_ucount_t index , void* out_value, bool (*compare)(void*, void*), bool* out_result)
{
	void* temp = malloc(BUFget_element_size());
	BUFget_at(index, temp);
	BUFpush(temp); 
	BUFget_at(BUFget_element_count() - 1, out_value);
	BUFpop(NULL);
	if(!compare(temp, out_value))
	{
		free(temp);
		*out_result = false;
		ASSERT(false, VOID, "Either of BUFget_element_size, BUFget_at or BUFpush doesn't work correctly");
	}
}
TESTED
void test_BUFset_at(buf_ucount_t index , void* in_value, bool (*compare)(void*, void*), bool* out_result)
{
	BUFset_at(index, in_value); 
	void* out_value = malloc(BUFget_element_size());
	BUFget_at(index, out_value);
	if(!compare(out_value, in_value))
		*out_result = false;
	ASSERT(compare(out_value, in_value), VOID, "BUFset_at doesn't sets correct value, when checked with BUFget_at after BUFset_at");
	free(out_value);
} 
TESTED
void* test_BUFgetptr_at(buf_ucount_t index, bool (*compare)(void*, void*), bool* out_result)
{
	void* ptr = BUFgetptr_at(index);
	buf_ucount_t __index = BUFfind_index_of(ptr, compare); 
	*out_result= __index == index;
	ASSERT(*out_result, ptr, "Either of BUFfind_index_of or BUFgetptr_at doesn't work correctly");
	BUFpush(ptr);
	void* temp = malloc(BUFget_element_size());
	BUFpop(temp); 

	if(!compare(temp, ptr))
	{
		free(temp); 
		*out_result = false;
		ASSERT(false, BUFgetptr_at(index), "Either of BUFpush, BUFpop, BUFget_element_size, or BUFget_ptr_at doesn't work correctly");
	}
	
	free(temp);
}
TESTED
void test_BUFfit(bool* out_result)
{
	BUFfit(); 
	if(!(BUFget_element_count() == BUFget_capacity()))
		*out_result = false;
	ASSERT(BUFget_element_count() == BUFget_capacity(), VOID, "BUFfit doesn't fits the buffer correctly because BUFget_element_count() != BUFget_capacity()");
}

TESTED
buf_ucount_t test_BUFget_element_count(bool* out_result)
{
	buf_ucount_t value = BUFget_element_count();
	BUFset_element_count(23); 
	if(BUFget_element_count() != 23)
	{
		BUFset_element_count(value);
		*out_result = false;
		ASSERT(false, value, "Either of BUFget_element_count or BUFset_element_count doesn't working correctly");
	}
	BUFset_element_count(value);
	return value;
}

TESTED
buf_ucount_t test_BUFget_element_size(bool* out_result)
{
	buf_ucount_t value = BUFget_element_size();
	BUFset_element_size(193); 
	if(BUFget_element_size() != 193)
	{
		BUFset_element_size(value);
		*out_result = false;
		ASSERT(false, value, "Either of BUFget_element_size or BUFset_element_size doesn't working correctly");
	}
	BUFset_element_size(value);
	return value;
}

TESTED
buf_ucount_t test_BUFget_capacity(bool* out_result)
{
	buf_ucount_t value = BUFget_capacity();
	BUFset_capacity(193); 
	if(BUFget_capacity() != 193)
	{
		BUFset_capacity(value);
		*out_result = false;
		ASSERT(false, value, "Either of BUFget_capacity or BUFset_capacity doesn't working correctly");
	}
	BUFset_capacity(value);
	return value;
}

TESTED
buf_ucount_t test_BUFget_offset(bool* out_result)
{
	buf_ucount_t offset = BUFget_offset();
	BUFset_offset(10); 
	if(BUFget_offset() != 10)
	{
		BUFset_offset(offset);
		*out_result = false;
		ASSERT(false, offset, "Either of BUFget_offset or BUFset_offset doesn't working correctly");
	}
	BUFset_offset(offset);
	return offset;
}

TESTED
void test_BUFset_capacity(buf_ucount_t capacity, bool* out_result) 
{
	BUFset_capacity(capacity);
	if(!(capacity == BUFget_capacity()))
		*out_result = false;
	ASSERT(capacity == BUFget_capacity(), VOID, "Either of BUFset_capacity or BUFget_capacity doesn't work correctly");
}

TESTED
void test_BUFset_element_count(buf_ucount_t element_count, bool* out_result)
{
	BUFset_element_count(element_count);
	if(!(element_count == BUFget_element_count()))
		*out_result = false;
	ASSERT(element_count == BUFget_element_count(), VOID, "Either of BUFset_element_count or BUFget_element_count doesn't work correctly");
}

TESTED
void test_BUFset_element_size(buf_ucount_t element_size, bool* out_result)
{
	BUFset_element_size(element_size);
	if(!(element_size == BUFget_element_size()))
		*out_result = false;
	ASSERT(element_size == BUFget_element_size(), VOID, "Either of BUFset_element_size or BUFget_element_size doesn't work correctly");	
}

TESTED
void test_BUFset_ptr(uint8_t* ptr, bool* out_result)
{
	BUFset_ptr(ptr);
	if(!(ptr == BUFget_ptr()))
		*out_result = false;
	ASSERT(ptr == BUFget_ptr(), VOID, "Either of BUFset_ptr or BUFget_ptr doesn't work correctly");		
}

TESTED
void test_BUFset_offset(buf_ucount_t offset, bool* out_result)
{
	BUFset_offset(offset);
	if(!(offset == BUFget_offset()))
		*out_result = false;
	ASSERT(offset == BUFget_offset(), VOID, "Either of BUFset_offset or BUFget_offset doesn't work correctly");		
}

NOT_TESTED
void* test_BUFget_offset_bytes(bool* out_result)
{
	if(BUFget_offset() == 0)
		return NULL;
	void* bytes = BUFget_offset_bytes(); 
	void* temp = malloc(BUFget_offset()); 
	memcpy(temp, bytes, BUFget_offset()); 
	bytes = malloc(BUFget_offset()); 
	memset(bytes, 10, BUFget_offset()); 
	BUFset_offset_bytes(bytes); 
	void* bytes2 = BUFget_offset_bytes(); 
	bool test_passed = !memcmp(bytes, bytes2, BUFget_offset()); 
	BUFset_offset_bytes(bytes);
	free(bytes); 
	free(temp);
	if(!test_passed)
		*out_result = false;
	ASSERT(test_passed, BUFget_offset_bytes(), "Either of BUFget_offset_bytes, BUFget_offset, or BUFset_offset_bytes");
	return BUFget_offset_bytes();
}

NOT_TESTED
void test_BUFset_offset_bytes(uint8_t* bytes, bool* out_result)
{
	BUFset_offset_bytes(bytes);
	void* _bytes = BUFget_offset_bytes(); 
	bool result = (memcmp(_bytes, bytes, BUFget_offset()) == 0);
	if(!result)
		*out_result = false;
	ASSERT(result == true, VOID, "Either BUFset_offset_bytes, BUFget_offset_bytes, or BUFget_offset doesn't work correctly");
}

TESTED
void test_BUFbind(BUFFER* buffer, bool* out_result)
{
	BUFbind(buffer); 
	if(!(BUFget_binded() == buffer))
		*out_result = false;
	ASSERT(BUFget_binded() == buffer, VOID, "Either of BUFbind or BUFget_binded doesn't work correctly"); 
}

TESTED
uint8_t* test_BUFget_ptr(bool (*compare)(void*, void*), bool* out_result)
{
	uint8_t* bytes = BUFget_ptr();
	if(BUFget_element_count() > 0)
	{
		if(!compare(bytes, BUFgetptr_at(0)))
			*out_result = false;
		ASSERT(compare(bytes, BUFgetptr_at(0)), NULL, "BUFget_ptr() doesn't points to the actual data buffer");
	}
	return bytes;
}

TESTED
void test_BUFpeek(void* out_value, bool (*compare)(void*, void*), bool* out_result)
{
	BUFpush(out_value);
	void* temp = malloc(BUFget_element_size());
	BUFpeek(temp);
	if(!compare(temp, out_value))
		*out_result = false;
	ASSERT(compare(temp, out_value), VOID, "Either of BUFpeek, BUFget_element_size, or BUFpush doesn't work correctly");
	BUFpop(out_value);
	if(!compare(temp, out_value))
		*out_result = false;
	ASSERT(compare(temp, out_value), VOID, "Either of BUFpeek, BUFget_element_size, BUFpush or BUFpop doesn't work correctly");
	BUFpeek(out_value);
	free(temp);
} 

TESTED
void* test_BUFpeek_ptr(bool (*comparer)(void*, void*), bool* out_result)
{
	void* ptr1 = BUFpeek_ptr();
	BUFpush(ptr1);
	void* ptr2 = BUFpeek_ptr(); 
	if(!comparer(ptr1, ptr2))
	{
		*out_result = false;
		BUFpop(NULL);	
		ASSERT(false, BUFpeek_ptr(), "Either of BUFpush or BUFpeek_ptr doesn't work correctly");
	}
	ptr2 = malloc(BUFget_element_size());
	BUFpop(ptr2);
	if(!comparer(ptr1, ptr2))
	{
		free(ptr2);
		*out_result = false; 
		ASSERT(false, BUFpeek_ptr(), "Either of BUFpush, BUFpop, BUFget_element_size or BUFpeek_ptr doesn't work correctly");
	}
	free(ptr2);
	return BUFpeek_ptr();
} 

TESTED
void test_BUFpop(void* out_value, bool (*compare)(void*, void*), bool* out_result)
{
	if(out_value != NULL)
	{
		BUFpush(out_value);
		void* ptr1 = malloc(BUFget_element_size()); 
		BUFpop(ptr1); 
		if(!compare(ptr1, out_value))
		{
			free(ptr1);
			*out_result = false;
			ASSERT(false, VOID, "Either of BUFpop, BUFpush or BUFget_element_size doesn't working correctly");
		}
		free(ptr1);
	}
	BUFpop(out_value);
}

TESTED
void test_BUFpush(void* in_value, bool (*comparer)(void*, void*), bool* out_result)
{
	BUFpush(in_value); 
	void* temp = malloc(BUFget_element_size()); 
	BUFpeek(temp); 
	if(!comparer(temp, in_value))
		*out_result = false;
	ASSERT(comparer(temp, in_value), VOID, "either of BUFpeek or BUFpush doesn't work correctly");
}

TESTED
bool test_BUFremove(void* object, bool (*comparer)(void*, void*), bool *out_result)
{
	if(!(BUFfind_index_of(object, comparer) != BUF_INVALID_INDEX))
		*out_result = false;
	ASSERT(BUFfind_index_of(object, comparer) != BUF_INVALID_INDEX, false, "Either the value doesn't exists in the buffer or BUFfind_index_of doesn't works correctly");
	bool result = BUFremove(object, comparer); 
	if(!(BUFfind_index_of(object, comparer) == BUF_INVALID_INDEX))
		*out_result = false;
	ASSERT(BUFfind_index_of(object, comparer) == BUF_INVALID_INDEX, false, "Either of BUFremove and BUFfind_index_of don't work correctly");
	return result;
}

NOT_TESTED
bool test_BUFremove_noshift(void* object, bool (*comparer)(void*, void*), bool* out_result)
{
	if(!(BUFfind_index_of(object, comparer) != BUF_INVALID_INDEX))
		*out_result = false; 
	ASSERT(BUFfind_index_of(object, comparer) != BUF_INVALID_INDEX, false, "Either the value doesn't exists in the buffer or BUFfind_index_of doesn't works correctly");
	bool result = BUFremove_noshift(object, comparer);
	if(!(BUFfind_index_of(object, comparer) == BUF_INVALID_INDEX))
		*out_result = false;
	ASSERT(BUFfind_index_of(object, comparer) == BUF_INVALID_INDEX, false, "Either of BUFremove_noshift and BUFfind_index_of don't work correctly");
	return result;
}

TESTED
buf_ucount_t test_BUFget_buffer_size()
{
	return BUFget_buffer_size();
}

TESTED
void test_BUFclear_buffer(void* clear_value, bool (*comparer)(void*, void*), bool *out_result)
{
	buf_ucount_t capacity = BUFget_capacity(); 
	uint8_t* bytes = BUFget_ptr();
	bool result = true;
	BUFclear_buffer(clear_value); 
	if(clear_value != NULL)
	for(buf_ucount_t i = 0; i < capacity; i++, bytes += BUFget_element_size())
			if(comparer(bytes, clear_value) == false)
				result = false;
	if(!result)
		*out_result = false;
	ASSERT(result == true, VOID, "BUFclear_buffer doesn't working correctly");	
}

TESTED
void test_BUFclear(void* clear_value, bool (*comparer)(void*, void*), bool* out_result)
{
	buf_ucount_t element_count = BUFget_element_count(); 
	uint8_t* bytes = BUFget_ptr();
	bool result = true;
	BUFclear(clear_value); 
	if(clear_value != NULL)
	for(buf_ucount_t i = 0; i < element_count; i++, bytes += BUFget_element_size())
			if(comparer(bytes, clear_value) == false)
				result = false;
	if(!result)
		*out_result = false;
	ASSERT(result, VOID, "BUFclear doesn't working correctly");
}

NOT_TESTED
bool test_BUFremove_at_noshift(buf_ucount_t index , void* out_value, bool (*comparer)(void*, void*), bool* out_result)
{
	void* temp = malloc(BUFget_element_size());
	BUFget_at(index, temp);
	if(!(temp != NULL))
		*out_result = false;
	ASSERT(temp != NULL, false, "Either index is invalid or BUFget_at or BUFget_element_size not working correctly");
	bool result = BUFremove_at_noshift(index, out_value);
	if(!comparer(temp, out_value))
		*out_result = false;
	ASSERT(comparer(temp, out_value), result, "BUFremove_at_noshift doesn't fills the out_value with the value at index");
	free(temp);
	return result;
}

TESTED
bool test_BUFremove_at(buf_ucount_t index , void* out_value, bool (*comparer)(void*, void*), bool *out_result)
{
	void* temp = malloc(BUFget_element_size());
	BUFget_at(index, temp);
	if(!(temp != NULL))
		*out_result = false;
	ASSERT(temp != NULL, false, "Either index is invalid or BUFget_at not working correctly");
	bool result = BUFremove_at(index, out_value);
	if(!comparer(temp, out_value))
		*out_result = false;
	ASSERT(comparer(temp, out_value), result, "BUFremove_at doesn't fills the out_value with the value at index");
	free(temp);
	return result;
}

TESTED
void test_BUFinsert_at(buf_ucount_t index , void* in_value, bool (*comparer)(void*, void*), bool *out_result)
{
	BUFinsert_at(index, in_value);
	if(!comparer(BUFgetptr_at(index), in_value))
		*out_result = false;
	ASSERT(comparer(BUFgetptr_at(index), in_value), VOID, "Either BUFinsert_at or BUFgetptr_at doesn't work correctly at index");
}

NOT_TESTED
void test_BUFinsert_at_noalloc(buf_ucount_t index , void* in_value , void* out_value, bool (*comparer)(void*, void*), bool* out_result)
{
	void* temp = malloc(BUFget_element_size());
	BUFget_at(index, temp);
	BUFinsert_at_noalloc(index, in_value, out_value);

	if(out_value != NULL)
	{
		if(!comparer(temp, out_value))
			*out_result = false;
		ASSERT(comparer(temp, out_value), VOID, "Either BUFget_at or BUFinsert_at_noalloc doesn't work correctly for index");
	}
	BUFget_at(index, temp); 
	if(!comparer(temp, in_value))
		*out_result = false;
	ASSERT(comparer(temp, in_value), VOID, "Either BUFget_at or BUFinsert_at_noalloc doesn't work correctly for index");
	if(temp != NULL)
		free(temp);
}

TESTED
buf_ucount_t test_BUFfind_index_of(void* value, bool (*comparer)(void*, void*), bool* out_result)
{
	buf_ucount_t count = BUFfind_index_of(value, comparer);
	buf_ucount_t __count = count;
	if(count == BUF_INVALID_INDEX)
	{
		BUFinsert_at(0, value); 
		count = BUFfind_index_of(value, comparer); 
	}
	if(!(count != BUF_INVALID_INDEX))
		*out_result = false;
	ASSERT(count != BUF_INVALID_INDEX, __count, "Either BUFinsert_at or BUFfind_index_of doens't work correctly");
	return __count;
}

TESTED
BUFFER* test_BUFget_binded_buffer(bool* out_result)
{
	BUFFER* buffer = BUFget_binded_buffer();
	BUFpush_binded();
	BUFFER* test_buffer = BUFcreate(NULL, sizeof(int), 0, 0); 
	BUFbind(test_buffer);
	if(!(BUFget_binded_buffer() == test_buffer))
		*out_result = false;
	ASSERT(BUFget_binded_buffer() == test_buffer, buffer, "Either of BUFget_binded_buffer, BUFcreate, or BUFbind doesn't work correctly");
	BUFfree();
	BUFpop_binded();
	return buffer;
}

TESTED
BUFFER* test_BUFget_clone(bool* out_result)
{
	BUFFER* clone = BUFget_clone();
	buf_ucount_t element_count = BUFget_element_count(); 
	buf_ucount_t capacity = BUFget_capacity(); 
	buf_ucount_t element_size = BUFget_element_size(); 
	buf_ucount_t offset = BUFget_offset(); 
	buf_ucount_t buffer_size = BUFget_buffer_size();
	uint8_t* bytes = BUFget_ptr(); 
	BUFpush_binded();
	BUFbind(clone); 
	if(!(BUFget_element_count() == element_count))
		*out_result = false;
	ASSERT(BUFget_element_count() == element_count, NULL, "BUFclone doesn't working correctly, BUFget_element_count() != element_count");
	if(!(BUFget_capacity() == capacity))
		*out_result = false;
	ASSERT(BUFget_capacity() == capacity, NULL, "BUFclone doesn't working correctly, BUFget_capacity() != capacity");
	if(!(BUFget_element_size() == element_size))
		*out_result = false;
	ASSERT(BUFget_element_size() == element_size, NULL, "BUFclone doesn't working correctly, BUFget_element_size() != element_size");
	if(!(BUFget_offset() == offset))
		*out_result = false;
	ASSERT(BUFget_offset() == offset, NULL, "BUFclone doesn't working correctly, BUFget_offset() != offset");
	if(!(BUFget_buffer_size() == buffer_size))
		*out_result = false;
	ASSERT(BUFget_buffer_size() == buffer_size, NULL, "BUFclone doesn't working correctly, BUFget_buffer_size() != buffer_size");
	bool result  = !memcmp(bytes, BUFget_ptr(), BUFget_buffer_size());
	if(!result)
		*out_result = false;
	ASSERT(result, NULL, "BUFclone doesn't working correctly, raw copy buffer is not equal to the original raw buffer");
	BUFpop_binded();
	return clone;
}

TESTED
void test_BUFresize(buf_ucount_t new_capacity, bool* out_result)
{
	//perform a test
	BUFFER* test_buffer = BUFcreate(NULL, sizeof(unsigned char), 0, 1); 
	BUFpush_binded();
	BUFbind(test_buffer);
	
	char null_value = 0; 
	BUFset_offset_bytes(&null_value);

	const char* test_string = "Hello World";
	int len = strlen(test_string);
	char* string = malloc(len + 1); 
	memcpy(string, test_string, len + 1);
	BUFresize(len);
	for(int i = 0; i < len; i++)
		BUFpush(&string[i]); 
	BUFfit(); 
	char* data = BUFget_ptr(); 
	if(strcmp(data, test_string))
	{
		BUFfree();
		free(string);
		BUFpop_binded();
		BUFresize(new_capacity);
		*out_result = false;
		ASSERT(false, VOID, "Either of BUFset_offset_bytes, BUFresize, BUFpush, BUFfit or BUFget_ptr doesn't work correctly");	
	}
	BUFfree();
	free(string);
	BUFpop_binded();
	//

	BUFresize(new_capacity);
}

TESTED
void test_BUFfree()
{
	BUFfree();
}

static bool compare_int(int* i1, int* i2)
{
	return *i1 == *i2;
}

bool BUFstart_testing(BUFFERTest* test)
{
	BUFpush_binded();
	int test_count = 1;
	bool result = true;
	/*-------------------------BUFbind----------------------*/
	LOG_TEST(test_count, "BUFbind"); test_count++;
	test_BUFbind(test->buffer, &result);
	LOG_RESULT(result);
	/*------------------------------------------------------*/

	/*---------------------BUFget_element_size--------------*/
	LOG_TEST(test_count, "BUFget_element_size");  test_count++; result = true;
	test_BUFget_element_size(&result);
	LOG_RESULT(result);
	/*-------------------------------------------------------*/

	/*-----------------------BUFpush------------------------*/
	LOG_TEST(test_count, "BUFpush"); test_count++; result = true;
	for(int i = 1; i <= 11; i++)
		test_BUFpush(GET_AT(i, BUFget_element_size(), test->test_elements), test->equal, &result);
	LOG_RESULT(result);
	/*------------------------------------------------------*/

	/*----------------------BUFpeek-------------------------*/
	LOG_TEST(test_count, "BUFpeek"); test_count++; result = true; 
	buf_ucount_t peek_count = 6;
	void* memblock = malloc(BUFget_element_size());
	for(buf_ucount_t i = 0; i < peek_count; i++)
	{
		test_BUFpeek(memblock, test->equal, &result);
		BUFpop(NULL);
	}
	LOG_RESULT(result);
	/*------------------------------------------------------*/

	/*--------------------BUFpeek_ptr------------------------*/
	LOG_TEST(test_count, "BUFpeek_ptr"); test_count++; result = true; 
	void* ptr = test_BUFpeek_ptr(test->equal, &result);
	LOG_RESULT(result);
	/*-------------------------------------------------------*/

	/*-------------------BUFget_element_count---------------*/
	LOG_TEST(test_count, "BUFget_element_count"); test_count++; result = true; 
	test_BUFget_element_count(&result);
	LOG_RESULT(result);

	/*------------------BUFget_offset----------------------*/
	LOG_TEST(test_count, "BUFget_offset"); test_count++; result = true; 
	test_BUFget_offset(&result); 
	LOG_RESULT(result);

	/*----------------------BUFget_capacity------------------*/
	LOG_TEST(test_count, "BUFget_capacity"); test_count++; result = true;
	result = (16 == BUFget_capacity());
	LOG_RESULT(result);
	/*-------------------------------------------------------*/

	/*----------------------BUFclear-------------------------*/
	LOG_TEST(test_count, "BUFclear"); test_count++; result = true;
	test_BUFclear(GET_AT(0, BUFget_element_size(), test->test_elements), test->equal, &result); 
	result = (result && (!BUFget_element_count()));
	LOG_RESULT(result);
	/*--------------------------------------------------------*/

	/*---------------------BUFpop-----------------------------*/
	LOG_TEST(test_count, "BUFpop"); test_count++; result = true;
	for(int i = 1; i <= 10; i++)
		test_BUFpush(GET_AT(i, BUFget_element_size(), test->test_elements), test->equal, &result);

	void* temp_ptr = malloc(10 * BUFget_element_size());
	for(int i = 0; i < 10; i++)
		test_BUFpop(GET_AT(9 - i, BUFget_element_size(), temp_ptr), test->equal, &result);
	buf_ucount_t* values = temp_ptr;
	bool result0 = true;
	for(int i = 0; i < 10; i++)
	{
		result0 = test->equal(GET_AT(i, BUFget_element_size(), temp_ptr), GET_AT(i + 1, BUFget_element_size(), test->test_elements));
		if(!result0)
			break;
	}
	result = (result && result0);
	LOG_RESULT(result);
	/*---------------------------------------------------------*/

	/*--------------------BUFresize----------------------------*/
	LOG_TEST(test_count, "BUFresize"); test_count++; result = true;
	BUFclear_buffer(NULL);
	bool result1 = (!BUFget_element_count()) && (BUFget_capacity() == 16); 
	bool result2 = true;
	bool result3 = true;
	BUFfit();
	result1 = (result1 && (BUFget_ptr() == NULL));
	BUFresize(10); 
	result1 = (result1 && (BUFget_capacity() == 10));
	for(int i = 1; i <= 10; i++)
		BUFpush(GET_AT(i, BUFget_element_size(), test->test_elements));
	BUFresize(8);
	memset(temp_ptr, 0, 10 * BUFget_element_size());
	for(int i = 0; i < 8; i++)
		BUFpop(GET_AT(7 - i, BUFget_element_size(), temp_ptr));
	for(int i = 0; i < 8; i++)
	{
		result2 = test->equal(GET_AT(i, BUFget_element_size(), temp_ptr), GET_AT(i + 1, BUFget_element_size(), test->test_elements));
		if(!result2)
		 	break;
	}
	for(int i = 1; i <= 8; i++)
		BUFpush(GET_AT(i, BUFget_element_size(), test->test_elements));
	result3 = (BUFget_capacity() == 8) && (BUFget_element_count() == 8);
	result = (result && result1 && result2 && result3); 
	LOG_RESULT(result);

	/*------------------BUFget_clone()-----------------------------*/
	LOG_TEST(test_count, "BUFget_clone"); test_count++; result = true; result1 = true;
	BUFFER* buffer = test_BUFget_clone(&result);
	result1 = (buffer != NULL); 
	result = (result && result1);
	BUFbind(buffer); 
	BUFfree();
	BUFbind(test->buffer);
	LOG_RESULT(result); 
	/*--------------------------------------------------------------*/

	/*----------------------BUFfind_index_of-------------------------*/
	LOG_TEST(test_count, "BUFfind_index_of"); test_count++; result = true; result1 = true;
	for(buf_ucount_t expected_index = 3; expected_index <= 7; expected_index++)
	{
		buf_ucount_t index = test_BUFfind_index_of(GET_AT(expected_index + 1, BUFget_element_size(), test->test_elements), test->equal, &result);
		result1 = result1 && (expected_index == index); 
		result1 = result1 && (index != BUF_INVALID_INDEX); 
		result = result && result1;
	}
	LOG_RESULT(result);
	/*---------------------------------------------------------------*/

	/*--------------------BUFremove-----------------------------------*/
	LOG_TEST(test_count, "BUFremove"); test_count++; result = true;
	buf_ucount_t save_count = BUFget_element_count();
	for(buf_ucount_t remove_index = 0; remove_index <= 5; remove_index++)
	{
		bool is_removed = test_BUFremove(GET_AT(remove_index + 1, BUFget_element_size(), test->test_elements), test->equal, &result);
		bool is_found = BUFfind_index_of(GET_AT(remove_index + 1, BUFget_element_size(), test->test_elements), test->equal) != BUF_INVALID_INDEX;
		result = result && is_removed && (!is_found);
	}
	result = result && ((BUFget_element_count() + 6) == save_count);
	LOG_RESULT(result);
	/*----------------------------------------------------------------*/

	/*----------------------BUFinsert_at------------------------------*/
	LOG_TEST(test_count, "BUFinsert_at"); test_count++; result = true; 
	save_count = BUFget_element_count(); 
	for(buf_ucount_t insert_index = 0; insert_index <= 5; insert_index++)
	{
		test_BUFinsert_at(insert_index, GET_AT((5 - insert_index) + 1, BUFget_element_size(), test->test_elements), test->equal, &result);
		if(!result)
			break;
	}
	result = result && ((BUFget_element_count() - 6) == save_count);
	LOG_RESULT(result);
	/*----------------------------------------------------------------*/

	/*------------------BUFcreate_object-----------------------------*/
	LOG_TEST(test_count, "BUFcreate_object"); test_count++; result = true;
	result1 = true; 
	result2 = true;
	BUFFER* stack_object_buffer = test_BUFcreate_object(test->bytes, &result1);
	stack_object_buffer = test_BUFcreate(stack_object_buffer, sizeof(BUFget_element_size()), BUFget_capacity(), 4, &result2);
	BUFbind(stack_object_buffer);
	BUFfree();
	BUFbind(test->buffer);
	result = (result && result1 && result2);
	LOG_RESULT(result);
	/*----------------------------------------------------------------*/


	free(temp_ptr);
	free(memblock);
	BUFpop_binded();
	return result;
}

bool BUFstart_default_testing()
{
	BUFFERTest test; 
	test.buffer = BUFcreate(NULL, sizeof(buf_ucount_t), 0, 0); 
	uint8_t bytes[BUFget_buffer_object_size()];
	test.bytes = &bytes[0];
	test.equal = (bool (*)(void*, void*))compare_int;
	test.print = (void (*)(void*))print_int;
	buf_ucount_t* ints = malloc(sizeof(buf_ucount_t) * 12); 
	ints[0] = 16; 
	ints[1] = 11;
	ints[2] = 22;
	ints[3] = 33;
	ints[4] = 44;
	ints[5] = 45;
	ints[6] = 12;
	ints[7] = 31; 
	ints[8] = 32; 
	ints[9] = 11; 
	ints[10] = 12; 
	ints[11] = 34; 
	test.test_elements = &ints[0];

	bool result = BUFstart_testing(&test);
	BUFbind(test.buffer);
	BUFfree();
	return result;
}