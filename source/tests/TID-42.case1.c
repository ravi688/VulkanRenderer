
#include <renderer/tests/TID-42.case1.h>
#include <renderer/debug.h>

#include <renderer/memory_allocator.h>

#define SIZE(num_bytes) char bytes[num_bytes]

typedef struct MemoryObject3 MemoryObject3;
typedef struct MemoryObject2
{
	SIZE(64);
	MemoryObject3* obj3;
} MemoryObject2;

const static u32 MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_2 = (MEMORY_ALLOCATION_TYPE_MAX + __COUNTER__);

typedef struct MemoryObject1 MemoryObject1;
typedef struct MemoryObject4
{
	SIZE(128);
	MemoryObject2* obj2;
	MemoryObject1* obj1;
} MemoryObject4;

const static u32 MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_4 = (MEMORY_ALLOCATION_TYPE_MAX + __COUNTER__);

typedef struct MemoryObject3
{
	SIZE(512);
	MemoryObject4* obj4;
} MemoryObject3;

const static u32 MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_3 = (MEMORY_ALLOCATION_TYPE_MAX + __COUNTER__);

typedef struct MemoryObject1
{
	SIZE(1024);
	MemoryObject2* obj2;
	MemoryObject3* obj3;
	MemoryObject1* self_reference;
} MemoryObject1;

const static u32 MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_1 = (MEMORY_ALLOCATION_TYPE_MAX + __COUNTER__);

/*GetDebugFooter(MemoryObject1, obj1, obj3);

GetDebugFooter(MemoryObject1)
{
	u32* list = heap_newv(u32, end - start);
	for(u32 i = 0; i < (end - start); i++)
		list[i] = offsetof(MemoryObject1, __debug_ptr1__);
	return list;
}

offsetof(MemoryObject1, __debug_ptr1__), 
offsetof(MemoryObject1, __debug_ptr2__)*/

/*
	-----------------------------
		Object Allocation Tree
	-----------------------------

		MemoryObject1
			|
		--------------------
		|					|
	MemoryObject3 <----MemoryObject2
		| 					|
	MemoryObject4 		    |
		|___________________|

 */


TEST_DATA(TID_42_CASE_1)
{
	memory_allocator_t* allocator;
	MemoryObject1* obj1;
	MemoryObject2* obj2;
	MemoryObject3* obj3;
	MemoryObject4* obj4;
};

SETUP_TEST(TID_42_CASE_1);

TEST_ON_INITIALIZE(TID_42_CASE_1)
{
	memory_allocator_create_info_t create_info = 
	{

	};
	this->allocator = memory_allocator_create(&create_info);

	this->obj1 = CAST_TO(MemoryObject1*, memory_allocator_alloc(this->allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_1, sizeof(MemoryObject1)));
	this->obj2 = CAST_TO(MemoryObject2*, memory_allocator_alloc(this->allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_2, sizeof(MemoryObject2)));
	this->obj3 = CAST_TO(MemoryObject3*, memory_allocator_alloc(this->allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_3, sizeof(MemoryObject3)));
	this->obj4 = CAST_TO(MemoryObject4*, memory_allocator_alloc(this->allocator, MEMORY_ALLOCATION_TYPE_OBJ_TEST_MEMORY_OBJECT_4, sizeof(MemoryObject4)));

	this->obj1->obj2 = this->obj2;
	this->obj1->obj3 = this->obj3;
	this->obj3->obj4 = this->obj4;
	this->obj4->obj2 = this->obj2;
	this->obj2->obj3 = this->obj3;
	this->obj1->self_reference = this->obj1;

	memory_allocation_tree_t* tree = memory_allocator_build_allocation_tree(this->allocator);
	memory_allocation_tree_serialize_to_file(tree, "memory_allocation_tree.dump");
	memory_allocation_tree_destroy(tree);
}

TEST_ON_TERMINATE(TID_42_CASE_1)
{

	memory_allocation_footprint_serialize_to_file(memory_allocator_get_footprint(this->allocator), 
											"memory_allocation_footprint.mmfp");

	memory_allocator_dealloc(this->allocator, this->obj1);
	memory_allocator_dealloc(this->allocator, this->obj2);
	memory_allocator_dealloc(this->allocator, this->obj3);
	memory_allocator_dealloc(this->allocator, this->obj4);
	memory_allocator_destroy(this->allocator);
}

TEST_ON_RENDER(TID_42_CASE_1) { }

TEST_ON_UPDATE(TID_42_CASE_1) { }
