#include <stdio.h>
#include <common/defines.h>
#include <common/assert.h>
#include <bufferlib/buffer.h>
#include <stdlib.h>
#include <string.h>

#define new(type) (type*)malloc(sizeof(type))
#define delete(ptr) free((void*)(ptr))

typedef struct RenderPassCreateInfo
{
	u32 hash;
} RenderPassCreateInfo;

typedef struct RenderPass
{
	u32 internalHandle;
} RenderPass;

static void RenderPassCreateNoAlloc(RenderPass* const pass, const RenderPassCreateInfo* createInfo)
{
	static u32 internalHandleCounter = 0; ++internalHandleCounter;
	pass->internalHandle = internalHandleCounter;
	puts("Render Pass Created");
}

static void RenderPassDestroy(RenderPass* pass)
{
	// nothing
	puts("Render Pass Destroyed");
}

/* --- Render Pass Library --- */

typedef struct RenderPassGraphSlot
{
	RenderPass pass;
	RenderPassCreateInfo createInfo;
} RenderPassGraphSlot;

typedef u32 RenderPassGraphSlotHandle;
#define RENDER_PASS_GRAPH_SLOT_HANDLE_INVALID U32_MAX
typedef u32 RenderPassGraphNodeHandle;
#define RENDER_PASS_GRAPH_NODE_HANDLE_INVALID U32_MAX

typedef BUFFER /* RenderPassGraphNodeHandle */ RenderPassGraphNodeHandleList;

typedef struct RenderPassGraphNode
{
	RenderPassGraphSlotHandle slotHandle;
	RenderPassGraphNodeHandleList nextPassNodeHandles;
} RenderPassGraphNode;

typedef BUFFER /* RenderPassGraphSlot */ RenderPassGraphSlotList;
typedef BUFFER /* RenderPassGraphNode */ RenderPassGraphNodeList;
typedef BUFFER /* RenderPassGraphNode* */ RenderPassGraphNodePtrStack;

typedef struct RenderPassGraph
{
	RenderPassGraphSlotList slots;
	RenderPassGraphNodeList nodes;
	RenderPassGraphSlotHandle prevPassNodeHandle;
	RenderPassGraphNodePtrStack nodePtrStack;
} RenderPassGraph;

static RenderPassGraph* RenderPassGraphCreate()
{
	RenderPassGraph* graph = new(RenderPassGraph);
	graph->slots = buf_new(RenderPassGraphSlot);
	graph->nodes = buf_new(RenderPassGraphNode);
	graph->prevPassNodeHandle = RENDER_PASS_GRAPH_NODE_HANDLE_INVALID;
	graph->nodePtrStack = buf_new(RenderPassGraphNode*);
	puts("Render Pass Library Created");
	return graph;
}

static void RenderPassGraphDestroy(RenderPassGraph* graph)
{
	u32 slotCount = buf_get_element_count(&graph->slots);
	for(u32 i = 0; i < slotCount; i++)
	{
		AUTO slot = buf_get_ptr_at_typeof(&graph->slots, RenderPassGraphSlot, i);
		RenderPassDestroy(&slot->pass);
	}
	buf_free(&graph->slots);
	delete(graph);
	puts("Render Pass Library Destroyed");
}

static void RenderPassGraphCreateRenderPath(RenderPassGraph* graph)
{
	graph->prevPassNodeHandle = RENDER_PASS_GRAPH_NODE_HANDLE_INVALID;
}

static bool createInfoComparer(void* value, void* cursor)
{
	const RenderPassCreateInfo* createInfo = CAST_TO(RenderPassCreateInfo*, value);
	const RenderPassGraphSlot* slot = CAST_TO(RenderPassGraphSlot*, cursor);
	return createInfo->hash == slot->createInfo.hash;
}

static void dumpNodePtrStack(RenderPassGraph* graph)
{
	printf("Stack: ");
	u32 count = buf_get_element_count(&graph->nodePtrStack);
	for(u32 i = 0; i < count; i++)
	{
		RenderPassGraphNode* node;
		buf_get_at(&graph->nodePtrStack, i, &node);
		printf("%lu ", node->slotHandle + 1);
	}
	puts("");
}

static bool IsCycle(RenderPassGraph* graph, RenderPassGraphNode* node, RenderPassGraphNode* prevNode)
{
	if(node == prevNode)
		return true;
	u32 nextCount = buf_get_element_count(&node->nextPassNodeHandles);
	for(u32 i = 0; i < nextCount; i++)
	{
		RenderPassGraphNodeHandle nextHandle;
		buf_get_at(&node->nextPassNodeHandles, i, &nextHandle);
		AUTO nextNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nextHandle);
		if(IsCycle(graph, nextNode, prevNode))
			return true;
	}
	return false;
}

typedef struct NodeComparerData
{
	RenderPassGraph* graph;
	RenderPassGraphSlotHandle slotHandle;
} NodeComparerData;

static bool nodeComparer(void* value, void* cursor)
{
	AUTO data = CAST_TO(NodeComparerData*, value);
	AUTO node = CAST_TO(RenderPassGraphNode*, cursor);
	/* if there is slot match and then make sure there is no cycle */
	if(node->slotHandle == data->slotHandle)
	{
		RenderPassGraph* graph = data->graph;
		_assert(buf_get_element_count(&graph->nodePtrStack) == 0);
		return (graph->prevPassNodeHandle == RENDER_PASS_GRAPH_NODE_HANDLE_INVALID) ||
				!IsCycle(graph, node, buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, graph->prevPassNodeHandle));
	}

	return false;
}

static bool renderPassGraphNodeHandleComparer(void* value, void* cursor)
{
	return DREF_TO(RenderPassGraphNodeHandle, value) == DREF_TO(RenderPassGraphNodeHandle, cursor);
}

static RenderPassGraphSlotHandle RenderPassGraphCreatePass(RenderPassGraph* graph, const RenderPassCreateInfo* createInfo)
{
	/* Create or Get RenderPassGraphSlot */
	buf_ucount_t slotIndex = buf_find_index_of(&graph->slots, (void*)createInfo, createInfoComparer);
	RenderPassGraphSlot* slot;
	if(slotIndex == BUF_INVALID_INDEX)
	{
		/* Create Render Pass Object and a slot for it (RenderPassGraphSlot) */
		buf_push_pseudo(&graph->slots, 1);
		slot = CAST_TO(RenderPassGraphSlot*, buf_peek_ptr(&graph->slots));
		RenderPassCreateNoAlloc(&slot->pass, createInfo);
		memcpy(&slot->createInfo, createInfo, sizeof(RenderPassCreateInfo));
		slotIndex = buf_get_element_count(&graph->slots) - 1;
	}
	else
		slot = buf_get_ptr_at_typeof(&graph->slots, RenderPassGraphSlot, slotIndex);

	/* Create or Get RenderPassGraphNode */
	NodeComparerData data = { .graph = graph, .slotHandle = slotIndex };
	buf_ucount_t nodeIndex = buf_find_index_of(&graph->nodes, (void*)(&data), nodeComparer);
	RenderPassGraphNode* node;
	if(nodeIndex == BUF_INVALID_INDEX)
	{
		/* Create a RenderPassGraphNode object */
		buf_push_pseudo(&graph->nodes, 1);
		node = CAST_TO(RenderPassGraphNode*, buf_peek_ptr(&graph->nodes));
		node->slotHandle = slotIndex;
		node->nextPassNodeHandles = buf_new(RenderPassGraphNodeHandle);
		nodeIndex = buf_get_element_count(&graph->nodes) - 1;
	}
	else
		node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nodeIndex);

	if(graph->prevPassNodeHandle != RENDER_PASS_GRAPH_NODE_HANDLE_INVALID)
	{
		AUTO prevNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, graph->prevPassNodeHandle);
		if(buf_find_index_of(&prevNode->nextPassNodeHandles, (void*)(&nodeIndex), renderPassGraphNodeHandleComparer) == BUF_INVALID_INDEX)
			buf_push(&prevNode->nextPassNodeHandles, &nodeIndex);
	}
 
	graph->prevPassNodeHandle = nodeIndex;
	return (RenderPassGraphSlotHandle)slotIndex;
}

static void RenderPassGraphDump(RenderPassGraph* graph)
{
	puts("digraph\n{");
	u32 nodeCount = buf_get_element_count(&graph->nodes);
	for(u32 i = 0; i < nodeCount; i++)
	{
		AUTO node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, i);
		u32 nextPassCount = buf_get_element_count(&node->nextPassNodeHandles);
		printf("\t%lu(%lu) -> { ", i + 1, node->slotHandle + 1);
		for(u32 j = 0; j < nextPassCount; j++)
		{
			RenderPassGraphNodeHandle nodeHandle = *buf_get_ptr_at_typeof(&node->nextPassNodeHandles, RenderPassGraphNodeHandle, j);
			RenderPassGraphSlotHandle slotHandle = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nodeHandle)->slotHandle;
			printf("%lu(%lu) ", nodeHandle + 1, slotHandle + 1);
		}
		puts("}");
	}
	puts("}");
}

static void RunRenderPaths(RenderPassGraph* graph)
{

}

/* --- Context --- */

typedef struct Context
{
	RenderPassGraph* graph;
} Context;


static Context* ContextCreate()
{
	Context* context = new(Context);
	context->graph = RenderPassGraphCreate();
	puts("Context Created");
	return context;
}

static void ContextDestroy(Context* context)
{
	RenderPassGraphDestroy(context->graph);
	delete(context);
	puts("Context Destroyed");
}

/* --- Shader --- */

typedef struct ShaderCreateInfo
{
	RenderPassCreateInfo* passCreateInfos;
	u32 passCreateInfoCount;
} ShaderCreateInfo;

typedef struct Shader
{
	BUFFER renderPassHandles;
} Shader;

static Shader* ShaderCreate(Context* context, const ShaderCreateInfo* createInfo)
{
	Shader* shader = new(Shader);
	shader->renderPassHandles = buf_new(RenderPassGraphSlotHandle);
	RenderPassGraphCreateRenderPath(context->graph);
	for(u32 i = 0; i < createInfo->passCreateInfoCount; i++)
	{
		RenderPassGraphSlotHandle handle = RenderPassGraphCreatePass(context->graph, &createInfo->passCreateInfos[i]);
		buf_push(&shader->renderPassHandles, &handle);
	}
	puts("Shader Created");
	return shader;
}

static void ShaderDestroy(Shader* shader)
{
	buf_free(&shader->renderPassHandles);
	delete(shader);
	puts("Shader Destroyed");
}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static Shader* LoadShader1(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 1 },
		{ .hash = 2 },
		{ .hash = 3 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}

static Shader* LoadShader2(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 1 },
		{ .hash = 4 },
		{ .hash = 3 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}

static Shader* LoadShader3(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 1 },
		{ .hash = 4 },
		{ .hash = 5 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}

static Shader* LoadShader4(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 1 },
		{ .hash = 4 },
		{ .hash = 6 },
		{ .hash = 7 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}

static Shader* LoadShader5(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 8 },
		{ .hash = 6 },
		{ .hash = 9 },
		{ .hash = 4 },
		{ .hash = 6 },
		{ .hash = 7 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}

static Shader* LoadShader6(Context* context)
{
	RenderPassCreateInfo passes[] = 
	{
		{ .hash = 8 },
		{ .hash = 6 },
		{ .hash = 9 },
		{ .hash = 4 },
		{ .hash = 4 },
		{ .hash = 4 },
		{ .hash = 6 },
		{ .hash = 7 }
	};

	ShaderCreateInfo createInfo = 
	{
		.passCreateInfoCount = ARRAY_SIZE(passes),
		.passCreateInfos = passes
	};
	return ShaderCreate(context, &createInfo);
}


int main()
{
	Context* context = ContextCreate();
	Shader* shader1 = LoadShader1(context);
	Shader* shader2 = LoadShader2(context);
	Shader* shader3 = LoadShader3(context);
	Shader* shader4 = LoadShader4(context);
	Shader* shader5 = LoadShader5(context);
	Shader* shader6 = LoadShader6(context);
	RenderPassGraphDump(context->graph);
	RunRenderPaths(context->graph);
	ShaderDestroy(shader1);
	ShaderDestroy(shader2);
	ShaderDestroy(shader3);
	ShaderDestroy(shader4);
	ShaderDestroy(shader5);
	ShaderDestroy(shader6);
	ContextDestroy(context);
	return 0;
}
