/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: main.c is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

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
	u32 prevPassCount;
	u32 depth;
} RenderPassGraphNode;

typedef BUFFER /* RenderPassGraphSlot */ RenderPassGraphSlotList;
typedef BUFFER /* RenderPassGraphNode */ RenderPassGraphNodeList;
typedef BUFFER /* RenderPassGraphNode* */ RenderPassGraphNodePtrStack;

typedef struct RenderPassGraph
{
	RenderPassGraphSlotList slots;
	RenderPassGraphNodeList nodes;
	RenderPassGraphNodeHandle prevPassNodeHandle;
	RenderPassGraphNodeHandleList optimizedRenderPath;
} RenderPassGraph;

static RenderPassGraph* RenderPassGraphCreate()
{
	RenderPassGraph* graph = new(RenderPassGraph);
	graph->slots = buf_new(RenderPassGraphSlot);
	graph->nodes = buf_new(RenderPassGraphNode);
	graph->prevPassNodeHandle = RENDER_PASS_GRAPH_NODE_HANDLE_INVALID;
	graph->optimizedRenderPath = buf_new(RenderPassGraphNodeHandle);
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

static bool isCycle(RenderPassGraph* graph, RenderPassGraphNode* node, RenderPassGraphNode* prevNode)
{
	if(node == prevNode)
		return true;
	u32 nextCount = buf_get_element_count(&node->nextPassNodeHandles);
	for(u32 i = 0; i < nextCount; i++)
	{
		RenderPassGraphNodeHandle nextHandle;
		buf_get_at(&node->nextPassNodeHandles, i, &nextHandle);
		AUTO nextNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nextHandle);
		if(isCycle(graph, nextNode, prevNode))
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
		return (graph->prevPassNodeHandle == RENDER_PASS_GRAPH_NODE_HANDLE_INVALID) ||
				!isCycle(graph, node, buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, graph->prevPassNodeHandle));
	}

	return false;
}

static bool renderPassGraphNodeHandleComparer(void* value, void* cursor)
{
	return DREF_TO(RenderPassGraphNodeHandle, value) == DREF_TO(RenderPassGraphNodeHandle, cursor);
}

static u32 max(u32 v1, u32 v2)
{
	return (v1 > v2) ? v1 : v2;
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
		node->prevPassCount = 0;
		node->depth = 0;
		nodeIndex = buf_get_element_count(&graph->nodes) - 1;
	}
	else
		node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nodeIndex);

	if(graph->prevPassNodeHandle != RENDER_PASS_GRAPH_NODE_HANDLE_INVALID)
	{
		AUTO prevNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, graph->prevPassNodeHandle);
		if(buf_find_index_of(&prevNode->nextPassNodeHandles, (void*)(&nodeIndex), renderPassGraphNodeHandleComparer) == BUF_INVALID_INDEX)
		{
			buf_push(&prevNode->nextPassNodeHandles, &nodeIndex);
			node->prevPassCount++;
		}
		node->depth = max(prevNode->depth + 1, node->depth);
	}
 
	graph->prevPassNodeHandle = nodeIndex;
	return (RenderPassGraphSlotHandle)slotIndex;
}

static void RenderPassGraphDump(RenderPassGraph* graph, bool isPrintable)
{
	puts("digraph\n{");
	u32 nodeCount = buf_get_element_count(&graph->nodes);
	for(u32 i = 0; i < nodeCount; i++)
	{
		AUTO node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, i);
		u32 nextPassCount = buf_get_element_count(&node->nextPassNodeHandles);
		if(isPrintable)
			printf("\t%lu -> { ", i + 1);
		else
			printf("\t%lu(%lu, %lu) -> { ", i + 1, node->slotHandle + 1, node->depth);
		for(u32 j = 0; j < nextPassCount; j++)
		{
			RenderPassGraphNodeHandle nodeHandle = *buf_get_ptr_at_typeof(&node->nextPassNodeHandles, RenderPassGraphNodeHandle, j);
			RenderPassGraphSlotHandle slotHandle = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nodeHandle)->slotHandle;
			if(isPrintable)
				printf("%lu ", nodeHandle + 1);
			else
				printf("%lu(%lu) ", nodeHandle + 1, slotHandle + 1);
		}
		puts("}");
	}
	puts("}");
}

static bool isAllDependenciesResolved(RenderPassGraph* graph, RenderPassGraphNode* node, RenderPassGraphNodeHandle nodeHandle)
{
	u32 dependencyCount = node->prevPassCount;
	u32 count = buf_get_element_count(&graph->optimizedRenderPath);
	for(u32 i = 0; i < count; i++)
	{
		RenderPassGraphNodeHandle _nodeHandle;
		buf_get_at(&graph->optimizedRenderPath, i, &_nodeHandle);

		/* if this node is a dependency of the node for which this function has been called then decrement the dependency count */
		if(buf_find_index_of(&buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, _nodeHandle)->nextPassNodeHandles, 
							(void*)(&nodeHandle), buf_u32_comparer) != BUF_INVALID_INDEX)
			dependencyCount--;
	}
	return dependencyCount == 0;
}

static bool nodeHandleGreaterThan(void* lhs, void* rhs, void* user_data)
{
	RenderPassGraph* graph = CAST_TO(RenderPassGraph*, user_data);
	AUTO lhsNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, DREF_TO(RenderPassGraphNodeHandle, lhs));
	AUTO rhsNode = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, DREF_TO(RenderPassGraphNodeHandle, rhs));
	return buf_get_element_count(&lhsNode->nextPassNodeHandles) > buf_get_element_count(&rhsNode->nextPassNodeHandles);
}

static void BuildOptimizedRenderPath(RenderPassGraph* graph)
{
	u32 count = buf_get_element_count(&graph->nodes);
	u32 nodeCount = count;
	BUFFER stagingList = buf_new(RenderPassGraphNodeHandle);
	u32 depth = 0;
	while(count > 0)
	{
		/* collect all the nodes (at the same depth), whose dependencies have been resolved, into the staging list */
		for(u32 i = 0; i < nodeCount; i++)
		{
			AUTO node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, i);
			if((node->depth <= depth) 
				&& (buf_find_index_of(&graph->optimizedRenderPath, (void*)(&i), buf_u32_comparer) == BUF_INVALID_INDEX) 
				&& isAllDependenciesResolved(graph, node, i))
				buf_push(&stagingList, &i);
		}

		/* now sort nodes in the staging list in the order of decreasing number of child nodes (nextPassHandles) */
		buf_sort(&stagingList, nodeHandleGreaterThan, (void*)graph);
			
		/* commit them into optimized render path buffer */
		u32 stageCount = buf_get_element_count(&stagingList);
		for(u32 i = 0; i < stageCount; i++)
			buf_push(&graph->optimizedRenderPath, buf_get_ptr_at(&stagingList, i));
			
		/* clear the staging list for the next iteration */
		buf_clear(&stagingList, NULL);
		
		/* decrement the count by the number of nodes which were in the staging buffer (as they are committed) */
		_assert(count >= stageCount);
		count -= stageCount;

		depth++;
	}
	buf_free(&stagingList);
}

static void RunOptimizedRenderPath(RenderPassGraph* graph)
{
	printf("Optimized Render Path: ");
	u32 count = buf_get_element_count(&graph->optimizedRenderPath);
	for(u32 i = 0; i < count; i++)
	{
		RenderPassGraphNodeHandle nodeHandle;
		buf_get_at(&graph->optimizedRenderPath, i, &nodeHandle);
		AUTO node = buf_get_ptr_at_typeof(&graph->nodes, RenderPassGraphNode, nodeHandle);
		printf("%lu(%lu) ", nodeHandle + 1, node->slotHandle + 1);
	}
	puts("");
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
	BUFFER testBuffer = buf_new(u32);
	u32 value = 100; buf_push(&testBuffer, &value);
	value = 120; buf_push(&testBuffer, &value);
	value = 20; buf_push(&testBuffer, &value);
	value = 12; buf_push(&testBuffer, &value);
	value = 10; buf_push(&testBuffer, &value);
	value = 34; buf_push(&testBuffer, &value);
	value = 50; buf_push(&testBuffer, &value);
	value = 620; buf_push(&testBuffer, &value);
	value = 220; buf_push(&testBuffer, &value);
	value = 50; buf_push(&testBuffer, &value);
	value = 720; buf_push(&testBuffer, &value);
	value = 820; buf_push(&testBuffer, &value);
	value = 920; buf_push(&testBuffer, &value);
	value = 0; buf_push(&testBuffer, &value);
	printf("Unsorted Elements: ");
	buf_traverse_elements(&testBuffer, 0, buf_get_element_count(&testBuffer) - 1, buf_u32_print, NULL);
	puts("");
	/* sort in descending order */
	buf_sort(&testBuffer, buf_u32_greater_than, NULL);
	printf("Sorted Elements: ");
	buf_traverse_elements(&testBuffer, 0, buf_get_element_count(&testBuffer) - 1, buf_u32_print, NULL);
	puts("");
	buf_free(&testBuffer);

	Context* context = ContextCreate();
	Shader* shader1 = LoadShader1(context);
	Shader* shader2 = LoadShader2(context);
	Shader* shader3 = LoadShader3(context);
	Shader* shader4 = LoadShader4(context);
	Shader* shader5 = LoadShader5(context);
	Shader* shader6 = LoadShader6(context);
	RenderPassGraphDump(context->graph, false);
	RenderPassGraphDump(context->graph, true);
	BuildOptimizedRenderPath(context->graph);
	RunOptimizedRenderPath(context->graph);
	ShaderDestroy(shader1);
	ShaderDestroy(shader2);
	ShaderDestroy(shader3);
	ShaderDestroy(shader4);
	ShaderDestroy(shader5);
	ShaderDestroy(shader6);
	ContextDestroy(context);
	return 0;
}
