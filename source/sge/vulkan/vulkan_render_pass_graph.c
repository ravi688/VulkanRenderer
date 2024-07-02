/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: vulkan_render_pass_graph.c is a part of VulkanRenderer

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

#include <sge/internal/vulkan/vulkan_render_pass_graph.h>
#include <sge/memory_allocator.h>
#include <sge/internal/vulkan/vulkan_renderer.h>
#ifdef GLOBAL_DEBUG
#include <stdio.h>
#endif /* GLOBAL_DEBUG */


SGE_API vulkan_render_pass_graph_t* vulkan_render_pass_graph_new(memory_allocator_t* allocator)
{
    vulkan_render_pass_graph_t* graph = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_VK_RENDER_PASS_GRAPH, vulkan_render_pass_graph_t);
    memzero(graph, vulkan_render_pass_graph_t);
    VULKAN_OBJECT_INIT(graph, VULKAN_OBJECT_TYPE_RENDER_PASS_GRAPH, VULKAN_OBJECT_NATIONALITY_INTERNAL);
    return graph;
}

SGE_API vulkan_render_pass_graph_t* vulkan_render_pass_graph_create(vulkan_renderer_t* renderer)
{
    vulkan_render_pass_graph_t* graph = vulkan_render_pass_graph_new(renderer->allocator);
    vulkan_render_pass_graph_create_no_alloc(renderer, graph);
    return graph;
}

SGE_API void vulkan_render_pass_graph_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_graph_t OUT graph)
{
    VULKAN_OBJECT_MEMZERO(graph, vulkan_render_pass_graph_t);
    graph->renderer = renderer;
    graph->nodes = memory_allocator_buf_new(renderer->allocator, vulkan_render_pass_graph_node_t);
    graph->prev_pass_node_handle = VULKAN_RENDER_PASS_GRAPH_NODE_HANDLE_INVALID;
    graph->optimized_render_path = memory_allocator_buf_new(renderer->allocator, vulkan_render_pass_graph_node_handle_t);
    graph->staging_list = memory_allocator_buf_new(renderer->allocator, vulkan_render_pass_graph_node_handle_t);
}

SGE_API void vulkan_render_pass_graph_destroy(vulkan_render_pass_graph_t* graph)
{
    u32 node_count = buf_get_element_count(&graph->nodes);
    for(u32 i = 0; i < node_count; i++)
    {
        AUTO node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, i);
        buf_free(&node->next_pass_node_handles);
    }
    buf_clear(&graph->nodes, NULL);
    buf_clear(&graph->optimized_render_path, NULL);
}

SGE_API void vulkan_render_pass_graph_release_resources(vulkan_render_pass_graph_t* graph)
{
    buf_free(&graph->nodes);
    buf_free(&graph->optimized_render_path);
    buf_free(&graph->staging_list);
    if(VULKAN_OBJECT_IS_INTERNAL(graph))
        memory_allocator_dealloc(graph->renderer->allocator, graph);
}


SGE_API void vulkan_render_pass_graph_create_path(vulkan_render_pass_graph_t* graph)
{
    graph->prev_pass_node_handle = VULKAN_RENDER_PASS_GRAPH_NODE_HANDLE_INVALID;
}

static bool is_cycle(vulkan_render_pass_graph_t* graph, vulkan_render_pass_graph_node_t* node, vulkan_render_pass_graph_node_t* origin_node)
{
    if(node == origin_node)
        return true;
    u32 next_count = buf_get_element_count(&node->next_pass_node_handles);
    for(u32 i = 0; i < next_count; i++)
    {
        vulkan_render_pass_graph_node_handle_t next_handle;
        buf_get_at(&node->next_pass_node_handles, i, &next_handle);
        AUTO next_node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, next_handle);
        if(is_cycle(graph, next_node, origin_node))
            return true;
    }
    return false;
}

typedef struct node_comparer_data_t
{
    vulkan_render_pass_graph_t* graph;
    vulkan_render_pass_handle_t pass_handle;
} node_comparer_data_t;

static bool node_comparer(void* value, void* cursor)
{
    AUTO data = CAST_TO(node_comparer_data_t*, value);
    AUTO node = CAST_TO(vulkan_render_pass_graph_node_t*, cursor);
    /* if there is slot match and then make sure there is no cycle */
    if(node->pass_handle == data->pass_handle)
    {
        vulkan_render_pass_graph_t* graph = data->graph;
        return (graph->prev_pass_node_handle == VULKAN_RENDER_PASS_GRAPH_NODE_HANDLE_INVALID) ||
                !is_cycle(graph, node, buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, graph->prev_pass_node_handle));
    }

    return false;
}

static bool node_handle_comparer(void* value, void* cursor)
{
    return DREF_TO(vulkan_render_pass_graph_node_handle_t, value) == DREF_TO(vulkan_render_pass_graph_node_handle_t, cursor);
}

SGE_API void vulkan_render_pass_graph_add(vulkan_render_pass_graph_t* graph, vulkan_render_pass_handle_t pass_handle)
{
    graph->is_outdated = true;

    /* Create or Get vulkan_render_pass_graph_node_t */
    node_comparer_data_t data = { .graph = graph, .pass_handle = pass_handle };
    buf_ucount_t node_index = buf_find_index_of(&graph->nodes, (void*)(&data), node_comparer);
    vulkan_render_pass_graph_node_t* node;
    if(node_index == BUF_INVALID_INDEX)
    {
        /* Create a vulkan_render_pass_graph_node_t object */
        buf_push_pseudo(&graph->nodes, 1);
        node = CAST_TO(vulkan_render_pass_graph_node_t*, buf_peek_ptr(&graph->nodes));
        node->pass_handle = pass_handle;
        node->next_pass_node_handles = memory_allocator_buf_new(graph->renderer->allocator, vulkan_render_pass_graph_node_handle_t);
        node->prev_pass_count = 0;
        node->depth = 0;
        node_index = buf_get_element_count(&graph->nodes) - 1;
    }
    else
        node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, node_index);

    if(graph->prev_pass_node_handle != VULKAN_RENDER_PASS_GRAPH_NODE_HANDLE_INVALID)
    {
        AUTO prev_node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, graph->prev_pass_node_handle);
        if(buf_find_index_of(&prev_node->next_pass_node_handles, (void*)(&node_index), node_handle_comparer) == BUF_INVALID_INDEX)
        {
            buf_push(&prev_node->next_pass_node_handles, &node_index);
            node->prev_pass_count++;
        }
        node->depth = max(prev_node->depth + 1, node->depth);
    }
 
    graph->prev_pass_node_handle = node_index;
}

#ifdef GLOBAL_DEBUG

static void render_pass_graph_dump(vulkan_render_pass_graph_t* graph, bool is_printable)
{
    puts("digraph\n{");
    u32 node_count = buf_get_element_count(&graph->nodes);
    for(u32 i = 0; i < node_count; i++)
    {
        AUTO node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, i);
        u32 next_pass_count = buf_get_element_count(&node->next_pass_node_handles);
        if(is_printable)
            printf("\t%"PRIu32" -> { ", i);
        else
            printf("\t%"PRIu32"(%"PRIu64", %"PRIu32") -> { ", i, node->pass_handle, node->depth);
        for(u32 j = 0; j < next_pass_count; j++)
        {
            vulkan_render_pass_graph_node_handle_t node_handle = *buf_get_ptr_at_typeof(&node->next_pass_node_handles, vulkan_render_pass_graph_node_handle_t, j);
            vulkan_render_pass_handle_t pass_handle = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, node_handle)->pass_handle;
            if(is_printable)
                printf("%"PRIu32" ", node_handle);
            else
                printf("%"PRIu32"(%"PRIu64") ", node_handle, pass_handle);
        }
        puts("}");
    }
    puts("}");
}

SGE_API void vulkan_render_pass_graph_dump(vulkan_render_pass_graph_t* graph)
{
    render_pass_graph_dump(graph, false);
    render_pass_graph_dump(graph, true);
}

#endif /* GLOBAL_DEBUG */

static bool is_all_dependencies_resolved(vulkan_render_pass_graph_t* graph, vulkan_render_pass_graph_node_t* node, vulkan_render_pass_graph_node_handle_t node_handle)
{
    u32 dependency_count = node->prev_pass_count;
    u32 count = buf_get_element_count(&graph->optimized_render_path);
    for(u32 i = 0; i < count; i++)
    {
        vulkan_render_pass_graph_node_handle_t _node_handle;
        buf_get_at(&graph->optimized_render_path, i, &_node_handle);

        /* if this node is a dependency of the node for which this function has been called then decrement the dependency count */
        if(buf_find_index_of(&buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, _node_handle)->next_pass_node_handles, 
                            (void*)(&node_handle), buf_u32_comparer) != BUF_INVALID_INDEX)
            dependency_count--;
    }
    return dependency_count == 0;
}

static bool node_handle_greater_than(void* lhs, void* rhs, void* user_data)
{
    vulkan_render_pass_graph_t* graph = CAST_TO(vulkan_render_pass_graph_t*, user_data);
    AUTO lhs_node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, DREF_TO(vulkan_render_pass_graph_node_handle_t, lhs));
    AUTO rhs_node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, DREF_TO(vulkan_render_pass_graph_node_handle_t, rhs));
    return buf_get_element_count(&lhs_node->next_pass_node_handles) > buf_get_element_count(&rhs_node->next_pass_node_handles);
}

SGE_API vulkan_render_pass_graph_node_handle_list_t* vulkan_render_pass_graph_get_or_build_optimized_path(vulkan_render_pass_graph_t* graph)
{
    if(!graph->is_outdated)
        return &graph->optimized_render_path;

    /* NOTE: We can't build the optimized path incrementally
     * However, a bit of research would be needed */
    buf_clear(&graph->optimized_render_path, NULL);

    u32 count = buf_get_element_count(&graph->nodes);
    u32 node_count = count;
    u32 depth = 0;
    while(count > 0)
    {
        /* collect all the nodes (at the same depth), whose dependencies have been resolved, into the staging list */
        for(u32 i = 0; i < node_count; i++)
        {
            AUTO node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, i);
            if((node->depth <= depth) 
                && (buf_find_index_of(&graph->optimized_render_path, (void*)(&i), buf_u32_comparer) == BUF_INVALID_INDEX) 
                && is_all_dependencies_resolved(graph, node, i))
                buf_push(&graph->staging_list, &i);
        }

        /* now sort nodes in the staging list in the order of decreasing number of child nodes (nextPassHandles) */
        buf_sort(&graph->staging_list, node_handle_greater_than, (void*)graph);
            
        /* commit them into optimized render path buffer */
        u32 stage_count = buf_get_element_count(&graph->staging_list);
        for(u32 i = 0; i < stage_count; i++)
            buf_push(&graph->optimized_render_path, buf_get_ptr_at(&graph->staging_list, i));
            
        /* clear the staging list for the next iteration */
        buf_clear(&graph->staging_list, NULL);
        
        /* decrement the count by the number of nodes which were in the staging buffer (as they are committed) */
        _assert(count >= stage_count);
        count -= stage_count;

        depth++;
    }
    
    graph->is_outdated = false;

    return &graph->optimized_render_path;
}

#ifdef GLOBAL_DEBUG
SGE_API void vulkan_render_pass_graph_dump_optimized_path(vulkan_render_pass_graph_t* graph)
{
    printf("Optimized Render Path: ");
    u32 count = buf_get_element_count(&graph->optimized_render_path);
    for(u32 i = 0; i < count; i++)
    {
        vulkan_render_pass_graph_node_handle_t node_handle;
        buf_get_at(&graph->optimized_render_path, i, &node_handle);
        AUTO node = buf_get_ptr_at_typeof(&graph->nodes, vulkan_render_pass_graph_node_t, node_handle);
        printf("%"PRIu32"(%"PRIu64") ", node_handle, node->pass_handle);
    }
    puts("");
}
#endif /* GLOBAL_DEBUG */
