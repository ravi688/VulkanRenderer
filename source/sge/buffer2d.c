/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: buffer2d.c is a part of VulkanRenderer

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

#include <sge/buffer2d.h>
#include <sge/bmp.h>
#include <sge/memory_allocator.h>
#include <common/hash_table.h>
#include <common/pair.h>

static rect2d_info_key_t generate_rect2d_info_key(buffer2d_t* buffer)
{
	buffer->counter++;
	return CAST_TO(rect2d_info_key_t, buffer->counter);
}

static rect2d_info_key_t add_vacant_rect(buffer2d_t* buffer, irect2d_t rect)
{
	rect2d_info_t rect_info =
	{
		.rect = rect,
		.area = IRECT2D_AREA(rect),
		.aspect_ratio = IRECT2D_ASPECT_RATIO(rect)
	};
	rect2d_info_key_t key = generate_rect2d_info_key(buffer);
	hash_table_add(&buffer->vacant_rects, (void*)&key, (void*)&rect_info);
	return key;
}

SGE_API buffer2d_t* buffer2d_new(memory_allocator_t* allocator)
{
	buffer2d_t* buffer = memory_allocator_alloc_obj(allocator, MEMORY_ALLOCATION_TYPE_OBJ_BUFFER2D, buffer2d_t);
	memzero(buffer, buffer2d_t);
	OBJECT_INIT(buffer, OBJECT_TYPE_BUFFER2D, OBJECT_NATIONALITY_INTERNAL);
	return buffer;
}

SGE_API buffer2d_t* buffer2d_create(memory_allocator_t* allocator, buffer2d_create_info_t* create_info)
{
	buffer2d_t* buffer = buffer2d_new(allocator);
	buffer2d_create_no_alloc(allocator, create_info, buffer);
	return buffer;
}

SGE_API void buffer2d_create_no_alloc(memory_allocator_t* allocator, buffer2d_create_info_t* create_info, buffer2d_t OUT buffer)
{
	debug_assert__(create_info->resize_mode == BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT, "For now we will only support BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT");

	OBJECT_MEMZERO(buffer, buffer2d_t);

	buffer->allocator = allocator;
	
	buffer->is_backed_buffer_owner = false,
	buffer->resize_mode = create_info->resize_mode,
	buffer->filled_rects = __hash_table_create(create_info->key_size,
									sizeof(filled_rect_info_t),
									128,
									128,
									create_info->key_comparer,
									create_info->key_hash_function, NULL),
	buffer->vacant_rects = hash_table_create(rect2d_info_key_t, rect2d_info_t, 512, 512, u32_equal_to, u32_hash, NULL),
	buffer->counter = 0;

	/* create 2d view if not supplied by the user */
	if(create_info->view != NULL)
		buffer->view = create_info->view;
	else
	{
		buffer2d_view_create_info_t view_create_info =
		{
			.size = create_info->size,
			.buffer = NULL
		};
		buffer->view = buffer2d_view_create(buffer->allocator, &view_create_info);
		buffer->is_view_owner = true;
	}

	_release_assert__((buffer->view->size.x == create_info->size.x) && (buffer->view->size.y == create_info->size.y));

	/* create linear buffer if not supplied by the user */
	buffer_t* backed_buffer = create_info->buffer;
	if((backed_buffer == NULL) && (buffer2d_view_get_backed_buffer_readonly(buffer->view) == NULL))
	{
		backed_buffer = BUFcreate(NULL, sizeof(u8), IEXTENT2D_AREA(create_info->size), 0);
		buffer->is_backed_buffer_owner = true;
	}

	if(buffer2d_view_get_backed_buffer_readonly(buffer->view) == NULL)
	{
		buf_clear(backed_buffer, NULL);
		buffer2d_view_set_buffer(buffer->view, backed_buffer);
	}

	/* initially the entire buffer is empty/vacant */
	add_vacant_rect(buffer, irect2d(ioffset2d(0, 0), buffer->view->size));
}

SGE_API void buffer2d_destroy(buffer2d_t* buffer)
{
	if(buffer->is_view_owner)
		buffer2d_view_destroy(buffer->view);
	if(buffer->is_backed_buffer_owner)
		buf_free(buffer2d_view_get_backed_buffer_readonly(buffer->view));
	hash_table_free(&buffer->filled_rects);
	hash_table_free(&buffer->vacant_rects);
}

SGE_API void buffer2d_release_resources(buffer2d_t* buffer)
{
	if(buffer->is_view_owner)
		buffer2d_view_release_resources(buffer->view);
	if(OBJECT_IS_INTERNAL(buffer))
		memory_allocator_dealloc(buffer->allocator, buffer);
}

SGE_API buffer_t* buffer2d_get_backed_buffer(buffer2d_t* buffer)
{
	return buffer2d_view_get_backed_buffer(buffer->view);
}

SGE_API buffer_t* buffer2d_get_backed_buffer_readonly(buffer2d_t* buffer)
{
	return buffer2d_view_get_backed_buffer_readonly(buffer->view);
}

SGE_API filled_rect_info_t* buffer2d_get_rect(buffer2d_t* buffer, void* key)
{
	return CAST_TO(filled_rect_info_t*, hash_table_get_value(&buffer->filled_rects, key));
}

SGE_API void buffer2d_get_rect_data(buffer2d_t* buffer, filled_rect_info_t* rect_info, void* out_data)
{
	buffer2d_view_get_rect(buffer->view, rect_info->rect_info.rect, out_data);
}

typedef struct _user_data_t
{
	f32 max_overlap;
	rect2d_info_key_t max_overlap_rect_key;
	u32 area;
	f32 aspect_ratio;
	iextent2d_t s;
} _user_data_t;

static void _find_best_matching_rect(void* key, void* value, void* user_data)
{
	AUTO rect_info = CAST_TO(rect2d_info_t*, value);
	AUTO data = CAST_TO(_user_data_t*, user_data);

	/* if smaller rectangle 's' is inside or lays over the 'rect_info->rect.extent' */
	if((data->s.width <= rect_info->rect.extent.width) && (data->s.height <= rect_info->rect.extent.height))
	{
		/* calculate how much area 's' covers */
		f32 overlap = CAST_TO(f32, data->area) / rect_info->area;

		/* if 's' covers more area than the previous max coverage then update the maximum overlap rectangle key  */
		if((overlap <= 1.0f) && (data->max_overlap < overlap))
		{
			data->max_overlap = overlap;
			data->max_overlap_rect_key = DEREF_TO(rect2d_info_key_t, key);
		}
	}
}

static rect2d_info_key_t find_best_matching_rect(buffer2d_t* buffer, iextent2d_t s)
{
	_user_data_t data =
	{
		.max_overlap = 0.0f,
		.max_overlap_rect_key = RECT2D_INFO_KEY_INVALID,
		.area = IEXTENT2D_AREA(s),
		.aspect_ratio = IEXTENT2D_ASPECT_RATIO(s),
		.s = s
	};
	hash_table_foreach(&buffer->vacant_rects, _find_best_matching_rect, &data);
	return data.max_overlap_rect_key;
}

static u32 get_double_size(u32 size) { return (size == 0) ? 1UL : (size << 1); }

static void add_filled_rect(buffer2d_t* buffer, irect2d_t rect, void* key PARAM_IF_DEBUG(icolor3_t color))
{
	filled_rect_info_t rect_info =
	{
		.rect_info =
			{
				.rect = rect,
				.area = IRECT2D_AREA(rect),
				.aspect_ratio = IRECT2D_ASPECT_RATIO(rect)
			}
		PARAM_IF_DEBUG(.color = color)
	};
	hash_table_add(&buffer->filled_rects, key, (void*)&rect_info);
}

static void write_data(buffer2d_t* buffer, irect2d_t rect, void* data, u32 size, bool is_broadcast)
{
	if(is_broadcast)
		buffer2d_view_broadcast_rect(buffer->view, rect, data, size);
	else
		buffer2d_view_set_rect(buffer->view, rect, data);
}

static bool is_inside(ioffset2d_t p, ioffset2d_t tl, ioffset2d_t br)
{
	return (p.x >= tl.x) && (p.x <= br.x) && (p.y >= br.y) && (p.y <= tl.y);
}

static irect2d_t _get_intersection(irect2d_t r1, irect2d_t r2, u32 _call_count)
{
	++_call_count;
	if(_call_count >= 3)
		return IRECT2D_NULL;

	ioffset2d_t tl1 = r1.offset;
	UNUSED_VARIABLE ioffset2d_t tr1 = { r1.offset.x + r1.extent.x, r1.offset.y };
	UNUSED_VARIABLE ioffset2d_t bl1 = { r1.offset.x, r1.offset.y + r1.extent.y };
	ioffset2d_t br1 = { r1.offset.x + r1.extent.x, r1.offset.y + r1.extent.y };
	ioffset2d_t tl2 = r2.offset;
	ioffset2d_t br2 = { r2.offset.x + r2.extent.x, r2.offset.y + r2.extent.y };

	bool is_inside_tl1 = is_inside(tl1, tl2, br2);
	bool is_inside_br1 = is_inside(br1, tl2, br2);

	if(is_inside_tl1 && !is_inside_br1)
		return irect2d(tl1, iextent2d(br2.x - tl1.x, br2.y - tl1.y));
	else if(is_inside_br1 && !is_inside_tl1)
		return irect2d(tl2, iextent2d(br1.x - tl2.x, br1.y - tl2.y));
	else if(is_inside_br1 && is_inside_tl1)
		return r1;
	else
		return _get_intersection(r2, r1, _call_count);
}

static irect2d_t get_intersection(irect2d_t r1, irect2d_t r2) { return _get_intersection(r1, r2, 0); }

static void subdivide_rect(buffer2d_t* buffer, rect2d_info_t* rect_info, iextent2d_t s, rect2d_info_key_t* const out_childs, u32 OUT child_count)
{
	irect2d_t r = rect_info->rect;
	u32 _child_count = 0;
	s64 ydiff = CAST_TO(s64, r.extent.height) - CAST_TO(s64, s.height);
	s64 xdiff = CAST_TO(s64, r.extent.width) - CAST_TO(s64, s.width);
 	if(ydiff > 0)
		out_childs[_child_count++] = add_vacant_rect(buffer, irect2d(ioffset2d(r.offset.x, r.offset.y + s.height), iextent2d(s.width, ydiff)));
	if(xdiff > 0)
		out_childs[_child_count++] = add_vacant_rect(buffer, irect2d(ioffset2d(r.offset.x + s.width, r.offset.y), iextent2d(xdiff, s.height)));
	if((xdiff > 0) && (ydiff > 0))
	{
		out_childs[_child_count++] = add_vacant_rect(buffer, irect2d(ioffset2d(r.offset.x + s.width, r.offset.y + s.height), iextent2d(xdiff, ydiff)));

		/* To increase packing ratio, we can enable these two lines of code, however we would need to add some extra lines of code to make it work! */
		// out_childs[_child_count++] = add_vacant_rect(buffer, irect2d(ioffset2d(r.offset.x + s.width, r.offset.y), iextent2d(xdiff, r.extent.height)));
		// out_childs[_child_count++] = add_vacant_rect(buffer, irect2d(ioffset2d(r.offset.x, r.offset.y + s.height), iextent2d(r.extent.width, ydiff)));
	}
	OUT child_count = _child_count;
}

static void recalculate_vacant_rects(buffer2d_t* buffer, rect2d_info_key_t vacant_rect_info_key, rect2d_info_t* vacant_rect_info, iextent2d_t s)
{
	rect2d_info_key_t childs[5];
	u32 child_count;
	/* subdivide the vacant rect into smaller rectangles */
	subdivide_rect(buffer, vacant_rect_info, s, childs, &child_count);

	/* recalculate vacant rects recursively for the childs */
	for(u32 i = 0; i < child_count; i++)
	{
		AUTO child_rect_info = CAST_TO(rect2d_info_t*, hash_table_get_value(&buffer->vacant_rects, &childs[i]));
		_debug_assert__(child_rect_info != NULL);

		/* check if 's' overlaps with any vacant child rect of this rect */
		irect2d_t _s = get_intersection(child_rect_info->rect, (irect2d_t) { vacant_rect_info->rect.offset, s });
		if(!irect2d_is_null(_s))
			recalculate_vacant_rects(buffer, childs[i], child_rect_info, _s.extent);
	}

	/* remove this rect from vacant rects as it has been subdivided into smaller rectangles */
	hash_table_remove(&buffer->vacant_rects, (void*)&vacant_rect_info_key);
}

static ioffset2d_t pack_data2d(buffer2d_t* buffer, iextent2d_t extent, void* data, u32 size, bool is_broadcast, bool OUT is_resized)
{
	/* find a rect which can accomodate a data of size { width, height } */
	rect2d_info_key_t vacant_rect_key = find_best_matching_rect(buffer, extent);

	bool _is_resized = false;
	/* keep expanding the buffer until an empty rect is found */
	while(vacant_rect_key == RECT2D_INFO_KEY_INVALID)
	{
		buffer2d_resize(buffer, get_double_size(buffer->view->size.x), get_double_size(buffer->view->size.y));
		vacant_rect_key = find_best_matching_rect(buffer, extent);
		_is_resized = true;
	}

	/* get the information of the rect */
	rect2d_info_t* rect_info = hash_table_get_value(&buffer->vacant_rects, &vacant_rect_key);

	_debug_assert__(rect_info != NULL);

	/* write the data to the buffer */
	if(data != NULL)
		write_data(buffer, irect2d(rect_info->rect.offset, extent), data, size, is_broadcast);

	/* save the offset as this vacant rect will be removed in 'recalculate_vacant_rects' */
	ioffset2d_t offset = rect_info->rect.offset;
	/* recalculates the list of empty rectangles which constitute the entire empty space */
	recalculate_vacant_rects(buffer, vacant_rect_key, rect_info, extent);

	if(is_resized != NULL)
		OUT is_resized = _is_resized;

	return offset;
}

#ifdef GLOBAL_DEBUG

static icolor3_t filled_colors[] =
{
	{ 255, 255, 255 },
	{ 0, 0, 0 },
	{ 0, 255, 0 },
	{ 255, 0, 0 },
	{ 255, 255, 0 },
	{ 128, 255, 0 },
	{ 255, 50, 0 },
	{ 255, 170, 0 }
};

SGE_API bool buffer2d_push_debug(buffer2d_t* buffer, void* key, void* value, u32 width, u32 height)
{
	static u32 counter = 0;
	static u32 colors_size = SIZEOF_ARRAY(filled_colors);
	counter = (1U + counter) % colors_size;
	return buffer2d_push(buffer, key, value, width, height, filled_colors[counter]);
}

#endif/*GLOBAL_DEBUG*/

SGE_API bool buffer2d_push(buffer2d_t* buffer, void* key, void* value, u32 width, u32 height PARAM_IF_DEBUG(icolor3_t color))
{
	/* pack the data */
	bool is_resized = false;
	ioffset2d_t offset = pack_data2d(buffer, iextent2d(width, height), value, 0 /* ignored */, false /* don't broadcast */, &is_resized);

	/* add the entry into the hash table 'elements' */
	add_filled_rect(buffer, irect2d(offset, iextent2d(width, height)), key PARAM_IF_DEBUG(color));

	return is_resized;
}

SGE_API bool buffer2d_push_broadcast(buffer2d_t* buffer, void* key, void* value, u32 size, u32 width, u32 height PARAM_IF_DEBUG(icolor3_t color))
{
	/* pack the data */
	bool is_resized = false;
	ioffset2d_t offset = pack_data2d(buffer, iextent2d(width, height), value, size, true, &is_resized);

	/* add the entry into the hash table 'elements' */
	add_filled_rect(buffer, irect2d(offset, iextent2d(width, height)), key PARAM_IF_DEBUG(color));

	return is_resized;
}

static void clear_vacant_rects(buffer2d_t* buffer)
{
	hash_table_clear(&buffer->vacant_rects);
	buffer->counter = 0;
	add_vacant_rect(buffer, irect2d(ioffset2d(0, 0), buffer->view->size));
}

typedef_pair_t(buffer2d_ptr_t, buffer2d_view_ptr_t);

static void push_again(void* key, void* value, void* user_data)
{
	AUTO filled_rect_info = CAST_TO(filled_rect_info_t*, value);

	AUTO pair = CAST_TO(pair_t(buffer2d_ptr_t, buffer2d_view_ptr_t)*, user_data);

	/* get the existing data for this rect */
	u8 data[IRECT2D_AREA(filled_rect_info->rect_info.rect)];
	buffer2d_view_get_rect(pair->second, filled_rect_info->rect_info.rect, data);

	/* pack it again and update the new position (offset) */
	DBG_BUFFER2D_RESIZE( rect2d_info_t before = filled_rect_info->rect_info );
	bool is_resized;
	AUTO offset = pack_data2d(pair->first, filled_rect_info->rect_info.rect.extent, data, 0, false, &is_resized);
	filled_rect_info->rect_info.rect.offset = offset;

	#if DBG_ENABLED(BUFFER2D_RESIZE)
		rect2d_info_t after = filled_rect_info->rect_info;
		if(pair->first->packed_rect_relocate_callback != NULL)
			pair->first->packed_rect_relocate_callback(&before, &after, pair->first->packed_rect_relocate_callback_handler_user_data);
	#endif /* DBG_BUFFER2D_RESIZE */

	_debug_assert__(is_resized == false);
}

SGE_API void buffer2d_resize(buffer2d_t* buffer, u32 num_rows, u32 num_columns)
{
	buffer2d_view_t* view = buffer->view;

	/* save the data temporarily as we need it later to repack the filled rects into resized buffer2d */
	buffer_t old_backed_buffer = buf_get_clone(buffer2d_view_get_backed_buffer_readonly(view));
	buffer2d_view_create_info_t view_create_info =
	{
		.size = view->size,
		.buffer = &old_backed_buffer
	};
	buffer2d_view_t old_view;
	buffer2d_view_create_no_alloc_ext(buffer->allocator, &view_create_info, &old_view);

	/* resize the backed buffer (it will invalid the exisiting written data and vacant rects ) */
	buffer2d_view_resize(view, num_rows, num_columns);

	/* clear the backed buffer as it is now invalidated and we need to write on it again */
	buffer2d_view_clear(view, NULL);

	/* clear the vacant rects as space has been resized so exisiting vacant rects are now invalidated */
	clear_vacant_rects(buffer);

	/* rebuild vacant rects and rewrite the exisiting data to make them consistent with new size */
	pair_t(buffer2d_ptr_t, buffer2d_view_ptr_t) user_data =
	{
		.first = buffer,
		.second = &old_view
	};
	hash_table_foreach(&buffer->filled_rects, push_again, (void*)&user_data);

	/* destroy the old data */
	buffer2d_view_destroy(&old_view);
	buffer2d_view_release_resources(&old_view);
	buf_free(&old_backed_buffer);
}

SGE_API void buffer2d_clear(buffer2d_t* buffer, void* clear_value)
{
	buffer2d_view_clear(buffer->view, clear_value);
	hash_table_clear(&buffer->filled_rects);
	clear_vacant_rects(buffer);
}

static void accumulate_area(void* key, void* value, void* user_data)
{
	DREF_TO(u32, user_data) += IRECT2D_AREA(CAST_TO(rect2d_info_t*, value)->rect);
}

SGE_API f32 buffer2d_get_packing_efficiency(buffer2d_t* buffer)
{
	u32 filled_area = 0;
	hash_table_foreach(&buffer->filled_rects, accumulate_area, &filled_area);
	return (f32)filled_area / IEXTENT2D_AREA(buffer->view->size);
}

#ifdef GLOBAL_DEBUG
static void dump_filled_rect(void* key, void* value, void* user_data)
{
	AUTO rect = CAST_TO(filled_rect_info_t*, value);

	AUTO _rect = irect2d(ioffset2d(rect->rect_info.rect.offset.x * sizeof(icolor3_t), rect->rect_info.rect.offset.y),
						 iextent2d(rect->rect_info.rect.extent.x * sizeof(icolor3_t), rect->rect_info.rect.extent.y));
	buffer2d_view_broadcast_rect(BUFFER2D_VIEW(user_data), _rect, &rect->color, sizeof(icolor3_t));
}

static icolor3_t vacant_colors[] =
{
	{ 50, 100, 255 },
	{ 60, 90, 255 },
	{ 70, 90, 255 },
	{ 50, 120, 255 },
	{ 70, 155, 255 },
	{ 0, 140, 255 }
};

static void dump_vacant_rect(void* key, void* value, void* user_data)
{
	AUTO rect = CAST_TO(rect2d_info_t*, value);

	AUTO _rect = irect2d(ioffset2d(rect->rect.offset.x * sizeof(icolor3_t), rect->rect.offset.y),
						 iextent2d(rect->rect.extent.x * sizeof(icolor3_t), rect->rect.extent.y));

	static u32 counter = 0;
	++counter;

	buffer2d_view_broadcast_rect(BUFFER2D_VIEW(user_data), _rect, &vacant_colors[counter % SIZEOF_ARRAY(vacant_colors)], sizeof(icolor3_t));
}

SGE_API void buffer2d_dump(buffer2d_t* buffer, const char* file_name)
{
	/* get the size of the bitmap dump */
	AUTO size = buffer->view->size;

	/* allocate pixel data buffer */
	buffer_t pixel_data = memory_allocator_buf_create(buffer->allocator, sizeof(icolor3_t), IEXTENT2D_AREA(size), 0);

	/* fill the pixel data buffer with WHITE color (as a background color) */
	icolor3_t default_color = ICOLOR3_GREY;
	/* NOTE: we can't use buf_clear here as it only clears upto the element_count which is zero in our case */
	buf_clear_buffer(&pixel_data, &default_color);

	/* buffer2d_view_create expects the backed_buffer's element size to be 1 as it only works with bytes */
	buf_set_element_size(&pixel_data, 1);
	buf_set_capacity(&pixel_data, sizeof(icolor3_t) * IEXTENT2D_AREA(size));

	/* create a 2d view of the pixel data buffer, to be able write pixel data with 2d location */
	buffer2d_view_create_info_t view_create_info =
	{
		.size = { size.x * sizeof(icolor3_t), size.y },
		.buffer = &pixel_data
	};
	buffer2d_view_t view;
	buffer2d_view_create_no_alloc_ext(buffer->allocator, &view_create_info, &view);

	/* write colors to the pixel data buffer */

	/* filled rectangles */
	hash_table_foreach(&buffer->filled_rects, dump_filled_rect, (void*)&view);

	/* vacant rectangles */
	hash_table_foreach(&buffer->vacant_rects, dump_vacant_rect, (void*)&view);

	/* save the pixel data buffer to a .bmp file */
	bmp_write(buf_get_ptr(&pixel_data), size.x, size.y, 3, file_name);

	/* destroy pixel data */
	buffer2d_view_destroy(&view);
	buffer2d_view_release_resources(&view);
	buf_free(&pixel_data);
}
#endif /* GLOBAL_DEBUG */
