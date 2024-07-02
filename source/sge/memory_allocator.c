/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: memory_allocator.c is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

#include <sge/memory_allocator.h>
#include <sge/alloc.h>
#include <sge/string_builder.h>
#include <sge/assert.h>

#include <disk_manager/file_writer.h>

#include <math.h>
#include <stdlib.h>
#include <inttypes.h>

#define ALLOCATION_FLAG_NO_ALIGN_RESTRICTION U32_MAX
#define _1_MEGA_BYTES (1048576)

#define debug_assert_aligned_memory(ptr, align) \
	debug_assert__(((align == U32_MAX) || ((align != U32_MAX) && ((CAST_TO(u64, ptr) % align) == 0))),\
		"unaligned memory address %p is returned by %s while the alignment requested was %u",\
		ptr,\
		(align == ALLOCATION_FLAG_NO_ALIGN_RESTRICTION) ? "stdlib::realloc" : "__aligned_realloc",\
		align);
/*
static void* __alloc(u32 size)
{
	void* ptr = malloc(size + sizeof(void*));
	DREF_TO(u64, ptr) = 0ULL;
	return ptr + sizeof(void*);
}

static void* __realloc(void* old_ptr, u32 size)
{
	void* ptr = realloc(CAST_TO(void*, DREF_TO(u64, old_ptr)), size + sizeof(void*));
	DREF_TO(u64, ptr) = 0ULL;
	return ptr + sizeof(void*);
}

static u32 __log2u32(u32 n)
{
	// make sure it is 64 bit or less machine
	_debug_assert__(sizeof(void*) <= 8);
	switch(n)
	{
		case 1: return 0;
		case 2: return 1;
		case 4: return 2;
		case 8: return 3;
		default:
			debug_log_warning("[Performance warning] log of %u is being calculated using stdlib::math::log2()", n);
			return CAST_TO(u32, log2(n));
	}
}

static u64 get_align_mask(u32 align)
{
	u32 bit_shift = __log2u32(align);
	return ((~0ULL) >> bit_shift) << bit_shift;
}

static void* get_aligned_ptr(void* ptr, u32 align)
{
	DREF_TO(u64, ptr) = CAST_TO(u64, ptr);
	ptr += sizeof(void*);
	return CAST_TO(void*, CAST_TO(u64, (ptr + align - 1)) & get_align_mask(align));
}

static void* __aligned_alloc(u32 size, u32 align)
{
	return get_aligned_ptr(malloc(size + sizeof(void*) + align - 1), align);
}

static void* __aligned_realloc(void* old_ptr, u32 old_size, u32 old_align, u32 size, u32 align)
{
	return get_aligned_ptr(realloc(CAST_TO(void*, DREF_TO(u64, old_ptr)), size + sizeof(void*) + align - 1), align);
}*/

static allocate_result_t stdlib_malloc(u32 size, u32 align, void* user_data)
{
	if((size == 0) || (align == 0))
		return (allocate_result_t) { ALLOCATE_RESULT_INVALID_REQUEST, NULL };

	void* ptr = NULL;
	if(align == ALLOCATION_FLAG_NO_ALIGN_RESTRICTION)
		ptr = heap_alloc(size);
	else
		ptr = heap_aligned_alloc(size, align);

	debug_assert_aligned_memory(ptr, align);

	if(ptr == NULL)
		return (allocate_result_t) { ALLOCATE_RESULT_OUT_OF_MEMORY, NULL };

	return (allocate_result_t) { ALLOCATE_RESULT_SUCCESS, ptr };
}

static allocate_result_t stdlib_realloc(void* old_ptr, u32 old_size, u32 old_align, u32 size, u32 align, void* user_data)
{
	if((size == 0) || (align == 0))
		return (allocate_result_t) { ALLOCATE_RESULT_INVALID_REQUEST, NULL };

	void* ptr = NULL;
	if(align == ALLOCATION_FLAG_NO_ALIGN_RESTRICTION)
		ptr =  heap_realloc(old_ptr, size);
	else
		ptr =  heap_aligned_realloc(old_ptr, size, align);
	
	debug_assert_aligned_memory(ptr, align);

	if(ptr == NULL)
		return (allocate_result_t) { ALLOCATE_RESULT_OUT_OF_MEMORY, NULL };

	return (allocate_result_t) { ALLOCATE_RESULT_SUCCESS, ptr };
}

static void stdlib_free(void* ptr, bool was_aligned, void* user_data)
{
	if(was_aligned)
		heap_aligned_free(ptr);
	else
		heap_free(ptr);
}

SGE_API memory_allocator_t* memory_allocator_create(const memory_allocator_create_info_t* create_info)
{
	memory_allocator_t* allocator = heap_new(memory_allocator_t);
	memzero(allocator, memory_allocator_t);
	allocator->allocation_map = dictionary_create(void*, memory_allocation_t, _1_MEGA_BYTES, dictionary_key_comparer_ptr);
	allocator->allocate = (create_info->allocate == NULL) ? stdlib_malloc : create_info->allocate;
	allocator->reallocate = (create_info->reallocate == NULL) ? stdlib_realloc : create_info->reallocate;
	allocator->deallocate = (create_info->deallocate == NULL) ? stdlib_free : create_info->deallocate;
	if(create_info->prev_footprint != NULL)
	{
		allocator->footprint.curr_usage = create_info->prev_footprint->curr_usage;
		allocator->footprint.peek_usage = create_info->prev_footprint->peek_usage;
	}
	allocator->footprint.curr_usage = heap_sizeof(memory_allocator_t);
	return allocator;
}

SGE_API void memory_allocator_destroy(memory_allocator_t* allocator)
{
	dictionary_free(&allocator->allocation_map);
	heap_free(allocator);
}

SGE_API void* __memory_allocator_alloc(memory_allocator_t* allocator, __memory_allocation_debug_info_t debug_info, u32 size)
{
	return __memory_allocator_aligned_alloc(allocator, debug_info, size, ALLOCATION_FLAG_NO_ALIGN_RESTRICTION);
}

SGE_API void* __memory_allocator_realloc(memory_allocator_t* allocator, void* old_ptr, __memory_allocation_debug_info_t debug_info, u32 size)
{
	return __memory_allocator_aligned_realloc(allocator, old_ptr, debug_info, size, ALLOCATION_FLAG_NO_ALIGN_RESTRICTION);
}

static void update_peek_usage_if_more(memory_allocator_t* allocator)
{
	/* we need to measure peek (maximum) usage at a particular time, so update the peek_usage if notice more usage */
	if(allocator->footprint.peek_usage < allocator->footprint.curr_usage)
		allocator->footprint.peek_usage = allocator->footprint.curr_usage;
}

SGE_API void* __memory_allocator_aligned_alloc(memory_allocator_t* allocator, __memory_allocation_debug_info_t debug_info, u32 size,  u32 align)
{
	_debug_assert__(size != 0);
	allocate_result_t result = allocator->allocate(size, align, allocator->user_data);
	if(result.flags != ALLOCATE_RESULT_SUCCESS)
	{
		debug_log_warning("Failed to allocate for size: "PRIu32", and align: "PRIu32, size, align);
		return NULL;
	}

	memory_allocation_t allocation =
	{
		.debug_info = debug_info,
		.size = size,
		.align = align,
		.ptr = result.ptr
	};
	dictionary_add(&allocator->allocation_map, &result.ptr, &allocation);
	allocator->footprint.curr_usage += size;

	update_peek_usage_if_more(allocator);

	IF_DEBUG(allocator->alloc_counter++);
	return result.ptr;
}

SGE_API void* __memory_allocator_aligned_realloc(memory_allocator_t* allocator, void* old_ptr,  __memory_allocation_debug_info_t debug_info, u32 size,  u32 align)
{
	_debug_assert__(size != 0);
	memory_allocation_t* old_alloc = NULL;

	/* if old_ptr is not NULL and old allocation exists then find the previous allocation */
	if(old_ptr != NULL)
	{
		buf_ucount_t index = dictionary_find_index_of(&allocator->allocation_map, &old_ptr);
		debug_assert__(index != BUF_INVALID_INDEX, "You're trying to realloc a memory block not been allocated with memory_allocator");
		old_alloc = CAST_TO(memory_allocation_t*, dictionary_get_value_ptr_at(&allocator->allocation_map, index));
	}

	allocate_result_t result = allocator->reallocate(old_ptr, 
													(old_alloc == NULL) ? 0 : old_alloc->size, 
													(old_alloc == NULL) ? 0 : old_alloc->align, size, align, allocator->user_data);

	if(result.flags != ALLOCATE_RESULT_SUCCESS)
	{
		debug_log_warning("Failed to reallocate for size: "PRIu32", and align: "PRIu32, size, align);
		return NULL;
	}

	if(old_alloc != NULL)
	{
		/* NOTE: we're dealing with unsigned integer, so we can't just subtract old_size from new_size, that's because negative number can't be represented
	 	* using unsigned integer */
		_debug_assert__(allocator->footprint.curr_usage >= old_alloc->size);
		allocator->footprint.curr_usage -= old_alloc->size;
	}

	if((old_alloc != NULL) && (old_ptr == result.ptr))
	{
		old_alloc->debug_info = debug_info;
		old_alloc->size = size;
		old_alloc->align = align;
	}
	else /* the new pointer is not equal to the old pointer, meaning the memory block has been shifted to increase or decrease its size */
	{
		/* remove the old allocation if exists */
		if(old_alloc != NULL)
		{
			dictionary_remove(&allocator->allocation_map, &old_ptr);
			IF_DEBUG(allocator->alloc_counter--);
		}

		/* add the new memory block with new address */
		memory_allocation_t allocation = 
		{
			.debug_info = debug_info,
			.size = size,
			.align = align,
			.ptr = result.ptr
		};
		dictionary_add(&allocator->allocation_map, &result.ptr, &allocation);
		IF_DEBUG(allocator->alloc_counter++);
	}

	allocator->footprint.curr_usage += size;
	update_peek_usage_if_more(allocator);

	return result.ptr;
}

static memory_allocation_t* memory_allocator_try_get_allocation(memory_allocator_t* allocator, void* ptr)
{
	memory_allocation_t* allocation;
	if(dictionary_try_get_value_ptr(&allocator->allocation_map, &ptr, CAST_TO(void**, &allocation)))
		return allocation;
	return NULL;
}

SGE_API bool memory_allocator_is_allocation_exists(memory_allocator_t* allocator, void* ptr)
{
	return dictionary_find_index_of(&allocator->allocation_map, &ptr) != BUF_INVALID_INDEX;
}

SGE_API void __memory_allocator_dealloc(memory_allocator_t* allocator, void* ptr)
{
	_debug_assert__(ptr != NULL);

	AUTO allocation = memory_allocator_try_get_allocation(allocator, ptr);
	debug_assert__(allocation != NULL, "You're trying to free an invalid pointer, no allocation info exist for %p", ptr);

	_debug_assert__(allocator->footprint.curr_usage >= allocation->size);
	allocator->footprint.curr_usage -= allocation->size;
	/* let the NULL ptr handled by the deallocate function pointer (user might have an implementation for it) */
	bool was_aligned = allocation->align != ALLOCATION_FLAG_NO_ALIGN_RESTRICTION;
	allocator->deallocate(ptr, was_aligned, allocator->user_data);
	dictionary_remove(&allocator->allocation_map, &ptr);

	IF_DEBUG(allocator->alloc_counter--);
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
	/* no allocations were made */
	if(count == 0)
		return NULL;
	memory_allocation_debug_node_t* nodes = heap_newv(memory_allocation_debug_node_t, count);
	safe_memzerov(nodes, memory_allocation_debug_node_t, count);

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
	root->children = (root->child_count > 0) ? CAST_TO(memory_allocation_debug_node_t**, buf_get_ptr(&unref_nodes)) : NULL;
	root->parents = buf_new(memory_allocation_debug_node_t*);

	memory_allocation_tree_t* tree = heap_new(memory_allocation_tree_t);
	tree->nodes = nodes;
	tree->root = root;
	return tree;;
}

static void memory_allocation_footprint_to_string(const memory_allocation_footprint_t* footprint, string_builder_t* builder);
static void memory_allocation_tree_to_string(memory_allocation_tree_t* tree, string_builder_t* builder);

static void string_builder_write_to_file_and_destroy(string_builder_t* builder, const char* const file_path);

SGE_API void memory_allocator_serialize_to_file(memory_allocator_t* allocator, const char* const file_path)
{
	string_builder_t* builder = string_builder_create(NULL, 512);
	IF_DEBUG( string_builder_append(builder, "LEAKED MEMORY BLOCK COUNT: %"PRIu64"\n", allocator->alloc_counter) );
	/* serialize the memory footprint */
	const memory_allocation_footprint_t* footprint = memory_allocator_get_footprint(allocator);
	memory_allocation_footprint_to_string(footprint, builder);
	/* serialize the memory allocation tree */
	memory_allocation_tree_t* tree = memory_allocator_build_allocation_tree(allocator);
	memory_allocation_tree_to_string(tree, builder);
	memory_allocation_tree_destroy(tree);
	/* write the string to a file */
	string_builder_write_to_file_and_destroy(builder, file_path);
}

static INLINE memory_allocation_tree_t* build_allocation_tree(memory_allocation_map_t* allocation_map)
{
	return build_tree(resolve_references(allocation_map), dictionary_get_count(allocation_map));
}

SGE_API memory_allocation_tree_t* memory_allocator_build_allocation_tree(memory_allocator_t* allocator)
{
	return build_allocation_tree(&allocator->allocation_map);
}

SGE_API void memory_allocation_tree_destroy(memory_allocation_tree_t* tree)
{
	if(tree->root->child_count > 0)
		free(tree->root->children);
	if(tree->nodes != NULL)
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
		string_builder_append(builder, "line: %u\n", node->allocation->debug_info.line);
		string_builder_append(builder, "function: %s\n", node->allocation->debug_info.function_str);
		string_builder_append(builder, "file: %s\n", node->allocation->debug_info.file_str);
		string_builder_append(builder, "size: %u\n", node->allocation->size);
		if(node->allocation->align != ALLOCATION_FLAG_NO_ALIGN_RESTRICTION)
			string_builder_append(builder, "align: %u\n", node->allocation->align);
		else
			string_builder_append(builder, "align: ALLOCATION_FLAG_NO_ALIGN_RESTRICTION\n");
		string_builder_append(builder, "address: %p\n", node->allocation->ptr);
	}
	else
	{
		string_builder_append(builder, "type_id: UNDEFINED\n");
		string_builder_append(builder, "type_str: UNDEFINED\n");
		string_builder_append(builder, "line: UNDEFINED\n");
		string_builder_append(builder, "function: UNDEFINED\n");
		string_builder_append(builder, "file: UNDEFINED\n");
		string_builder_append(builder, "size: 0\n");
		string_builder_append(builder, "align: ALLOCATION_FLAG_NO_ALIGN_RESTRICTION\n");
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

static void memory_allocation_tree_to_string(memory_allocation_tree_t* tree, string_builder_t* builder)
{
	u32 size = memory_allocation_tree_calculate_size(tree->root);

	string_builder_append(builder, "TOTAL BYTES ALLOCATED: %u (%.1f KB)\n", size, CAST_TO(float, size) / 1024);
	string_builder_append(builder, "ALLOCATION TREE:\n\n");
	memory_allocation_debug_node_to_string(tree->root, builder);
}

static void string_builder_write_to_file_and_destroy(string_builder_t* builder, const char* const file_path)
{
	write_text_to_file(file_path, string_builder_get_str(builder));
	string_builder_append_null(builder);
	string_builder_destroy(builder);
}

SGE_API void memory_allocation_tree_serialize_to_file(memory_allocation_tree_t* tree, const char* const file_path)
{
	string_builder_t* builder = string_builder_create(NULL, 512);
	memory_allocation_tree_to_string(tree, builder);
	string_builder_write_to_file_and_destroy(builder, file_path);
}

SGE_API memory_allocation_footprint_t* memory_allocator_get_footprint(memory_allocator_t* allocator)
{
	return &allocator->footprint;
}

static void memory_allocation_footprint_to_string(const memory_allocation_footprint_t* footprint, string_builder_t* builder)
{
	string_builder_append(builder, "CURR MEMORY USAGE: %"PRIu32" (%.1f KB)\n", footprint->curr_usage, CAST_TO(f32, footprint->curr_usage) / 1024);
	string_builder_append(builder, "PEEK MEMORY USAGE: %"PRIu32" (%.1f KB)\n", footprint->peek_usage, CAST_TO(f32, footprint->peek_usage) / 1024);
}

SGE_API void memory_allocation_footprint_serialize_to_file(const memory_allocation_footprint_t* footprint, const char* const file_path)
{
	string_builder_t* builder = string_builder_create(NULL, 128);
	memory_allocation_footprint_to_string(footprint, builder);
	string_builder_write_to_file_and_destroy(builder, file_path);
}


void* memory_allocator_call_malloc(buf_ucount_t size, void* user_data)
{
	return memory_allocator_alloc(CAST_TO(memory_allocator_t*, user_data), MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, size);
}

void memory_allocator_call_free(void* ptr, void* user_data)
{
	memory_allocator_dealloc(CAST_TO(memory_allocator_t*, user_data), ptr);
}

void* memory_allocator_call_realloc(void* old_ptr, buf_ucount_t size, void* user_data)
{
	return memory_allocator_realloc(CAST_TO(memory_allocator_t*, user_data), old_ptr, MEMORY_ALLOCATION_TYPE_IN_MEMORY_BUFFER, size);
}
