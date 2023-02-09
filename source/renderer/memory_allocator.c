#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/assert.h>

#include <math.h>
#include <stdlib.h>

static allocate_result_t stdlib_malloc(u32 size, u32 align)
{
	if((size == 0) || (align == 0))
		return (allocate_result_t) { ALLOCATE_RESULT_INVALID_REQUEST, NULL };

	void* ptr = malloc(size);
	if(ptr == NULL)
		return (allocate_result_t) { ALLOCATE_RESULT_OUT_OF_MEMORY, NULL };

	return (allocate_result_t) { ALLOCATE_RESULT_SUCCESS, ptr };
}

static void stdlib_free(void* ptr)
{
	free(ptr);
}

RENDERER_API memory_allocator_t* memory_allocator_create(const memory_allocator_create_info_t* create_info)
{
	memory_allocator_t* allocator = heap_new(memory_allocator_t);
	memzero(allocator, memory_allocator_t);
	allocator->allocation_map = dictionary_create(void*, memory_allocation_t, 1, dictionary_key_comparer_ptr);
	allocator->allocate = (create_info->allocate == NULL) ? stdlib_malloc : create_info->allocate;
	allocator->deallocate = (create_info->deallocate == NULL) ? stdlib_free : create_info->deallocate;
	return allocator;
}

RENDERER_API void memory_allocator_destroy(memory_allocator_t* allocator)
{
	dictionary_free(&allocator->allocation_map);
	heap_free(allocator);
}

RENDERER_API void* __memory_allocator_alloc(memory_allocator_t* allocator, __memory_allocation_debug_info_t debug_info, u32 size)
{
	allocate_result_t result = allocator->allocate(size, size);
	if(result.flags != ALLOCATE_RESULT_SUCCESS)
		return NULL;

	memory_allocation_t allocation =
	{
		.debug_info = debug_info,
		.size = size,
		.ptr = result.ptr
	};
	dictionary_add(&allocator->allocation_map, &allocation.ptr, &allocation);
	return allocation.ptr;
}

RENDERER_API void memory_allocator_dealloc(memory_allocator_t* allocator, void* ptr)
{
	dictionary_remove(&allocator->allocation_map, &ptr);
	allocator->deallocate(ptr);
}

enum
{
	ALREADY_VISITED = BIT64(0),
	EVER_REFERENCED = BIT64(1)
};

static memory_allocation_debug_node_t* find_if(memory_allocation_debug_node_t* nodes, u32 count, bool (*condition)(memory_allocation_debug_node_t* node, void* user_data), void* user_data)
{
	for(u32 i = 0; i < count; i++)
	{
		AUTO ptr = &nodes[i];
		if(condition(&nodes[i], user_data))
			return ptr;
	}
	return NULL;
}

static bool condition_unvisited(memory_allocation_debug_node_t* node, void* user_data)
{
	bool is_unvisited = (node->__internal_debug_flags & ALREADY_VISITED) == 0;
	node->__internal_debug_flags |= ALREADY_VISITED;
	return is_unvisited;
}

static bool condition_address_matches(memory_allocation_debug_node_t* node, void* user_data)
{
	bool matches = CAST_TO(u64, node->allocation->ptr) == DREF_TO(u64, user_data);
	if(matches)
		node->__internal_debug_flags |= EVER_REFERENCED;
	return matches;
}

static INLINE u32 calculate_bits_required(u32 max_number) { return ceil(log2(max_number)); }
const static u32 U64_SIZE = sizeof(u64);

static INLINE void memory_allocation_dump(memory_allocation_t* allocation)
{
	debug_log_info("debug_info { %u, %s }, size: %u, ptr: %u",
					allocation->debug_info.allocation_type,
					allocation->debug_info.allocation_type_str,
					allocation->size,
					allocation->ptr);
}

static memory_allocation_debug_node_t* resolve_references(memory_allocation_map_t* allocation_map)
{
	u32 count = dictionary_get_count(allocation_map);
	memory_allocation_debug_node_t* nodes = heap_newv(memory_allocation_debug_node_t, count);
	memzerov(nodes, memory_allocation_debug_node_t, count);

	for(u32 i = 0; i < count; i++)
		nodes[i].allocation = CAST_TO(memory_allocation_t*, dictionary_get_value_ptr_at(allocation_map, i));

	assert(sizeof(void*) == U64_SIZE);
	const u32 num_bits = calculate_bits_required(U64_SIZE);

	/* for now we are building our project in 64-bit machine */
	assert(num_bits == 3);

	memory_allocation_debug_node_t* node;
	while((node = find_if(nodes, count, condition_unvisited, NULL)) != NULL)
	{
		BUFFER references = buf_new(memory_allocation_debug_node_t*);
		u32 size = (node->allocation->size >> num_bits) << num_bits;
		for(u32 dword_offset = 0; dword_offset < size; dword_offset += U64_SIZE)
		{
			memory_allocation_debug_node_t* referenced_node = find_if(nodes, count, condition_address_matches, node->allocation->ptr + dword_offset);
			if(referenced_node != NULL)
				buf_push(&references, &referenced_node);
		}

		node->child_count = buf_get_element_count(&references);
		node->children = CAST_TO(memory_allocation_debug_node_t**, buf_get_ptr(&references));
	}
	return nodes;
}

static memory_allocation_tree_t* build_tree(memory_allocation_debug_node_t* nodes, u32 count)
{
	BUFFER unref_nodes = buf_new(memory_allocation_debug_node_t*);
	for(u32 i = 0; i < count; i++)
		if((nodes[i].__internal_debug_flags & EVER_REFERENCED) == 0)
		{
			memory_allocation_debug_node_t* ptr = nodes + i;
			buf_push(&unref_nodes, &ptr);
		}

	memory_allocation_debug_node_t* root = heap_new(memory_allocation_debug_node_t);
	memzero(root, memory_allocation_debug_node_t);

	root->__internal_debug_flags = CAST_TO(u64, nodes);
	root->child_count = buf_get_element_count(&unref_nodes);
	root->children = CAST_TO(memory_allocation_debug_node_t**, buf_get_ptr(&unref_nodes));
	return root;;
}

static INLINE memory_allocation_tree_t* build_allocation_tree(memory_allocation_map_t* allocation_map)
{
	return build_tree(resolve_references(allocation_map), dictionary_get_count(allocation_map));
}

RENDERER_API memory_allocation_tree_t* memory_allocator_build_allocation_tree(memory_allocator_t* allocator)
{
	return build_allocation_tree(&allocator->allocation_map);
}

RENDERER_API void memory_allocation_tree_destroy(memory_allocation_tree_t* tree)
{
	heap_free(CAST_TO(void*, tree->__internal_debug_flags));
	heap_free(tree);
}

RENDERER_API void memory_allocation_tree_serialize_to_file(const memory_allocation_tree_t* tree, const char* const file_path)
{
	static int depth = -1;

	depth++;

	for(u32 i = 0; i < tree->child_count; i++)
	{
		debug_log_info("[%d]", depth);
		memory_allocation_tree_serialize_to_file(tree->children[i], NULL);
	}

	depth--;
}

RENDERER_API memory_allocation_footprint_t* memory_allocator_get_footprint(memory_allocator_t* allocator)
{
	return NULL;
}

RENDERER_API void memory_allocation_footprint_serialize_to_file(const memory_allocation_footprint_t* footprint, const char* const file_path)
{

}
