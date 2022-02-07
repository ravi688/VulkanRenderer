
#pragma once


#include <renderer/defines.h>
#include <stdlib.h>

BEGIN_CPP_COMPATIBLE

// RENDERER_API extern tgc_t gc;

END_CPP_COMPATIBLE

#ifndef DEBUG
#	define GC_START(stack_address) //tgc_start(&gc, (void*)(stack_address))
#	define GC_STOP() //tgc_stop(&gc)
#	define GC_RUN() //tgc_run(&gc)
#	define GC_PAUSE() //tgc_pause(&gc)
#	define GC_RESUME() //tgc_resume(&gc)
#	define GC_ALLOC(size) malloc(size)
#	define GC_FREE(ptr) free(ptr)
#else
#	include <stdlib.h>
#	define GC_START(stack_address)
#	define GC_STOP()
#	define GC_RUN()
#	define GC_PAUSE()
#	define GC_RESUME()
#	define GC_ALLOC(size) malloc(size)
#	define GC_FREE(ptr) free(ptr)
#endif

#define GC_NEW(type) (type*)GCALLOC(sizeof(type))
#define GC_NEWV(type, count) (type*)GC_ALLOC(sizeof(type) * (count))
