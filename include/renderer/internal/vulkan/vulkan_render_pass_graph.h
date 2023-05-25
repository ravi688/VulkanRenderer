#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/internal/vulkan/vulkan_handles.h>

/* stores references to a node in the vulkan render pass graph */
typedef u32 vulkan_render_pass_graph_node_handle_t;
#define VULKAN_RENDER_PASS_GRAPH_NODE_HANDLE_INVALID U32_MAX

typedef BUFFER /* vulkan_render_pass_graph_node_handle_t */ vulkan_render_pass_graph_node_handle_list_t;

/* represents a node in the vulkan render pass graph */
typedef struct vulkan_render_pass_graph_node_t
{
	/* handle to the actual pass (vulkan_render_pass_t) in the vulkan render pass pool */
	vulkan_render_pass_handle_t pass_handle;
	/* list of next passes (vulkan_render_pass_graph_node_t which in turn contains a reference to the actual render pass object (vulkan_render_pass_t)) */
	vulkan_render_pass_graph_node_handle_list_t next_pass_node_handles;
	/* number of dependencies (the passes which have to be run before the pass this node references */
	u32 prev_pass_count;
	/* the depth of this node in the render pass graph */
	u32 depth;
} vulkan_render_pass_graph_node_t;

/* list of */
typedef BUFFER /* vulkan_render_pass_graph_node_t */ vulkan_render_pass_graph_node_list_t;

typedef struct vulkan_renderer_t vulkan_renderer_t;

/* stores graph data structure */
typedef struct vulkan_render_pass_graph_t
{
	vulkan_renderer_t* renderer;
	/* list of nodes in this graph */
	vulkan_render_pass_graph_node_list_t nodes;

	/* following fields modify their data upon calling 
	 * vulkan_render_pass_graph_create_optimized_render_path()
	 * vulkan_render_pass_graph_create_pass() 
	 */

	/* handle to the previous node */
	vulkan_render_pass_graph_node_handle_t prev_pass_node_handle;
	/* list of minimum number of nodes ordered in such a way that every path 
	 * is covered from the starting points to the ending points incrementally */
	vulkan_render_pass_graph_node_handle_list_t optimized_render_path;
	/* staging list to hold intermediate node handles while generating the optimized render path */
	vulkan_render_pass_graph_node_handle_list_t staging_list;

	/* true if the optimized render path has to be rebuilt, otherwise false */
	bool is_outdated;
} vulkan_render_pass_graph_t;


BEGIN_CPP_COMPATIBLE

/* constructors and destructors */

/* creates a vulkan_render_pass_graph_t object and returns pointer to it. */
RENDERER_API vulkan_render_pass_graph_t* vulkan_render_pass_graph_new(memory_allocator_t* allocator);
/* (calls vulkan_render_pass_graph_create internally). 
 * allocates memory for the lists in the vulkan_render_pass_graph_t structure. */
RENDERER_API vulkan_render_pass_graph_t* vulkan_render_pass_graph_create(vulkan_renderer_t* renderer);
/* identical to the above fuction except it doesn't allocate memory for vulkan_render_pass_graph_t object */
RENDERER_API void vulkan_render_pass_graph_create_no_alloc(vulkan_renderer_t* renderer, vulkan_render_pass_graph_t OUT graph);
/* destroys the vulkan API objects and makes the vulkan_render_pass_graph_t reusable again */
RENDERER_API void vulkan_render_pass_graph_destroy(vulkan_render_pass_graph_t* graph);
/* releases all the heap memory allocated */
RENDERER_API void vulkan_render_pass_graph_release_resources(vulkan_render_pass_graph_t* graph);

/* logic functions */

/* creates a new path in the render pass graph (must be called before adding render pass handles for the new shader) */
RENDERER_API void vulkan_render_pass_graph_create_path(vulkan_render_pass_graph_t* graph);
/* adds a new render pass handle */
RENDERER_API void vulkan_render_pass_graph_add(vulkan_render_pass_graph_t* graph, vulkan_render_pass_handle_t pass_handle);

/* prints the graph in dot language */
#ifdef GLOBAL_DEBUG
RENDERER_API void vulkan_render_pass_graph_dump(vulkan_render_pass_graph_t* graph);
#else
#	define vulkan_render_pass_graph_dump(...)
#endif /* GLOBAL_DEBUG */

/* returns a list of nodes (an optimized render path) */
RENDERER_API vulkan_render_pass_graph_node_handle_list_t* vulkan_render_pass_graph_get_or_build_optimized_path(vulkan_render_pass_graph_t* graph);


END_CPP_COMPATIBLE
