#include "buffer.h"

typedef struct BUFFERTest
{
	BUFFER* buffer;
	uint8_t* bytes; 			//size must be at least BUFget_buffer_object_size()
	bool (*equal)(void*, void*);
	void (*print)(void*);
	void* test_elements;		// [clear value]
} BUFFERTest;

BUFFER* test_BUFcreate(BUFFER* __buffer, buf_ucount_t element_size, buf_ucount_t capacity, buf_ucount_t offset, bool *out_result);
BUFFER* test_BUFcreate_object(uint8_t* bytes, bool* out_result);
void test_BUFget_at(buf_ucount_t index , void* out_value, bool (*compare)(void*, void*), bool* out_result);
void test_BUFset_at(buf_ucount_t index , void* in_value, bool (*compare)(void*, void*), bool* out_result);
void* test_BUFgetptr_at(buf_ucount_t index, bool (*compare)(void*, void*), bool* out_result);
void test_BUFfit(bool* out_result);
buf_ucount_t test_BUFget_element_count(bool* out_result);
buf_ucount_t test_BUFget_element_size(bool* out_result);
buf_ucount_t test_BUFget_capacity(bool* out_result);
buf_ucount_t test_BUFget_offset(bool* out_result);
void test_BUFset_capacity(buf_ucount_t capacity, bool* out_result) ;
void test_BUFset_element_count(buf_ucount_t element_count, bool* out_result);
void test_BUFset_element_size(buf_ucount_t element_size, bool* out_result);
void test_BUFset_ptr(uint8_t* ptr, bool* out_result);
void test_BUFset_offset(buf_ucount_t offset, bool* out_result);
void* test_BUFget_offset_bytes(bool* out_result);
void test_BUFset_offset_bytes(uint8_t* bytes, bool* out_result);
void test_BUFbind(BUFFER* buffer, bool* out_result);
uint8_t* test_BUFget_ptr(bool (*compare)(void*, void*), bool* out_result);
void test_BUFpeek(void* out_value, bool (*compare)(void*, void*), bool* out_result);
void* test_BUFpeek_ptr(bool (*comparer)(void*, void*), bool* out_result);
void test_BUFpop(void* out_value, bool (*compare)(void*, void*), bool* out_result);
void test_BUFpush(void* in_value, bool (*comparer)(void*, void*), bool* out_result);
bool test_BUFremove(void* object, bool (*comparer)(void*, void*), bool *out_result);
bool test_BUFremove_noshift(void* object, bool (*comparer)(void*, void*), bool* out_result);
buf_ucount_t test_BUFget_buffer_size();
void test_BUFclear_buffer(void* clear_value, bool (*comparer)(void*, void*), bool *out_result);
void test_BUFclear(void* clear_value, bool (*comparer)(void*, void*), bool* out_result);
bool test_BUFremove_at_noshift(buf_ucount_t index , void* out_value, bool (*comparer)(void*, void*), bool* out_result);
bool test_BUFremove_at(buf_ucount_t index , void* out_value, bool (*comparer)(void*, void*), bool *out_result);
void test_BUFinsert_at(buf_ucount_t index , void* in_value, bool (*comparer)(void*, void*), bool *out_result);
void test_BUFinsert_at_noalloc(buf_ucount_t index , void* in_value , void* out_value, bool (*comparer)(void*, void*), bool* out_result);
buf_ucount_t test_BUFfind_index_of(void* value, bool (*comparer)(void*, void*), bool* out_result);
BUFFER* test_BUFget_binded_buffer(bool* out_result);
BUFFER* test_BUFget_clone(bool* out_result);
void test_BUFresize(buf_ucount_t new_capacity, bool* out_result);
void test_BUFfree();


bool BUFstart_testing(BUFFERTest* test);
bool BUFstart_default_testing();