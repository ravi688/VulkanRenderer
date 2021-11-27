
#pragma once

#include <renderer/defines.h>

#define raw_buffer_capacity(raw_buffer) (*__raw_buffer_capacity(raw_buffer))
#define raw_buffer_count(raw_buffer) (*__raw_buffer_count(raw_buffer))
#define raw_buffer_element_size(raw_buffer) (*__raw_buffer_element_size(raw_buffer))

u64* __raw_buffer_capacity(void* raw_buffer);
u64* __raw_buffer_count(void* raw_buffer);
u64* __raw_buffer_element_size(void* raw_buffer);

void* raw_buffer_ensure_capacity(void* raw_buffer, u64 capacity);
void* raw_buffer_push(void* raw_buffer, void* data);
void* raw_buffer_new();
void raw_buffer_clear(void* raw_buffer);
void raw_buffer_destroy(void* raw_buffer);
void raw_buffer_pop(void* raw_buffer);
