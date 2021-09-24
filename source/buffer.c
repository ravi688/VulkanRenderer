#include "buffer.h"				//This must be included at the top for preprocessing reasons


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef BUF_NOT_INCLUDE_COMMON_UTILITIES
#include "CommonUtilities.h"
#endif/*BUF_NOT_INCLUDE_COMMON_UTILITIES*/


#if BUFFER_LIB_VERSION < 0x0016
#error "Buffer.c requries BUFFER_LIB_VERSION greater than or equal to 0x0016"
#endif

//Version 1.6

#define STACK_ALLOCATED_OBJECT  0x1
#define HEAP_ALLOCATED_OBJECT 	 0x2

struct __BUFFER 
{
	uint8_t* bytes;
	buf_ucount_t info; 
	buf_ucount_t capacity;
	buf_ucount_t element_count;		
	buf_ucount_t element_size;
	buf_ucount_t offset; 		//added in version 1.5
	struct __BUFFER* auto_managed_empty_blocks; 
#ifdef BUF_CALLBACKS_SUPPORTED
	void (*on_post_resize)(void);
	void (*on_pre_resize)(void);
#endif/*BUF_CALLBACKS_SUPPORTED*/
	void (*free)(void*);					//TODO: add a version number to this added
	bool is_auto_managed; 
};
struct __BUFFER* binded_buffer;
struct __BUFFER* temp_binded_buffer;;

#ifdef BUF_GET_BUFFER_OBJECT_SIZE_FUNCTION_SUPPORTED
function_signature_void(uint64_t, BUFget_buffer_object_size)
{ 
	return sizeof(struct __BUFFER); 
}
#endif/*BUF_GET_BUFFER_OBJECT_SIZE_FUNCTION_SUPPORTED*/
#ifdef BUF_CALLBACKS_SUPPORTED
function_signature(void, BUFset_on_post_resize, void (*on_post_resize)(void))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->on_post_resize = on_post_resize;
}
function_signature(void, BUFset_on_pre_resize, void (*on_pre_resize)(void))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->on_pre_resize = on_pre_resize;
}
#endif/*BUF_CALLBACKS_SUPPORTED*/
#ifdef BUF_PSEUDO_INSERT_REMOVE_PUSH_AND_POP_SUPPORTED
function_signature(void, BUFpush_pseudo, buf_ucount_t count)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	buf_ucount_t previous_element_count = binded_buffer->element_count;
	binded_buffer->element_count += count; 
	if(binded_buffer->capacity <= 0)
		binded_buffer->capacity = 1;
	buf_ucount_t previous_capacity = binded_buffer->capacity;

	while(binded_buffer->capacity < binded_buffer->element_count)
		binded_buffer->capacity *= 2;
	if(previous_capacity != binded_buffer->capacity)
	{
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_pre_resize != NULL)
		{
			buf_ucount_t temp = previous_capacity; 
			previous_capacity = binded_buffer->capacity; 
			binded_buffer->capacity = temp;
		 	(binded_buffer->on_pre_resize)();
		 	temp = previous_capacity;
		 	previous_capacity = binded_buffer->capacity; 
			binded_buffer->capacity = temp;
		}
		#endif
		binded_buffer->bytes = realloc(binded_buffer->bytes , binded_buffer->capacity * binded_buffer->element_size); 
		GOOD_ASSERT(binded_buffer->bytes != NULL, "Memory Allocation Failure Exception");
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_post_resize != NULL) (binded_buffer->on_post_resize)();
		#endif
	}
	memset(binded_buffer->bytes + previous_element_count * binded_buffer->element_size , 0 , binded_buffer->element_size * count); 
}
function_signature(void, BUFpop_pseudo, buf_ucount_t count)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(count <= binded_buffer->element_count, "Buffer Underflow Exception");
	binded_buffer->element_count -= count;	
}
function_signature(void, BUFinsert_pseudo, buf_ucount_t index, buf_ucount_t count)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->capacity,"Buffer Overflow Exception");
	buf_ucount_t num_shift_elements = binded_buffer->element_count - index; 
	binded_buffer->element_count += count; 
	if(binded_buffer->element_count > binded_buffer->capacity)
	{	
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_pre_resize != NULL) (binded_buffer->on_pre_resize)();
		#endif
		binded_buffer->bytes = realloc(binded_buffer->bytes , binded_buffer->element_size * binded_buffer->element_count);
		GOOD_ASSERT(binded_buffer->bytes != NULL, "Memory Allocation Failure Exception");
		binded_buffer->capacity = binded_buffer->element_count;
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_post_resize != NULL) (binded_buffer->on_post_resize)();
		#endif
	}
	uint8_t* dst_ptr = binded_buffer->bytes + (binded_buffer->element_count - 1) * binded_buffer->element_size;
	uint8_t offset = binded_buffer->element_size * count;
	while(num_shift_elements)
	{
		memcpy(dst_ptr , dst_ptr - offset, binded_buffer->element_size) ; 
	    dst_ptr -= binded_buffer->element_size;
		--num_shift_elements;
	}
	memset(binded_buffer->bytes + index * binded_buffer->element_size, 0, count * binded_buffer->element_size);
}
function_signature(void, BUFremove_pseudo, buf_ucount_t index, buf_ucount_t count)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->element_count, "Index Out Of Range Exception");
	GOOD_ASSERT(count <= binded_buffer->element_count,"Buffer Underflow Exception");
	binded_buffer->element_count -= count; 
	buf_ucount_t num_shift_elements = binded_buffer->element_count;
	uint8_t* dst_ptr = binded_buffer->bytes + index * binded_buffer->element_size;
	while(num_shift_elements)
	{
		memcpy(dst_ptr , dst_ptr + binded_buffer->element_size, binded_buffer->element_size); 
		dst_ptr += binded_buffer->element_size;
		--num_shift_elements;
	}
	memset(binded_buffer->bytes + binded_buffer->element_count * binded_buffer->element_size , 0 , binded_buffer->element_size * count);
}
#endif/*BUF_PSEUDO_INSERT_REMOVE_PUSH_AND_POP_SUPPORTED*/
function_signature(void, BUFset_auto_managed, bool value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception"); 
	if(value && (binded_buffer->auto_managed_empty_blocks == NULL))
			binded_buffer->auto_managed_empty_blocks = BUFcreate(NULL, sizeof(uint8_t*) , 1, 0); 
	else if(!value && (binded_buffer->auto_managed_empty_blocks != NULL))
			{
				BUFFER* previous_binded_buffer = binded_buffer; 
				BUFbind(binded_buffer->auto_managed_empty_blocks); 
				BUFfree(); 
				BUFbind(previous_binded_buffer); 
			}
 	binded_buffer->is_auto_managed = value; 
}
#if BUFFER_LIB_VERSION < 0x0014
void BUFpush_binded()
{
	temp_binded_buffer = binded_buffer;
	return;
}
void BUFpop_binded()
{
	binded_buffer = temp_binded_buffer;
	return;
}
#endif
function_signature_void(buf_ucount_t, BUFget_offset)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX); 
	return binded_buffer->offset;
}
function_signature_void(buf_ucount_t, BUFget_capacity)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
 	return binded_buffer->capacity;
 }
function_signature_void(buf_ucount_t, BUFget_element_count) 
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
 	return binded_buffer->element_count;
} 
function_signature_void(buf_ucount_t, BUFget_element_size)
{ 
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
	return binded_buffer->element_size;  
} 
function_signature_void(void*, BUFget_ptr)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL); 
	return binded_buffer->bytes; 
} 
function_signature(void, BUFset_offset, buf_ucount_t offset)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->offset = offset;
}
function_signature(void, BUFset_capacity, buf_ucount_t capacity)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
 	binded_buffer->capacity = capacity;
}
function_signature(void, BUFset_element_count, buf_ucount_t element_count)
{ 
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->element_count = element_count; 
}
function_signature(void, BUFset_element_size,  buf_ucount_t element_size)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->element_size = element_size; 
} 
function_signature(void, BUFset_ptr, uint8_t* ptr)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->bytes = ptr; 
}
function_signature_void(BUFFER*, BUFget_binded_buffer)
{ 
	return binded_buffer;
} 
function_signature_void(bool, BUFis_auto_managed)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", false);
	return binded_buffer->is_auto_managed; 
}
function_signature(void, BUFbind, BUFFER* buffer)
{
  binded_buffer = buffer; 
} 
function_signature_void(void, BUFunbind)
{ 
	binded_buffer = NULL; 
} 
function_signature_void(void, BUFlog)
{
	printf(
			 "----------------------\n"
			 "ElementCount :\t%u\n"
		   "Capacity :\t%u\n"
		   "ElementSize :\t%u\n"
		   "Offset: \t%u\n"
		   "----------------------\n",
		    binded_buffer->element_count,
		    binded_buffer->capacity,
		    binded_buffer->element_size,
		    binded_buffer->offset
			);
}
#if BUFFER_LIB_VERSION >= 0x0015
//Added in version 1.5
function_signature(void, BUFtraverse_elements, buf_ucount_t start, buf_ucount_t end, void (*func)(void* /*element ptr*/, void* /*args ptr*/), void* args)
{
	 GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception"); 
	 GOOD_ASSERT((start <= end) && (start < binded_buffer->element_count) && (end < binded_buffer->element_count), "(start <= end) && (start < binded_buffer->element_count) && (end < binded_buffer->element_count) evaulates to false");
	 for(buf_ucount_t i = start; i <= end; i++)
	 			func(BUFget_ptr_at(i), args);
}
#endif/*BUFFER_LIB_VERSION >= 0x0015*/
function_signature(bool, BUFisStackAllocated, BUFFER* buffer)
{
	return ((((struct __BUFFER*)buffer)->info) & STACK_ALLOCATED_OBJECT) == STACK_ALLOCATED_OBJECT; 
}
function_signature(bool, BUFisHeapAllocated, BUFFER* buffer)
{
	return ((((struct __BUFFER*)buffer)->info) & HEAP_ALLOCATED_OBJECT) == HEAP_ALLOCATED_OBJECT; 
}
function_signature_void(void, BUFfree)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	if((binded_buffer->free != NULL) && BUFget_element_count()) 										//TODO: Add version number to this addition
		BUFtraverse_elements(0, BUFget_element_count() - 1, (void (*)(void*, void*))binded_buffer->free, NULL);
	if(binded_buffer->bytes != NULL)
	{
		free(binded_buffer->bytes);  
		binded_buffer->bytes == NULL;
	}

  if(binded_buffer->info & HEAP_ALLOCATED_OBJECT)
  { binded_buffer->info = 0x00; free(binded_buffer) ; } 
	binded_buffer = NULL;
}
#if BUFFER_LIB_VERSION >= 0x0016
//Added in version 1.5
function_signature_void(BUFFER*, BUFget_clone)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL); 
	BUFFER* cloned_buffer = BUFcreate(NULL, binded_buffer->element_size, binded_buffer->capacity, binded_buffer->offset);
	memcpy(cloned_buffer->bytes, binded_buffer->bytes, binded_buffer->capacity * binded_buffer->element_size + binded_buffer->offset);
	cloned_buffer->element_count = binded_buffer->element_count; 
	cloned_buffer->on_pre_resize = binded_buffer->on_pre_resize; 
	cloned_buffer->on_post_resize = binded_buffer->on_post_resize;
	return cloned_buffer;
}
#endif/*BUFFER_LIB_VERSION >= 0x0015*/

#if BUFFER_LIB_VERSION >= 0x0016
function_signature(BUFFER*, BUFmove_to, BUFFER* destination)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL); 
	GOOD_ASSERT(binded_buffer != destination, "Source and Destination BUFFERs are referencing to the same memory location", NULL); 
	GOOD_ASSERT(destination->element_size == BUFget_element_size(), "Source.element_size != Destination.element_size", NULL);

	BUFpush_binded();
	BUFcopy_to(destination);
	BUFfree(); 
	BUFpop_binded();
}
function_signature(BUFFER*, BUFcopy_to, BUFFER* destination)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL); 
	GOOD_ASSERT(destination != NULL, "Destination Buffer Is NULL Exception", NULL);
	GOOD_ASSERT(binded_buffer != destination, "Source and Destination BUFFERs are referencing to the same memory location", NULL); 
	GOOD_ASSERT(destination->element_size == BUFget_element_size(), "Source.element_size != Destination.element_size", NULL);
	BUFpush_binded();
	if(destination->capacity < binded_buffer->element_count)
	{
		BUFbind(destination); 
		BUFresize(binded_buffer->element_count);
	}
	memcpy(destination->bytes, binded_buffer->bytes, binded_buffer->capacity * binded_buffer->element_count + binded_buffer->offset);
	destination->element_count = binded_buffer->element_count; 
	BUFpop_binded();
}
function_signature(BUFFER*, BUFcopy_construct, BUFFER* source_buffer)
{
	GOOD_ASSERT(source_buffer != NULL, "Source Buffer Is NULL Exception", NULL); 
	BUFbind(source_buffer);
	uint8_t* offset_bytes = BUFget_offset_bytes();
	BUFFER* buffer = BUFcreate(NULL, source_buffer->element_size, source_buffer->capacity, source_buffer->offset); 
	BUFbind(buffer);
	if(offset_bytes != NULL)
		BUFset_offset_bytes(offset_bytes); 
	BUFbind(source_buffer);
	BUFcopy_to(buffer);
	BUFbind(buffer); 
	BUFset_auto_managed(source_buffer->is_auto_managed); 
	BUFset_on_pre_resize(source_buffer->on_pre_resize); 
	BUFset_on_post_resize(source_buffer->on_post_resize); 
	BUFset_on_free(source_buffer->free);
	return buffer;
}

function_signature(void, BUFset_on_free, void (*free)(void*))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL);
	binded_buffer->free = free;
}

#endif/*BUFFER_LIB_VERSION >= 0x0015*/
//TODO
//Replace the name with BUFcreate_object_from_bytes
function_signature(BUFFER*, BUFcreate_object, uint8_t* bytes)
{
	struct __BUFFER* buffer = (struct __BUFFER*)(bytes);
	buffer->bytes = NULL;
	buffer->info = 0x00;
	buffer->info |= STACK_ALLOCATED_OBJECT; 
	buffer->element_size = 0;
	buffer->element_count = 0;
	buffer->capacity = 0;
	buffer->auto_managed_empty_blocks = NULL; 
	buffer->is_auto_managed = false; 
	buffer->on_pre_resize = NULL; 
	buffer->on_post_resize = NULL;
	buffer->offset = 0;	//added in version 1.5
	buffer->free = NULL;
	return buffer;
}
function_signature(BUFFER*, BUFcreate, BUFFER* __buffer, buf_ucount_t element_size, buf_ucount_t capacity, buf_ucount_t offset)
{
	GOOD_ASSERT((int64_t)(element_size) != -1, "element_size cannot be negative", NULL); 
	GOOD_ASSERT((int64_t)(capacity) != -1, "capacity cannot be negative", NULL);
	GOOD_ASSERT((int64_t)(offset) != -1, "offset cannot be negative", NULL);
	struct __BUFFER* buffer = (struct __BUFFER*)__buffer;
	if(buffer == NULL)
	{ 
		buffer = malloc(sizeof(struct __BUFFER));
		GOOD_ASSERT(buffer != NULL, "Memory Allocation Failure Exception", NULL);
		buffer->info = 0x00; 
		buffer->info |= HEAP_ALLOCATED_OBJECT;
		buffer->auto_managed_empty_blocks = NULL; 
		buffer->is_auto_managed = false;
		buffer->on_pre_resize = NULL; 
		buffer->on_post_resize = NULL;
		buffer->free = NULL;
	}
	buffer->bytes = (uint8_t*)malloc(element_size * capacity + offset); 
	GOOD_ASSERT(buffer->bytes != NULL, "Memory Allocation Failure Exception", NULL);
	buffer->element_size = element_size; 
	buffer->capacity = capacity;
	buffer->element_count = 0;
	buffer->offset = offset; //added in version 1.5
	return buffer; 
}
function_signature(void, BUFget_at, buf_ucount_t index, void* out_value)
{	
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception");
	memcpy(out_value , binded_buffer->bytes + index * binded_buffer->element_size, binded_buffer->element_size); 
}
function_signature(void*, BUFgetptr_at, buf_ucount_t index)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL);
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception", NULL);
	return binded_buffer->bytes + index * binded_buffer->element_size; 
}
function_signature(void, BUFset_at, buf_ucount_t index , void* in_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception");
	memcpy(binded_buffer->bytes + index * binded_buffer->element_size, in_value , binded_buffer->element_size) ; 
}
#if BUFFER_LIB_VERSION >= 0x0015
function_signature_void(void*, BUFget_offset_bytes)
{
	if(binded_buffer->offset == 0)
		return NULL; 
	return binded_buffer->bytes + binded_buffer->capacity * binded_buffer->element_size;
}
function_signature(void, BUFset_offset_bytes, uint8_t* offset_bytes)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(offset_bytes != NULL, "offset_bytes is NULL Exception");
	if(binded_buffer->offset == 0)
		return;
	memcpy(binded_buffer->bytes + binded_buffer->capacity * binded_buffer->element_size, offset_bytes, binded_buffer->offset);
}
#endif

#if BUFFER_LIB_VERSION >= 0x0015
//Added in version 1.5
function_signature_void(buf_ucount_t, BUFget_buffer_size)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
	return binded_buffer->capacity * binded_buffer->element_size  + binded_buffer->offset;
}
#endif /*BUFFER_LIB_VERSION >= 0x0015*/
#if BUFFER_LIB_VERSION >= 0x0015
//Added in version 1.5
function_signature(void, BUFresize, buf_ucount_t new_capacity)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	if(new_capacity == binded_buffer->capacity)
		return;
	buf_ucount_t new_buffer_size = new_capacity * binded_buffer->element_size + binded_buffer->offset; 
	buf_ucount_t buffer_size = binded_buffer->capacity * binded_buffer->element_size + binded_buffer->offset;
	void* new_buffer = malloc(new_buffer_size); 
	if(new_buffer_size > buffer_size)
	{
		if((buffer_size - binded_buffer->offset) != 0)
		//copy only the elments, excluding the offset
		memcpy(new_buffer, binded_buffer->bytes, buffer_size - binded_buffer->offset);
		if(binded_buffer->offset != 0) 
		//copy the offset bytes at the end of the new buffer
		memcpy(new_buffer + new_buffer_size - binded_buffer->offset, binded_buffer->bytes + buffer_size - binded_buffer->offset, binded_buffer->offset);
		//set the intermediate bytes to zero
		memset(new_buffer + buffer_size - binded_buffer->offset, 0, new_buffer_size - buffer_size);
	}
	else//if (new_buffer_size < buffer_size)
	{
		//copy only the elements, excluding the offset
		memcpy(new_buffer, binded_buffer->bytes, new_buffer_size - binded_buffer->offset);
		if(binded_buffer->offset != 0)
		//copy the offset bytes at the end of the new buffer
		memcpy(new_buffer + new_buffer_size - binded_buffer->offset, binded_buffer->bytes + buffer_size - binded_buffer->offset, binded_buffer->offset);
	}
	if(binded_buffer->bytes != NULL)
		free(binded_buffer->bytes);
	binded_buffer->bytes = new_buffer;
	if((new_buffer_size < buffer_size) && (binded_buffer->element_count >= binded_buffer->capacity))
		binded_buffer->element_count = new_capacity;
	binded_buffer->capacity = new_capacity;
}
#endif /*BUFFER_LIB_VERSION >= 0x0015*/

#if BUFFER_LIB_VERSION >= 0x0015
//Added in version 1.5
//Clears entire buffer
function_signature(void, BUFclear_buffer, void* clear_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	if(clear_value == NULL)
		memset(binded_buffer->bytes, 0, binded_buffer->capacity * binded_buffer->element_size + binded_buffer->offset); 
	else
	{
		buf_ucount_t capacity = binded_buffer->capacity; 
		while(capacity > 0)
		{
			--capacity;
			memcpy(binded_buffer->bytes + capacity * binded_buffer->element_size, clear_value, binded_buffer->element_size);
		}
		memset(binded_buffer->bytes + binded_buffer->capacity * binded_buffer->element_size, 0, binded_buffer->offset);
	}
	binded_buffer->element_count = 0;
}
#endif/*BUFFER_LIB_VERSION >= 0x0015*/
//Clears only elements
function_signature(void, BUFclear, void* clear_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	if(clear_value == NULL)
		memset(binded_buffer->bytes , 0 , binded_buffer->element_count * binded_buffer->element_size) ; 
	else
	{
		buf_ucount_t element_count = binded_buffer->element_count;
		while(element_count > 0)
		{
			--element_count;
			memcpy(binded_buffer->bytes + element_count * binded_buffer->element_size, clear_value , binded_buffer->element_size); 
		}
	} 
 	binded_buffer->element_count = 0;
}
function_signature(void, BUFinsert_at_noalloc, buf_ucount_t index , void* in_value , void* out_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->capacity,"Buffer Overflow Exception");
	if(out_value != NULL)
		memcpy(out_value , binded_buffer->bytes + index * binded_buffer->element_size , binded_buffer->element_size) ; 
	memcpy(binded_buffer->bytes + index * binded_buffer->element_size, in_value , binded_buffer->element_size) ;  
}
function_signature(void, BUFinsert_at, buf_ucount_t index , void* in_value)
{	
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->capacity,"Buffer Overflow Exception");
	buf_ucount_t num_shift_elements = binded_buffer->element_count - index; 
	++(binded_buffer->element_count); 
	if((binded_buffer->element_count) > binded_buffer->capacity)
	{
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_pre_resize != NULL)(binded_buffer->on_pre_resize)();
		#endif
		binded_buffer->bytes = realloc(binded_buffer->bytes , binded_buffer->element_size * binded_buffer->element_count) ; 
		GOOD_ASSERT(binded_buffer->bytes != NULL, "Memory Allocation Failure Exception");
		binded_buffer->capacity = binded_buffer->element_count;
		#ifdef BUF_CALLBACKS_SUPPORTED
		if(binded_buffer->on_post_resize != NULL) (binded_buffer->on_post_resize)();
		#endif
	}

	uint8_t* dst_ptr = binded_buffer->bytes + (binded_buffer->element_count - 1) * binded_buffer->element_size;
	while(num_shift_elements)
	{
		memcpy(dst_ptr , (dst_ptr - binded_buffer->element_size) , binded_buffer->element_size) ; 
	    dst_ptr -= binded_buffer->element_size;
		--num_shift_elements;
	}
	memcpy(dst_ptr , in_value , binded_buffer->element_size); 
}
static bool ptr_comparer(void* ptr1, void* ptr2) { return *((uint8_t*)ptr1) == *((uint8_t*)ptr2); }
#if BUFFER_LIB_VERSION >= 0x0015
function_signature(bool, BUFremove_at_noshift, buf_ucount_t index , void* out_value)
#else
function_signature(void, BUFremove_at_noshift, buf_ucount_t index , void* out_value)
#endif
{
	#if BUFFER_LIB_VERSION >= 0x0015
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", false);
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception", false);
	#else
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception");
	#endif
	if(out_value != NULL)
		memcpy(out_value , binded_buffer->bytes + index * binded_buffer->element_size , binded_buffer->element_size) ;
	if(BUFis_auto_managed())
	{		
		BUFFER* previous_binded_buffer = binded_buffer; 
		BUFbind(binded_buffer->auto_managed_empty_blocks); 
		uint8_t* ptr = binded_buffer->bytes + index * binded_buffer->element_size;

		if(BUFfind_index_of(&ptr, ptr_comparer) == -1) /*if ptr is not found in the auto_managed_empty_blocks BUFFER*/
			BUFpush(&ptr);
		BUFbind(previous_binded_buffer); 
	}
	memset(binded_buffer->bytes + index * binded_buffer->element_size , 0 , binded_buffer->element_size); 
	#if BUFFER_LIB_VERSION >= 0x0015
	return true;
	#endif
}

#if BUFFER_LIB_VERSION >= 0x0015
function_signature(bool, BUFremove_at, buf_ucount_t index , void* out_value)
#else
function_signature(void, BUFremove_at, buf_ucount_t index , void* out_value)
#endif
{
	#if BUFFER_LIB_VERSION >= 0x0015
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", false);
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception", false);
	#else
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	GOOD_ASSERT(index < binded_buffer->element_count,"Index Out of Range Exception");
	#endif
	--(binded_buffer->element_count); 
	if(out_value != NULL)
		memcpy(out_value , binded_buffer->bytes + index * binded_buffer->element_size , binded_buffer->element_size); 

	uint8_t* dst_ptr = binded_buffer->bytes + index * binded_buffer->element_size;
	if(index <= binded_buffer->element_count)
	{
		buf_ucount_t num_shift_elements = (binded_buffer->element_count - index) ;
		while(num_shift_elements > 0)
		{
			memcpy(dst_ptr , dst_ptr + binded_buffer->element_size, binded_buffer->element_size); 
			dst_ptr += binded_buffer->element_size;
			--num_shift_elements;
		}
	}
	memset(dst_ptr , 0 , binded_buffer->element_size); 
	#if BUFFER_LIB_VERSION >= 0x0015
	return true;
	#endif
}
function_signature(bool, BUFremove_noshift, void* object, bool (*comparer)(void*, void*))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", false);
	uint8_t* cursor = binded_buffer->bytes; 
	for(int i = 0; i < binded_buffer->element_count; i++, cursor += binded_buffer->element_size)
	{
		if(comparer(cursor, object))
		{
			memset(cursor, 0, binded_buffer->element_size); 
			if(BUFis_auto_managed())
			{		
				BUFFER* previous_binded_buffer = binded_buffer; 
				BUFbind(binded_buffer->auto_managed_empty_blocks); 
				if(BUFfind_index_of(&cursor, ptr_comparer) == BUF_INVALID_INDEX) /*if ptr is not found in the auto_managed_empty_blocks BUFFER*/
				 	BUFpush(&cursor); 
				BUFbind(previous_binded_buffer); 
			}
			return true; 
		}
	}
	return false; 
}
function_signature(bool, BUFremove, void* object, bool (*comparer)(void*, void*))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", false);
	uint8_t* cursor = binded_buffer->bytes; 
	for(int i = 0; i < binded_buffer->element_count; i++, cursor += binded_buffer->element_size)
	{
		if(comparer(cursor, object))
		{
			memcpy(cursor, cursor + binded_buffer->element_size, (binded_buffer->element_count - i - 1) * binded_buffer->element_size); 
			memset(BUFpeek_ptr(), 0, binded_buffer->element_size); 
			--(binded_buffer->element_count); 
			return true; 
		}
	}
	return false; 
}
function_signature_void(void, BUFfit)
{
	//TODO
	//Replace this with BUFresize(binded_buffer->element_count)
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	binded_buffer->bytes = (uint8_t*)realloc(binded_buffer->bytes , binded_buffer->element_count * binded_buffer->element_size); 
	if(binded_buffer->element_count != 0)
	GOOD_ASSERT((binded_buffer->bytes != NULL), "Memory Allocation Failure Exception");
	binded_buffer->capacity = binded_buffer->element_count;
}
function_signature_void(void*, BUFpeek_ptr)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", NULL);
	if(binded_buffer->element_count == 0) return NULL; 
	return binded_buffer->bytes + (binded_buffer->element_count - 1) * binded_buffer->element_size;
}
function_signature(void, BUFpeek, void* out_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	memcpy(out_value , binded_buffer->bytes + (binded_buffer->element_count - 1) * binded_buffer->element_size , binded_buffer->element_size) ;   
}
function_signature(void, BUFpop, void* out_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");
	if(binded_buffer->element_count == 0) return;
	--(binded_buffer->element_count);
	if(out_value != NULL)
		memcpy(out_value , binded_buffer->bytes + binded_buffer->element_count * binded_buffer->element_size , binded_buffer->element_size) ; 
}

#if BUFFER_LIB_VERSION == 0x0013
function_signature(buf_count_t, BUFfind_index_of, void* value, bool (*comparer)(void*, void*))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
	uint8_t* cursor = binded_buffer->bytes; 
	for(int i = 0; i < BUFget_element_count(); i++, cursor += BUFget_element_size())
		if(comparer(value,cursor))
			return i; 
	return BUF_INVALID_INDEX;
}
#else
function_signature(buf_ucount_t, BUFfind_index_of, void* value, bool (*comparer)(void*, void*))
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception", BUF_INVALID_INDEX);
	uint8_t* cursor = binded_buffer->bytes; 
	for(int i = 0; i < BUFget_element_count(); i++, cursor += BUFget_element_size())
		if(comparer(value,cursor))
			return i; 
	return BUF_INVALID_INDEX;
}
#endif

//TODO: 
//Modified in version 1.6
function_signature(void, BUFpush, void* in_value)
{
	GOOD_ASSERT(binded_buffer != NULL, "Binded Buffer Is NULL Exception");

	buf_ucount_t new_capacity = (BUFget_capacity() == 0) ? 1 : BUFget_capacity(); 
	BUFset_element_count(BUFget_element_count() + 1);
	while(new_capacity < BUFget_element_count())
		new_capacity *= 2;
	BUFresize(new_capacity);
	BUFset_at(BUFget_element_count() - 1, in_value);
}