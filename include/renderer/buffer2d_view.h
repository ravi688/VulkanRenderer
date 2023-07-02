#pragma once

#include <renderer/defines.h>
#include <bufferlib/buffer.h>
#include <renderer/rect.h> // iextent2d_t

typedef struct buffer2d_view_t
{
	buffer_t* backed_buffer;
	iextent2d_t size;
} buffer2d_view_t;

#define BUFFER2D_VIEW(ptr) DYNAMIC_CAST(buffer2d_view_t*, ptr)


BEGIN_CPP_COMPATIBLE

/* constructor and destructures */
RENDERER_API buffer2d_view_t buffer2d_view_create(buffer_t* backed_buffer, u32 width, u32 height);
static INLINE_IF_RELEASE_MODE void buffer2d_view_destroy(buffer2d_view_t* view) { }

RENDERER_API void buffer2d_view_resize(buffer2d_view_t* view, u32 width, u32 height);
RENDERER_API void buffer2d_view_clear(buffer2d_view_t* view);

/* getters */
static INLINE_IF_RELEASE_MODE buffer_t* buffer2d_view_get_backed_buffer(buffer2d_view_t* view) { return view->backed_buffer; }
static INLINE_IF_RELEASE_MODE iextent2d_t buffer2d_view_get_size(buffer2d_view_t* view) { return view->size; }
RENDERER_API void buffer2d_view_get_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* out_data);
static INLINE_IF_RELEASE_MODE void buffer2d_view_get_rect(buffer2d_view_t* view, irect2d_t rect, void* out_data)
{
	buffer2d_view_get_at(view, rect.offset.x, rect.offset.y, rect.extent.x, rect.extent.y, out_data);
}

/* setters */
RENDERER_API void buffer2d_view_set_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* in_data);
static INLINE_IF_RELEASE_MODE void buffer2d_view_set_rect(buffer2d_view_t* view, irect2d_t rect, void* in_data)
{
	buffer2d_view_set_at(view, rect.offset.x, rect.offset.y, rect.extent.x, rect.extent.y, in_data);
}
RENDERER_API void buffer2d_view_broadcast_rect(buffer2d_view_t* view, irect2d_t rect, void* in_value, u32 in_value_size);

END_CPP_COMPATIBLE
