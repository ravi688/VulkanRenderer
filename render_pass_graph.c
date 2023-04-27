/* Taken from:  https://github.com/scottt/debugbreak */

/* Copyright (c) 2011-2021, Scott Tsai
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DEBUG_BREAK_H
#define DEBUG_BREAK_H

#ifdef _MSC_VER

#define debug_break __debugbreak

#else

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG_BREAK_USE_TRAP_INSTRUCTION 1
#define DEBUG_BREAK_USE_BULTIN_TRAP      2
#define DEBUG_BREAK_USE_SIGTRAP          3

#if defined(__i386__) || defined(__x86_64__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__inline__ static void trap_instruction(void)
{
	__asm__ volatile("int $0x03");
}
#elif defined(__thumb__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
/* FIXME: handle __THUMB_INTERWORK__ */
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in GDB source.
	 * Both instruction sequences below work. */
#if 1
	/* 'eabi_linux_thumb_le_breakpoint' */
	__asm__ volatile(".inst 0xde01");
#else
	/* 'eabi_linux_thumb2_le_breakpoint' */
	__asm__ volatile(".inst.w 0xf7f0a000");
#endif

	/* Known problem:
	 * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
	 * 'step' would keep getting stuck on the same instruction.
	 *
	 * Workaround: use the new GDB commands 'debugbreak-step' and
	 * 'debugbreak-continue' that become available
	 * after you source the script from GDB:
	 *
	 * $ gdb -x debugbreak-gdb.py <... USUAL ARGUMENTS ...>
	 *
	 * 'debugbreak-step' would jump over the breakpoint instruction with
	 * roughly equivalent of:
	 * (gdb) set $instruction_len = 2
	 * (gdb) tbreak *($pc + $instruction_len)
	 * (gdb) jump   *($pc + $instruction_len)
	 */
}
#elif defined(__arm__) && !defined(__thumb__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in GDB source,
	 * 'eabi_linux_arm_le_breakpoint' */
	__asm__ volatile(".inst 0xe7f001f0");
	/* Known problem:
	 * Same problem and workaround as Thumb mode */
}
#elif defined(__aarch64__) && defined(__APPLE__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
#elif defined(__aarch64__)
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'aarch64-tdep.c' in GDB source,
	 * 'aarch64_default_breakpoint' */
	__asm__ volatile(".inst 0xd4200000");
}
#elif defined(__powerpc__)
	/* PPC 32 or 64-bit, big or little endian */
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'rs6000-tdep.c' in GDB source,
	 * 'rs6000_breakpoint' */
	__asm__ volatile(".4byte 0x7d821008");

	/* Known problem:
	 * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
	 * 'step' stuck on the same instruction ("twge r2,r2").
	 *
	 * The workaround is the same as ARM Thumb mode: use debugbreak-gdb.py
	 * or manually jump over the instruction. */
}
#elif defined(__riscv)
	/* RISC-V 32 or 64-bit, whether the "C" extension
	 * for compressed, 16-bit instructions are supported or not */
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void trap_instruction(void)
{
	/* See 'riscv-tdep.c' in GDB source,
	 * 'riscv_sw_breakpoint_from_kind' */
	__asm__ volatile(".4byte 0x00100073");
}
#else
	#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_SIGTRAP
#endif


#ifndef DEBUG_BREAK_IMPL
#error "debugbreak.h is not supported on this target"
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_TRAP_INSTRUCTION
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	trap_instruction();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	__builtin_debugtrap();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_TRAP
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	__builtin_trap();
}
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_SIGTRAP
#include <signal.h>
__attribute__((always_inline))
__inline__ static void debug_break(void)
{
	raise(SIGTRAP);
}
#else
#error "invalid DEBUG_BREAK_IMPL value"
#endif

#ifdef __cplusplus
}
#endif

#endif /* ifdef _MSC_VER */

#endif /* ifndef DEBUG_BREAK_H */


#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef uint32_t u32;
#define U32_MAX (~0UL)

#define debug_assert(assertion) __private__assert(assertion, #assertion, __LINE__, __FUNCTION__, __FILE__)
static void __private__assert(bool assertion, const char* description, u32 line, const char* function, const char* file)
{
	if(assertion)
		return;
	printf("Assertion Failed: %s, at %lu, %s, %s\n", description, line, function, file);
	debug_break();
}

#define ARGS __FUNCTION__##_args
#define ARGS_BEGIN(var) va_list ARGS; va_start(ARGS, var)
#define ARGS_GET(type) va_arg(ARGS, type)
#define ARGS_END() va_end(ARGS)

#define foreach_arg(type_ptr, id, count) ARGS_BEGIN(count); u32 __i__ = 0; for(type_ptr id = (((count) == 0) ? (type_ptr)(NULL) : ARGS_GET(type_ptr)); __i__ < count; __i__++, id = ARGS_GET(type_ptr), (__i__ == count) ? ARGS_END() : (void)(1))

/* structure to hold a pair of u32 and bool values */
typedef struct u32_bool_pair_t
{
	/* pass id */
	u32 id;
	/* true, if this dependency is resolved otherwise false */
	u32 resolved;
} u32_bool_pair_t;

typedef struct pass_t pass_t;

/* structure to hold render pass information such as execution dependencies, and next pass executions */
typedef struct pass_t
{
	/* unique id assigned to this pass at the time creation */
	u32 id;

	/* list of next passes to be executed after this pass */
	u32* nexts;
	/* number of passes in the above list */
	u32 next_count;
	/* list of previous passes which have to be executed before this pass */
	u32_bool_pair_t* prevs;
	/* number of passes in the above list */
	u32 prev_count;

	/* true if this pass is completed (dependencies resolved, and this is executed after that) */
	bool has_completed;
	/* true if one of it's dependencies get resolved (then it becomes dirty, means need to be executed again) */
	bool is_dirty;
} pass_t;

/* structure to hold graph of render passes */
typedef struct graph_t
{
	/* list of unique render passes in this graph */
	pass_t** passes;
	/* number of passes in the above list */
	u32 pass_count;
	/* max number of pases that this graph can hold */
	u32 max_pass_count;

	/* following fields are modified at runtime (when the execution resolution happens) */

	/* list of passes (their ids) which are waiting to be executed again once their dependencies get resolved */
	u32* waiting_list;
	/* number of passes (ids) in the above list */
	u32 waiting_list_size;
	/**/
	pass_t** stack;
	u32 stack_depth;
} graph_t;

/* creates a graph object with several fix sized allocations */
static graph_t* create_graph(u32 max_pass_count)
{
	graph_t* graph = (graph_t*)malloc(sizeof(graph_t));
	memset(graph, 0, sizeof(graph_t));
	graph->max_pass_count = max_pass_count;
	graph->passes = (pass_t**)malloc(sizeof(pass_t*) * max_pass_count);
	memset(graph->passes, 0, sizeof(pass_t*) * max_pass_count);
	graph->waiting_list = (u32*)malloc(sizeof(u32) * max_pass_count);
	graph->stack = (pass_t**)malloc(sizeof(pass_t*) * graph->max_pass_count);
	return graph;
}

/* clears the graph for the next usage */
static void clear_graph(graph_t* graph)
{
	for(u32 i = 0; i < graph->pass_count; i++)
	{
		if(graph->passes[i]->next_count > 0)
			free(graph->passes[i]->nexts);
		free(graph->passes[i]);
	}
	graph->pass_count = 0;
	debug_assert(graph->stack_depth == 0);
	debug_assert(graph->waiting_list_size == 0);
}

/* creates a new pass in the graph 'graph' */
static pass_t* create(graph_t* graph)
{
	debug_assert(graph->max_pass_count > graph->pass_count);
	pass_t* pass = (pass_t*)malloc(sizeof(pass_t));
	memset(pass, 0, sizeof(pass_t));
	pass->id = graph->pass_count + 1;
	pass->nexts = (u32*)malloc(sizeof(u32) * graph->max_pass_count);
	pass->prevs = (u32_bool_pair_t*)malloc(sizeof(u32_bool_pair_t) * graph->max_pass_count);

	graph->passes[graph->pass_count] = pass;
	graph->pass_count++;
	return pass;
}

/* finds a pass object by it's id in the graph */
static pass_t* id_to_pass(graph_t* graph, u32 id)
{
	for(u32 i = 0; i < graph->pass_count; i++)
		if(graph->passes[i]->id == id)
			return graph->passes[i];
	debug_assert(false);
	return NULL;
}

/* sets up the next render passes for the pass 'current' */
static void set_nexts(graph_t* graph, pass_t* current, u32 next_count, ...)
{
	debug_assert(next_count <= graph->max_pass_count);
	current->next_count = next_count;
	
	foreach_arg(pass_t*, next_pass, next_count)
	{
		current->nexts[__i__] = next_pass->id;
		next_pass->prevs[next_pass->prev_count] = (u32_bool_pair_t) { current->id, false };
		next_pass->prev_count++;
	}
}

/* dumps the pass information on stdout */
static void dump(pass_t* pass)
{
	printf("\t%lu -> { ", pass->id);
	for(u32 i = 0; i < pass->next_count; i++)
	{
		printf("%lu", pass->nexts[i]);
		if((i + 1) < pass->next_count)
			putchar(',');
		putchar(' ');
	}
	printf("}\n", pass->next_count);
}

/* dumps the entire graph of the passes in the graph 'graph' in dot language */
static void graph_dump(graph_t* graph)
{
	puts(	"------------------------------\n"
			"Render Pass Graph\n"
			"------------------------------\ndigraph\n{");
	for(u32 i = 0; i < graph->pass_count; i++)
		dump(graph->passes[i]);
	puts("}");
}

/* returns the id of the least dependent (whose prev count is least) pass out of the list of passes 'passes' */
static u32 get_least_dependent(graph_t* graph, u32* passes, u32 pass_count)
{
	if(pass_count == 0)
		return U32_MAX;

	pass_t* pass = id_to_pass(graph, passes[0]);

	u32 prev_count = pass->prev_count;
	u32 index = 0;
	for(u32 i = 1; i < pass_count; i++)
	{
		pass_t* pass = id_to_pass(graph, passes[i]);
		if(prev_count > pass->prev_count)
		{
			prev_count = pass->prev_count;
			index = i;
		}
	}
	return index;
}

/* removes a pass from the list of passes (ids) 'list' by it's index in the list and returns the new size of the list */
static u32 remove_pass(u32 index, u32* list, u32 list_size)
{
	debug_assert(list_size > 0);
	debug_assert(index < list_size);
	u32 pass = list[index];
	list_size--;
	for(u32 i = index; i < list_size; i++)
		list[i] = list[i + 1];
	list[list_size] = pass;
	return list_size;
}

/* returns true if all the dependencies (prevs) have been resolved (executed) */
static bool all_dependencies_resolved(graph_t* graph, pass_t* pass)
{
	for(u32 i = 0; i < pass->prev_count; i++)
		if(!pass->prevs[i].resolved)
			return false;
	return true;
}

/* returns true if the 'next' pass has to be executed after the 'current' pass */
static bool nexts_contains(pass_t* current, pass_t* next)
{
	for(u32 i = 0; i < current->next_count; i++)
		if(current->nexts[i] == next->id)
			return true;
	return false;
}

/* marks a pass and it's children as dirty (to be executed) recursively */
static void mark_dirty(graph_t* graph, pass_t* pass)
{
	assert(graph->stack_depth < graph->max_pass_count);

	/* mark this pass dirty */
	pass->is_dirty = true;

	/* we are using stack to detect cycles and avoid inifinite recursion */
	graph->stack[graph->stack_depth++] = pass;

	for(u32 i = 0; i < pass->next_count; i++)
	{
		pass_t* next_pass = id_to_pass(graph, pass->nexts[i]);
		
		/* set to true if cycle is detected */
		bool is_continue = false;
		for(u32 j = 0; j < graph->stack_depth; j++)
		{
			if((next_pass == graph->stack[i]) || nexts_contains(next_pass, graph->stack[i]))
			{
				is_continue = true;
				break;
			}
		}

		/* don't go further as there is a cycle */
		if(is_continue)
			continue;

		mark_dirty(graph, next_pass);
	}

	assert(graph->stack_depth > 0);
	graph->stack_depth--;
}

static void generate_execution_sequence(graph_t* graph, pass_t* pass);

/* executes all possible passes whose dependencies have been resolved and the pass is yet to be executed */
static void try_execute_waits(graph_t* graph)
{
	/* determine which passes have to be executed (mark theom dirty) */
	u32 count = 0;
	pass_t* passes[graph->max_pass_count];
	for(u32 i = 0; i < graph->waiting_list_size; i++)
	{
		pass_t* pass = id_to_pass(graph, graph->waiting_list[i]);
		if(all_dependencies_resolved(graph, pass) && !pass->has_completed)
		{
			mark_dirty(graph, pass);
			passes[count] = pass;
			count++;
		}
	}

	/* execute the dirty passes */
	while(count > 0)
	{
		--count;
		generate_execution_sequence(graph, passes[count]);
	}

	/* update the waiting list count */
	graph->waiting_list_size -= count;
}

/* dumps the waiting list (pass ids) */
static void dump_waiting_list(graph_t* graph)
{
	printf("Waitings: ");
	for(u32 i = 0; i < graph->waiting_list_size; i++)
		printf("%lu ", graph->waiting_list[i]);
	puts("");
}

/* puts a pass with id 'pass' into the waiting list */
static void wait(graph_t* graph, u32 pass)
{
	debug_assert(graph->waiting_list_size < graph->max_pass_count);
	for(u32 i = 0; i < graph->waiting_list_size; i++)
		if(graph->waiting_list[i] == pass)
			return;
	graph->waiting_list[graph->waiting_list_size] = pass;
	graph->waiting_list_size++;
	dump_waiting_list(graph);
}

/* executes a pass 'pass' and removes it from the waiting list if it is in the list */
static void execute(graph_t* graph, pass_t* pass)
{
	/* execute here (for now just dump the id) */
	printf("Execute Render Pass { ID: %lu }\n", pass->id);

	/* mark this pass as resolved for all the next passes which are dependent on it */
	for(u32 i = 0; i < pass->next_count; i++)
	{
		pass_t* next_pass = id_to_pass(graph, pass->nexts[i]);
		for(u32 j = 0; j < next_pass->prev_count; j++)
		{
			if(next_pass->prevs[j].id == pass->id)
			{
				next_pass->prevs[j].resolved = true;
				break;
			}
		}
	}

	/* this pass is no more dirty */
	pass->is_dirty = false;

	/* remove this pass from the waiting list if found */
	for(u32 i = 0; i < graph->waiting_list_size; i++)
	{
		if(graph->waiting_list[i] == pass->id)
		{
			u32 j = i;
			graph->waiting_list_size--;
			while(j < graph->waiting_list_size)
			{
				graph->waiting_list[j] = graph->waiting_list[j + 1];
				j++;
			}
			break;
		}
	}
}

/* generates the execution sequence of the render passes (resolves the pass dependencies) */
static void generate_execution_sequence(graph_t* graph, pass_t* pass)
{
	/* if the pass has been completed then just return */
	if(pass->has_completed)
		return;
	
	/* otherwise execute the pass */
	execute(graph, pass);

	/* if all dependencies resolved then mark this pass as completed */
	if(all_dependencies_resolved(graph, pass))
		pass->has_completed = true;

	u32 next_count = pass->next_count;
	while(next_count > 0)
	{
		/* get the least dependent next pass (as that would be easier to solve for) */
		u32 index = get_least_dependent(graph, pass->nexts, next_count);
		debug_assert(index != U32_MAX);
		pass_t* next_pass = id_to_pass(graph, pass->nexts[index]);
		debug_assert(next_pass != NULL);
		try_execute_waits(graph);
		if(!all_dependencies_resolved(graph, next_pass))
			wait(graph, next_pass->id);
		generate_execution_sequence(graph, next_pass);
		next_count = remove_pass(index, pass->nexts, next_count);
	}
}

static void dump_execution_sequence(graph_t* graph)
{
	puts(	"------------------------------\n"
			"Render Pass Execution Sequence\n"
			"------------------------------");
	
	/* generate the execution sequence for each starting node (the passes which don't dependent on any other pass) */
	for(u32 i = 0; i < graph->pass_count; i++)
	{
		if(graph->passes[i]->prev_count == 0)
			generate_execution_sequence(graph, graph->passes[i]);
	}
}

typedef const char* (*test_func_ptr_t)(graph_t* graph);

static const char* test1(graph_t* graph)
{
	/* create the nodes */
	pass_t* rp1 = create(graph);
	pass_t* rp2 = create(graph);
	pass_t* rp3 = create(graph);
	pass_t* rp4 = create(graph);
	pass_t* rp5 = create(graph);
	pass_t* rp6 = create(graph);

	/* setup the dependencies */
	set_nexts(graph, rp1, 2, rp2, rp3);
	set_nexts(graph, rp2, 1, rp3);
	set_nexts(graph, rp3, 1, rp6, rp4);
	set_nexts(graph, rp4, 2, rp6, rp5);
	set_nexts(graph, rp6, 3, rp5, rp3, rp2);

	return __FUNCTION__;
}

static const char* test2(graph_t* graph)
{
	/* create the nodes */
	pass_t* p1 = create(graph);
	pass_t* p2 = create(graph);
	pass_t* p3 = create(graph);
	pass_t* p4 = create(graph);
	pass_t* p5 = create(graph);

	/* setup the dependencies */
	set_nexts(graph, p1, 2, p2, p4);
	set_nexts(graph, p2, 3, p4, p3, p5);
	set_nexts(graph, p4, 1, p2);

	return __FUNCTION__;
}

static const char* test3(graph_t* graph)
{
	/* create the nodes */
	pass_t* rp1 = create(graph);
	pass_t* rp2 = create(graph);
	pass_t* rp3 = create(graph);
	pass_t* rp4 = create(graph);
	pass_t* rp5 = create(graph);
	pass_t* rp6 = create(graph);

	/* setup the dependencies */
	set_nexts(graph, rp1, 2, rp2, rp3);
	set_nexts(graph, rp2, 1, rp3);
	set_nexts(graph, rp3, 2, rp6, rp4);
	set_nexts(graph, rp4, 2, rp6, rp5);
	// set_nexts(graph, rp6, 2, rp5, rp3); 
	set_nexts(graph, rp6, 3, rp5, rp3, rp2);

	return __FUNCTION__;
}

int main()
{
	graph_t* graph = create_graph(10);

	test_func_ptr_t tests[] = { test1,  test2 , test3 };
	u32 test_count = sizeof(tests) / sizeof(test_func_ptr_t);

	for(u32 i = 0; i < test_count; i++)
	{
		clear_graph(graph);
		const char* test_id = tests[i](graph);
		printf("test_id: %s\n", test_id);
		graph_dump(graph);
		dump_execution_sequence(graph);
		if((i + 1) != test_count)
			puts("");
	}

	return 0;
}
