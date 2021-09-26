
#pragma once


#include <tgc/tgc.h>

extern tgc_t gc;

#define GC_START(stack_address) tgc_start(&gc, (void*)(stack_address))
#define GC_STOP() tgc_stop(&gc)
#define GC_RUN() tgc_run(&gc)
#define GC_PAUSE() tgc_pause(&gc)
#define GC_RESUME() tgc_resume(&gc)
#define GC_ALLOC(size) tgc_alloc(&gc, size)
#define GC_FREE(ptr) tgc_free(&gc, ptr)

#define GC_NEW(type) (type*)GC_ALLOC(sizeof(type))
#define GC_NEWV(type, count) (type*)GC_ALLOC(sizeof(type) * (count))