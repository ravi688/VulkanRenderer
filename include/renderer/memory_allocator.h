#pragma once

#include <renderer/defines.h>

#include <renderer/memory_allocation_type.h>

/* holds the debug info for each memory allocation */
typedef struct __memory_allocation_debug_info_t
{
    /* type of the allocation */
    memory_allocation_type_t allocation_type;
    /* string, holds the debug name for the allocation type */
    const char* allocation_type_str;
} __memory_allocation_debug_info_t;

/* helper macro to create inlined __memory_allocation_debug_info_t object */
#define __memory_allocation_debug_info(allocation_type) (__memory_allocation_debug_info_t) { allocation_type, #allocation_type }

/* flags that can be set at the time of the create of memory_allocator_t object */
typedef enum memory_allocation_bit_flags_t
{
	/* set this if you want the allocator to preallocate memory at the startup,
	 * the preallocation details come from memory_allocation_footprint_t object.
	 */
	MEMORY_ALLOCATION_PREALLOCATE_FROM_FOOTPRINT_BIT = BIT32(0),
	/* set this if you want the allocator to collect allocation data and pattern,
	 * and later save them in the disk in the form of memory_allocation_footprint_t object.
	 */
	MEMORY_ALLOCATION_PRESIST_FOOTPRINT_ANALYSIS_BIT = BIT32(1)
} memory_allocation_bit_flags_t;

/* stores allocation data and patterns (useful for saving in the disk for the first time and later preallocating) */
typedef struct memory_allocation_footprint_t
{

} memory_allocation_footprint_t;

/* stores the memory allocation informatin including pointer to the memory chunk */
typedef struct memory_allocation_t
{
    /* debug information for the memory allocation */
    __memory_allocation_debug_info_t debug_info;
    /* size of the memory allocation (in bytes) */
    u32 size;
    /* pointer to the memory allocation */
    void* ptr;
} memory_allocation_t;

/* stores debug information (useful for building log files) for a memory allocation */
typedef struct memory_allocation_debug_node_t memory_allocation_debug_node_t;
typedef struct memory_allocation_debug_node_t
{
    /* [internal] size of the memory allocation including the size of the referenced allocations recursively */
    u32 __internal_size;
    /* [internal] debug flags */
    u64 __internal_debug_flags;
    /* pointer to the memory allocation object to which this node corresponds */
    memory_allocation_t* allocation;
    /* number of memory_allocation_debug_node_t objects which are directly reachable from this memory allocation object */
    u32 child_count;
    /* pointer to the list of pointers to the memory_allocation_debug_node_t objects which are directly reachable from this memory allocation object */
    memory_allocation_debug_node_t** children;
} memory_allocation_debug_node_t;

typedef struct memory_allocation_tree_t
{
    /* root node of the tree, it is separately allocated */
    memory_allocation_debug_node_t* root;

    /* list of all the nodes present in the tree (except the root)
     * the nodes are contiguously allocated, and hence it is required to preserve the ptr
     * to later free it, memory_allocation_tree_destroy() */
    memory_allocation_debug_node_t* nodes;
} memory_allocation_tree_t;

/* enumeration for the possible cases when allocating memory through the user provided callbacks */
typedef enum allocate_result_flags_t
{
	/* allocation is success */
	ALLOCATE_RESULT_SUCCESS,
	/* ran out of memory */
	ALLOCATE_RESULT_OUT_OF_MEMORY,
	/* invalid request such as incorrect alignemnt and size */
	ALLOCATE_RESULT_INVALID_REQUEST
} allocate_result_flags_t;

/* result object which shall be returned by user provided 'allocate' and 'deallocate' functions */
typedef struct allocate_result_t
{
	/* status of the allocation requestion (failed or success) */
	allocate_result_flags_t flags;
	/* holds a valid address value if success, otherwise a NULL value */
	void* ptr;
} allocate_result_t;

/* create info struct to be passed to one of the constructors of memory_allocator_t object */
typedef struct memory_allocator_create_info_t
{
	/* flags to customize the behaviour of the allocator */
	memory_allocation_bit_flags_t flags;
	/* footprint to be loaded from the disk to preallocate the memory blocks and objects */
	memory_allocation_footprint_t* prev_footprint;

	/* NOTE: allocate and deallocate both can't be NULL at the same time due to the fact that 
	   the memory chunk allocated from a heap must be deallocted from the same heap and the same technique. */

	/* user provided malloc function, used to allocate memory chunks in the host memory,
	 * if NULL then the memory allocator will use the stdlib.malloc */
	allocate_result_t (*allocate)(u32 size, u32 align);
	/* user provided free function, used to deallocate memory chunks from the host memory, 
	 * if NULL then the memory allocator will use the stdlib.free */
	void (*deallocate)(void* ptr);

} memory_allocator_create_info_t;

#include <renderer/dictionary.h>

/* memory_allocation_map_t holds list of memory allocations and maps one-to-one each allocation with its memory address */
typedef dictionary_t memory_allocation_map_t;

/* memory allocator object to be used every time a user calls memory_allocator_* functions */
typedef struct memory_allocator_t
{
    /* one-to-one mapping between each memory allocation and memory address of that allocation */
    memory_allocation_map_t allocation_map;
    /* user provided malloc function, used to allocate memory chunks in the host memory */
    allocate_result_t (*allocate)(u32 size, u32 align);
    /* user provided free function, used to deallocate memory chunks from the host memory */ 
    void (*deallocate)(void* ptr);
} memory_allocator_t;


BEGIN_CPP_COMPATIBLE

/* constructor and destructor */
RENDERER_API memory_allocator_t* memory_allocator_create(const memory_allocator_create_info_t* create_info);
RENDERER_API void memory_allocator_destroy(memory_allocator_t* allocator);

/* allocates memory chunk and attaches appropriate debug information to that chunk */
#define memory_allocator_alloc_obj_array(allocator, allocation_type, type, count) CAST_TO(type*, memory_allocator_alloc(allocator, allocation_type, sizeof(type) * (count)))
#define memory_allocator_alloc_obj(allocator, allocation_type, type) CAST_TO(type*, memory_allocator_alloc(allocator, allocation_type, sizeof(type)))
#define memory_allocator_alloc(allocator, allocation_type, size) __memory_allocator_alloc(allocator, __memory_allocation_debug_info(allocation_type), size)
RENDERER_API void* __memory_allocator_alloc(memory_allocator_t* allocator, __memory_allocation_debug_info_t debug_info, u32 size);
/* deallocates memory chunk */
RENDERER_API void memory_allocator_dealloc(memory_allocator_t* allocator, void* ptr);

/* builds a memory allocation tree, usually it is used for debug logging purpose */
RENDERER_API memory_allocation_tree_t* memory_allocator_build_allocation_tree(memory_allocator_t* allocator);
/* destroys the memory allocation tree (frees up the allocated memory) */
RENDERER_API void memory_allocation_tree_destroy(memory_allocation_tree_t* tree);
/* serializes the memory_allocation_tree_t object into a file (human readable text file) */
RENDERER_API void memory_allocation_tree_serialize_to_file(memory_allocation_tree_t* tree, const char* const file_path);

/* returns a valid memory footprint analysis if MEMORY_ALLOCATION_PRESIST_FOOTPRINT_ANALYSIS_BIT bit is set, otherwise NULL */
RENDERER_API memory_allocation_footprint_t* memory_allocator_get_footprint(memory_allocator_t* allocator);
/* serializes the memory_allocation_footprint_t object into a file (binary file) */
RENDERER_API void memory_allocation_footprint_serialize_to_file(const memory_allocation_footprint_t* footprint, const char* const file_path);

END_CPP_COMPATIBLE
