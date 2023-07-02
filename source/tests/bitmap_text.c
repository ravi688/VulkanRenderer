/*
	test id: BITMAP_TEXT
	Test to check if bitmap text is working fine
 */

#include <renderer/tests/bitmap_text.h>

#define RENDERER_INCLUDE_MATH
#define RENDERER_INCLUDE_3D_TEXT_RENDER_SYSTEM
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

#include <renderer/hash_table.h>
#include <renderer/bmp.h>
#include <renderer/color.h>
#include <renderer/buffer2d.h>

TEST_DATA(BITMAP_TEXT)
{
	render_scene_t* scene;

	camera_t* camera;

	glyph_mesh_pool_t* glyph_pool;
	font_t* font;

	text_mesh_t* text_mesh;
	text_mesh_string_handle_t string_handle;

	render_object_t* render_object;
	material_t* material;

	int isScreenSpace; /* bools are not support currently in V3DShader (see: TID64)*/
};


SETUP_TEST(BITMAP_TEXT);

TEST_ON_RENDERER_INITIALIZE(BITMAP_TEXT)
{
	return (renderer_initialization_data_t)
	{
		.window_name = "BITMAP_TEXT (press any key to switch from Screen space to World space and vice versa",
		.window_width = 800,
		.window_height = 800,
		.is_resizable = true,
		.is_fullscreen = false
	};
}

static void print_key_value_pair(void* key, void* value, void* user_data)
{
	debug_log_info("Pair: %s, %lu", *(char**)key, DREF_TO(u32, value));
}

static void test_hash_table()
{
	/* hash table test */
	hash_table_t table = hash_table_create(const char*, u32, 5, string_equal_to, string_hash);
	const char* str = "Hello World";
	u32 value = 100;
	hash_table_add(&table, &str, &value); value = 200; str = "Hello World 2";
	hash_table_add(&table, &str, &value); value = 300; str = "ABCD";
	hash_table_add(&table, &str, &value); value = 500; str = "3434";
	hash_table_add(&table, &str, &value); value = 5430; str = "3435";
	hash_table_add(&table, &str, &value); value = 505; str = "3436";
	hash_table_add(&table, &str, &value); value = 5023; str = "3437";

	str = "Hello World";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));
	str = "ABCD";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));
	str = "Hello World 2";
	debug_log_info("(\"%s\", %lu)", str, DREF_TO(u32, hash_table_get_value(&table, &str)));

	str = "ABCD";
	hash_table_remove(&table, &str);

	hash_table_foreach(&table, print_key_value_pair, NULL);	

	hash_table_free(&table);
}

void test_bitmap()
{
	/*f32 half_diag = sqrt(2) * 256;
	s32 wave_number = 1;*/
	s32 num_divisons = 8;
	icolor3_t data[512 * 512];
	for(s32 i = 0; i < 512; i++)
	{
		for(s32 j = 0; j < 512; j++)
		{
			/* ** Not sure why this is not working and giving completely unexpected results */
			/*			
			f32 t = vec2_magnitude(vec2_sub(vec2(i, j), vec2(256, 256))) / half_diag;
			t = t * wave_number * 3.14159f;
			AUTO color = color3(fmaxf(sin(t), 0), fmaxf(sin(t - 1.57f), 0), t);
			data[i * 512 + j] = icolor3(color.r * 255, color.g * 255, color.b * 255);
			*/			
			// s32 size = 512 / num_divisons;
			// s32 y = (i / size) * size;
			// s32 x = (j / size) * size;

			// if(((i >= x) && (i < (size + x))) && ((j >= y) < (size + y)))
			// 	data[i * 512 + j] = ICOLOR3_YELLOW;
			// else
			// 	data[i * 512 + j] = ICOLOR3_BLUE;

			if(i < 100)
				data[i * 512 + j] = ICOLOR3_YELLOW;
			else
				data[i * 512 + j] = ICOLOR3_BLUE;
		}
	}
	bmp_write(data, 512, 512, 3, "test_bitmap.bmp");
}

static void test_buffer2d_view()
{
	iextent2d_t size = { 512, 512 };
	buffer_t pixels = buf_create(sizeof(icolor3_t), IEXTENT2D_AREA(size), 0);
	AUTO default_color = ICOLOR3_WHITE;
	buf_clear_buffer(&pixels, &default_color);
	buf_set_element_size(&pixels, 1); // as buffer2d_view only works with element_size = 1 buffers
	buf_set_capacity(&pixels, sizeof(icolor3_t) * IEXTENT2D_AREA(size));

	buffer2d_view_t view = buffer2d_view_create(&pixels, size.x * sizeof(icolor3_t), size.y);

	icolor3_t colors[IEXTENT2D_AREA(size)];
	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_RED;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(32 * 3, 0), iextent2d(3 * 480, 32)), colors);

	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_BLUE;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(0, 32), iextent2d(32 * 3, 480)), colors);

	for(u32 i = 0; i < IEXTENT2D_AREA(size); i++)
		colors[i] = ICOLOR3_GREEN;

	buffer2d_view_set_rect(&view, irect2d(ioffset2d(32 * 3, 32), iextent2d(3 * 480, 480)), colors);

	bmp_write(buf_get_ptr(&pixels), size.x, size.y, 3, "buffer2d_view.bmp");

	buffer2d_view_destroy(&view);
	buf_free(&pixels);
}

typedef struct input_t
{
	u32 key;
	u32 width;
	u32 height;
} input_t;

static void test_buffer2d()
{
	buffer2d_create_info_t create_info = 
	{
		.size = { 512, 512 },
		.buffer = NULL,
		.resize_mode = BUFFER2D_RESIZE_MODE_ASPECT_RATIO_STRICT,
		.key_size = sizeof(u32),
		.key_comparer = u32_equal_to,
		.key_hash_function = u32_hash
	};
	buffer2d_t buffer = buffer2d_create(&create_info);

	icolor3_t colors[] = 
	{
		ICOLOR3_WHITE,
		ICOLOR3_BLACK,
		ICOLOR3_GREEN,
		ICOLOR3_BLUE,
		ICOLOR3_RED,
		ICOLOR3_YELLOW,
		ICOLOR3_ORANGE,
		ICOLOR3_BROWN,
		ICOLOR3_PINK
	};

	#define KEY __COUNTER__

	input_t inputs[] = 
	{
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 128, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 5, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 64, 128 },
		{ KEY, 480, 32 },
		{ KEY, 32, 448 },
		{ KEY, 5, 128 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
		{ KEY, 32, 32 },
	};

	u32 input_count = SIZEOF_ARRAY(inputs);
	for(u32 i = 0; i < input_count; i++)
	{
		IF_DEBUG( AUTO color = colors[i % SIZEOF_ARRAY(colors)] );
		buffer2d_push(&buffer, &inputs[i].key, NULL, inputs[i].width, inputs[i].height PARAM_IF_DEBUG(color));
	}

	IF_DEBUG( buffer2d_dump(&buffer, "buffer2d_dump.bmp") );

	debug_log_info("Packing Efficientcy: %f", buffer2d_get_packing_efficiency(&buffer));

	buffer2d_destroy(&buffer);
}

TEST_ON_INITIALIZE(BITMAP_TEXT)
{
	AUTO camera_system = renderer_get_camera_system(renderer);
	AUTO slib = renderer_get_shader_library(renderer);
	AUTO mlib = renderer_get_material_library(renderer);

	this->camera = camera_system_getH(camera_system,
		camera_system_create_camera(camera_system, CAMERA_PROJECTION_TYPE_PERSPECTIVE));
	camera_set_clear(this->camera, COLOR_GREEN, 1.0f);
	camera_set_active(this->camera, true);
	camera_set_transform(this->camera, mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

	this->scene = render_scene_create_from_mask(renderer, BIT64(RENDER_QUEUE_TYPE_GEOMETRY));

	this->material = material_library_getH(mlib, 
							material_library_create_materialH(mlib, 
							shader_library_load_shader(slib, 
								"shaders/builtins/text_shader.sb"), "MyMaterial"));
	this->isScreenSpace = 1;
	material_set_int(this->material, "parameters.isScreenSpace", 1);

	this->font = font_load_and_create(renderer->allocator, "showcase/resource/fonts/arial.ttf");
	this->glyph_pool = glyph_mesh_pool_create(renderer, this->font);
	this->text_mesh = text_mesh_create(renderer, this->glyph_pool);
	this->string_handle = text_mesh_string_create(this->text_mesh);
	text_mesh_string_setH(this->text_mesh, this->string_handle, "Screen Space");

	this->render_object = render_scene_getH(this->scene, render_scene_create_object(this->scene, RENDER_OBJECT_TYPE_TEXT_MESH, RENDER_QUEUE_TYPE_GEOMETRY));
	render_object_set_material(this->render_object, this->material);
	render_object_attach(this->render_object, this->text_mesh);
	render_object_set_transform(this->render_object, mat4_translation(0.0f, 0.0f, -3.0f));

	render_scene_build_queues(this->scene);

	test_hash_table();
	test_bitmap();
	test_buffer2d_view();
	test_buffer2d();
}

TEST_ON_TERMINATE(BITMAP_TEXT)
{
	text_mesh_destroy(this->text_mesh);
	text_mesh_release_resources(this->text_mesh);
	font_destroy(this->font);
	font_release_resources(this->font);
	glyph_mesh_pool_destroy(this->glyph_pool);
	glyph_mesh_pool_release_resources(this->glyph_pool);
	render_scene_destroy(this->scene);
	render_scene_release_resources(this->scene);
}


TEST_ON_UPDATE(BITMAP_TEXT)
{
	if(kbhit())
	{
		getch();
		this->isScreenSpace = !this->isScreenSpace;
		material_set_int(this->material, "parameters.isScreenSpace", this->isScreenSpace);
		switch(this->isScreenSpace)
		{
			case 0:
				text_mesh_string_setH(this->text_mesh, this->string_handle, "World Space");
				break;
			case 1:
				text_mesh_string_setH(this->text_mesh, this->string_handle, "Screen Space");
				break;
		}
	}
}

TEST_ON_RENDER(BITMAP_TEXT)
{
	render_scene_render(this->scene, RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
}

