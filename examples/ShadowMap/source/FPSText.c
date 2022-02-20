
#include <FPSText.h>

static void rebuildTransform(FPSText* this);

void FPSText_setPosition(FPSText* this, vec3f position)
{
	this->position = VEC3 { position.x, 2 * position.y / WINDOW_HEIGHT, 2 * position.z / WINDOW_WIDTH };
	this->position = vec3f_scale(this->position, this->fontSize);
	rebuildTransform(this);
}

void FPSText_setFontSize(FPSText* this, float size)
{
	this->fontSize = size;
	rebuildTransform(this);
}

static void onWindowResize(render_window_t* _, void* user_data)
{
	vec3f position = { };
	position.y = WINDOW_HEIGHT * 0.5f - 250.0f;
	position.z = -WINDOW_WIDTH * 0.5f + 250.0f;
	FPSText_setPosition(user_data, position);
}

static void rebuildTransform(FPSText* this)
{
	mat4_move(float)(&this->transform, 
		mat4f_mul(mat4f_scale(VEC3{ this->fontSize, this->fontSize, this->fontSize }),
				  mat4f_translate(this->position)));
}

void OnInstantiate(FPSText)
{
	BindOnDestroy(FPSText);
	BindOnRender(FPSText);
	BindOnStart(FPSText);
	BindOnUpdate(FPSText);

	this->shader = shader_load(RENDERER, "resource/shaders/TextShader.sb");
	material_create_info_t create_info = 
	{
		.is_vertex_attrib_from_file = true,
		.shader = this->shader
	};
	this->material = material_create(RENDERER, &create_info);

	this->font = font_load_and_create("resource/fonts/Roboto/Roboto-Bold.ttf");
	this->glyphPool = glyph_mesh_pool_create(RENDERER, this->font);
	this->textMesh = text_mesh_create(RENDERER, this->glyphPool);
	this->stringHandle = text_mesh_string_create(this->textMesh);
	FPSText_set(this, "FPS: 0");
	FPSText_setColor(this, VEC4 { 1, 1, 1, 1 });
	FPSText_setFontSize(this, 30);
	onWindowResize(NULL, this);
	render_window_subscribe_on_resize(WINDOW, onWindowResize, this);
	this->fpsCount = 0;
	this->fpsUpdateInterval = 50 MILLISECONDS;
}

void OnStart(FPSText)
{
	this->camera = scene_manager_find_component_of_type(SCENE_MANAGER, Camera);
	this->fpsTimeHandle = time_get_handle();
}

void OnRender(FPSText)
{
	material_bind(this->material);
	mat4f mvp = mat4f_mul(this->camera->screenProjection, this->transform);
	material_set_push_mat4(this->material, "push.mvpMatrix", mat4f_transpose(mvp));
	text_mesh_draw(this->textMesh);
}

void OnUpdate(FPSText)
{
	float elapsed = time_get_seconds(this->fpsTimeHandle);
	if(elapsed >= this->fpsUpdateInterval)
	{
		char buffer[32] = { };
		sprintf(buffer, "FPS: %u", (u32)(this->fpsCount / elapsed));
		FPSText_set(this, buffer);
		this->fpsTimeHandle = time_get_handle();
		this->fpsCount = 0;
	}
	this->fpsCount++;
}

void OnDestroy(FPSText)
{
	font_destroy(this->font);
	font_release_resources(this->font);
	glyph_mesh_pool_destroy(this->glyphPool);
	glyph_mesh_pool_release_resources(this->glyphPool);
	text_mesh_destroy(this->textMesh);
	text_mesh_release_resources(this->textMesh);
}

