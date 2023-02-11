#include <renderer/memory_allocator.h>
#include <renderer/alloc.h>
#include <renderer/string_builder.h>
#include <renderer/assert.h>

#include <disk_manager/file_writer.h>

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
	allocator->deallocate(ptr);
	dictionary_remove(&allocator->allocation_map, &ptr);
}

enum
{
	/* if this node is visited, useful when building the allocation tree */
	ALREADY_VISITED = BIT64(0),
	/* if this node is every referenced other than self referenced */
	OTHER_REFERENCED = BIT64(1),
	/* if this node references to itself */
	SELF_REFERENCED = BIT64(2)
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
	return CAST_TO(u64, node->allocation->ptr) == DREF_TO(u64, user_data);
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

static bool is_cyclic_reference(memory_allocation_debug_node_t* node, memory_allocation_debug_node_t* reference)
{
	if(node == NULL)
		return false;
	else if(node->allocation->ptr == reference->allocation->ptr)
		return true;

	u32 parent_count = buf_get_element_count(&node->parents);
	memory_allocation_debug_node_t** parents = buf_get_ptr(&node->parents);

	for(u32 i = 0; i < parent_count; i++)
		if(is_cyclic_reference(parents[i], reference))
			return true;

	return false;
}

static memory_allocation_debug_node_t* resolve_references(memory_allocation_map_t* allocation_map)
{
	u32 count = dictionary_get_count(allocation_map);
	memory_allocation_debug_node_t* nodes = heap_newv(memory_allocation_debug_node_t, count);
	memzerov(nodes, memory_allocation_debug_node_t, count);

	for(u32 i = 0; i < count; i++)
	{
		nodes[i].allocation = CAST_TO(memory_allocation_t*, dictionary_get_value_ptr_at(allocation_map, i));
		nodes[i].__internal_size = U32_MAX;
		nodes[i].parents = buf_new(memory_allocation_debug_node_t*);
	}

	_debug_assert__(sizeof(void*) == U64_SIZE);
	const u32 num_bits = calculate_bits_required(U64_SIZE);

	/* for now we are building our project in 64-bit machine */
	_debug_assert__(num_bits == 3);

	memory_allocation_debug_node_t* node;
	while((node = find_if(nodes, count, condition_unvisited, NULL)) != NULL)
	{
		BUFFER references = buf_new(memory_allocation_debug_node_t*);
		u32 size = (node->allocation->size >> num_bits) << num_bits;
		for(u32 dword_offset = 0; dword_offset < size; dword_offset += U64_SIZE)
		{
			memory_allocation_debug_node_t* referenced_node = find_if(nodes, count, condition_address_matches, node->allocation->ptr + dword_offset);
			if(referenced_node != NULL)
			{
				/* if there is a self reference */
				if(referenced_node == node)
				{
					node->__internal_debug_flags |= SELF_REFERENCED;
					node->__internal_debug_flags &= ~OTHER_REFERENCED;
				}
				/* if there is a cyclic reference or the node is already referenced by other nodes */
				else if(is_cyclic_reference(node, referenced_node) || (referenced_node->__internal_debug_flags & OTHER_REFERENCED))
				{
					/* more debug information */
				}
				/* only add this node in the list of references when the reference is not a self reference
				 * as adding self reference to the list of references leads to a cycle, hence infinite loop in traversal. */
				else
				{
					/* now this node has been referenced */
					referenced_node->__internal_debug_flags |= OTHER_REFERENCED;

					/* add the referenced node into the list of references */
					buf_push(&references, &referenced_node);

					/* add the node into the list of the parents of the referenced node */
					buf_push(&referenced_node->parents, &node);
				}
			}
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
		if((nodes[i].__internal_debug_flags & OTHER_REFERENCED) == 0)
		{
			memory_allocation_debug_node_t* ptr = nodes + i;
			buf_push(&unref_nodes, &ptr);
		}

	memory_allocation_debug_node_t* root = heap_new(memory_allocation_debug_node_t);
	memzero(root, memory_allocation_debug_node_t);

	root->child_count = buf_get_element_count(&unref_nodes);
	root->children = CAST_TO(memory_allocation_debug_node_t**, buf_get_ptr(&unref_nodes));
	root->parents = buf_new(memory_allocation_debug_node_t*);

	memory_allocation_tree_t* tree = heap_new(memory_allocation_tree_t);
	tree->nodes = nodes;
	tree->root = root;
	return tree;;
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
	heap_free(tree->nodes);
	heap_free(tree->root);
	heap_free(tree);
}

static u32 memory_allocation_tree_calculate_size(memory_allocation_debug_node_t* node)
{
	/* calculate the sizes of the children recursively */
	u32 referenced_sized = 0;
	for(u32 i = 0; i < node->child_count; i++)
	{
		AUTO child = node->children[i];
		/* if the size for the child has already been calculated then don't calculate again */
		referenced_sized += (child->__internal_size != U32_MAX) ? child->__internal_size :  memory_allocation_tree_calculate_size(node->children[i]);
	}

	/* the root of the tree doesn't have allocation */
	return node->__internal_size = (((node->allocation == NULL) ? 0 : node->allocation->size) + referenced_sized);
}

static void memory_allocation_debug_node_to_string(const memory_allocation_debug_node_t* node, string_builder_t* builder)
{
	string_builder_append(builder, "ALLOC\n");
	string_builder_append(builder, "{\n");
	string_builder_increment_indentation(builder);
	if(node->allocation != NULL)
	{
		string_builder_append(builder, "type_id: %u\n", node->allocation->debug_info.allocation_type);
		string_builder_append(builder, "type_str: %s\n", node->allocation->debug_info.allocation_type_str);
		string_builder_append(builder, "size: %u\n", node->allocation->size);
		string_builder_append(builder, "address: %p\n", node->allocation->ptr);
	}
	else
	{
		string_builder_append(builder, "type_id: UNDEFINED\n");
		string_builder_append(builder, "type_str: UNDEFINED\n");
		string_builder_append(builder, "size: 0\n");
		string_builder_append(builder, "address: UNDEFINED\n");
	}
	string_builder_append(builder, "ref_count: %u\n", node->child_count);
	string_builder_append(builder, "ref_size: %u\n", node->__internal_size - ((node->allocation == NULL) ? 0 : node->allocation->size));
	string_builder_append(builder, "other referenced: %s\n", (node->__internal_debug_flags & OTHER_REFERENCED) ? "true" : "false");
	string_builder_append(builder, "self referenced: %s\n", (node->__internal_debug_flags & SELF_REFERENCED) ? "true" : "false");

	string_builder_increment_indentation(builder);
	for(u32 i = 0; i < node->child_count; i++)
	{
		string_builder_append(builder, "\n");
		memory_allocation_debug_node_to_string(node->children[i], builder);
	}
	string_builder_decrement_indentation(builder);

	string_builder_decrement_indentation(builder);
	string_builder_append(builder, "}\n");
}

RENDERER_API void memory_allocation_tree_serialize_to_file(memory_allocation_tree_t* tree, const char* const file_path)
{
	string_builder_t* builder = string_builder_create(NULL, 512);

	u32 size = memory_allocation_tree_calculate_size(tree->root);

	string_builder_append(builder, "TOTAL BYTES ALLOCATED: %u (%.1f KB)\n", size, CAST_TO(float, size) / 1024);
	string_builder_append(builder, "ALLOCATION TREE:\n\n");
	memory_allocation_debug_node_to_string(tree->root, builder);

	string_builder_append_null(builder);
	write_text_to_file(file_path, string_builder_get_str(builder));

	string_builder_destroy(builder);
}

RENDERER_API memory_allocation_footprint_t* memory_allocator_get_footprint(memory_allocator_t* allocator)
{
	return NULL;
}

RENDERER_API void memory_allocation_footprint_serialize_to_file(const memory_allocation_footprint_t* footprint, const char* const file_path)
{

}
