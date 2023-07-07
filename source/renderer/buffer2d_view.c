#include <renderer/buffer2d_view.h>

RENDERER_API buffer2d_view_t buffer2d_view_create(buffer_t* backed_buffer, u32 width, u32 height)
{
	_debug_assert__(backed_buffer != NULL);
	_debug_assert__(buf_get_element_size(backed_buffer) == 1);
	if(buf_get_capacity(backed_buffer) < (width * height))
		buf_resize(backed_buffer, width * height);
	buf_set_element_count(backed_buffer, buf_get_capacity(backed_buffer));
	return (buffer2d_view_t) { .backed_buffer = backed_buffer, .size = { width, height } };
}

RENDERER_API void buffer2d_view_resize(buffer2d_view_t* view, u32 width, u32 height)
{
	if((view->size.x == width) && (view->size.y == height))
		return;
	buf_set_element_count(view->backed_buffer, 0);
	buf_resize(view->backed_buffer, width * height);
	buf_set_element_count(view->backed_buffer, width * height);
	view->size = iextent2d(width, height);
}

RENDERER_API void buffer2d_view_clear(buffer2d_view_t* view, void* clear_value)
{
	buf_clear(view->backed_buffer, clear_value);
	buf_set_element_count(view->backed_buffer, IEXTENT2D_AREA(view->size));
	_debug_assert__(buf_get_element_count(view->backed_buffer) == buf_get_capacity(view->backed_buffer));
}

RENDERER_API void buffer2d_view_set_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* in_data)
{
	_debug_assert__((loc_x + size_x) <= view->size.x);
	_debug_assert__((loc_y + size_y) <= view->size.y);

	u8* _data = CAST_TO(u8*, in_data);
	for(u32 i = 0, k = 0; i < size_y; i++)
		for(u32 j = 0; j < size_x; j++, k++)
			buf_set_at(view->backed_buffer, (loc_y + i) * view->size.x + (loc_x + j), &_data[k]);
}

RENDERER_API void buffer2d_view_broadcast_rect(buffer2d_view_t* view, irect2d_t rect, void* in_value, u32 in_value_size)
{
	debug_assert__((rect.extent.x % in_value_size) == 0, "rect.extent.x (row size) must be multiple of in_value_size");

	u8* _data = CAST_TO(u8*, in_value);
	for(u32 i = 0; i < rect.extent.y; i++)
		for(u32 j = 0; j < rect.extent.x; j++)
			buf_set_at(view->backed_buffer, (rect.offset.y + i) * view->size.x + (rect.offset.x + j), &_data[j % in_value_size]);
}

RENDERER_API void buffer2d_view_get_at(buffer2d_view_t* view, u32 loc_x, u32 loc_y, u32 size_x, u32 size_y, void* out_data)
{
	_debug_assert__((loc_x + size_x) <= view->size.x);
	_debug_assert__((loc_y + size_y) <= view->size.y);

	u8* _data = CAST_TO(u8*, out_data);
	for(u32 i = 0, k = 0; i < size_y; i++)
		for(u32 j = 0; j < size_x; j++, k++)
			buf_get_at(view->backed_buffer, (loc_y + i) * view->size.x + (loc_x + j), &_data[k]);
}

