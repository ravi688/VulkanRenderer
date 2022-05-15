//For handling rendering
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/render_window.h>
#include <renderer/time.h>
#include <renderer/render_queue.h>
#include <renderer/render_object.h>
#include <renderer/camera.h>

//For handling text/font rendering
#include <renderer/font.h>
#include <renderer/text_mesh.h>
// #include <renderer/text.h>

#include <renderer/mesh3d.h>
#include <renderer/defines.h>
#include <renderer/assert.h>

#include <renderer/memory_allocator.h>
#include <hpml/memory/header_config.h>
#include <hpml/memory/memory.h>
#include <hpml/vec2/header_config.h>
#include <hpml/vec2/vec2.h>
#include <hpml/vec3/header_config.h>
#include <hpml/vec3/vec3.h>
#include <hpml/vec4/header_config.h>
#include <hpml/vec4/vec4.h>
#include <hpml/mat4/header_config.h>
#include <hpml/mat4/mat4.h>
#include <hpml/mat2/header_config.h>
#include <hpml/mat2/mat2.h>
#include <hpml/affine_transformation/header_config.h>
#include <hpml/affine_transformation/affine_transformation.h>

#define DEG2RAD 0.01745f
#define RAD2DEG 57.29577f

static mat4_t(float) mat4_transform(vec3_t(float) position, vec3_t(float) eulerRotation) { return mat4_mul(float)(2, mat4_translation(float)(position.x, position.y, position.z), mat4_rotation(float)(eulerRotation.x, eulerRotation.y, eulerRotation.z)); }

static void recreate_matrix(render_window_t* window, void* user_data)
{
	Game* game = user_data;
	mat4_move(float)(&game->projectionMatrix,  mat4_persp_projection(float)(0.04f, 100, 60 * DEG2RAD, (float)window->width/window->height));
	// mat4_move(float)(&projectionMatrix,  mat4_ortho_projection(float)(0, 10, 3, (float)window->width/window->height));
	mat4_move(float)(&game->screenSpaceMatrix, mat4_ortho_projection(float)(-0.04f, 100, window->height, (float)window->width / window->height));
}

static void u32_to_string(u32 value, char* string)
{
	sprintf(string, "%u", value);
}

typedef struct Game
{
	renderer_t* renderer;
	render_queue_t* geometryQueue;
	render_queue_t* uiQueue;

	render_objects_t** renderObjects;

	material_t* textMaterial;
	material_t* gameUIMaterial;
	material_t* rockMaterial;
	material_t* skyboxMaterial;
	material_t* groundMaterial;
	font_t* font;
	glyph_mesh_pool_t* pool;
	text_mesh_t* textMesh;
	mesh_t* rockMesh;
	mesh_t* skyboxMesh;
	mesh_t* quadMesh;
	mesh_t* groundPlaneMesh;

	text_mesh_string_handle_t scoreTextHandle;
	time_handle_t secondTimeHandle;
	float deltaTime;
	float gameTime;

	mat4_t(float) cameraTransform;
	mat4_t(float) viewMatrix;
	mat4_t(float) clipMatrix;
} Game;


static void setup_shaders(Game* library);
static void setup_materials(Game* library);
static void setup_render_queues(Game* game);
static void update(Game* game);
static void game_release_resources(Game* game);
static void setup_camera_and_light(Game* game);

int main(int argc, char** argv)
{
	memory_allocator_init(&argc);

	Game game;

	// setup the renderer
	game.renderer = renderer_init(RENDERER_GPU_TYPE_DISCRETE, 800, 800, "Vulkan 3D Renderer", false);
	recreate_matrix(renderer_get_window(game.renderer), &game);
	render_window_subscribe_on_resize(renderer_get_window(game.renderer), recreate_matrix, &game);

	// setup the camera
	setup_camera_and_light(&game);

	// setup the shader & material libraries
	setup_shaders(game);
	setup_materials(game);

	// setup the render queues
	game.geometryQueue = render_queue_create(game.renderer, "Geometry");
	game.uiQueue = render_queue_create(game.renderer, "UI");
	setup_render_queues(&game);

	// setup the time
	time_handle_t frame_time_handle = time_get_handle();
	time_handle_t game_time_handle = time_get_handle();
	game.secondTimeHandle = time_get_handle();
	game.frameCount = 0;
	game.angle = 0;

	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(game.renderer))
	{
		/* Updates */
		game.deltaTime = time_get_delta_time(&frame_time_handle);
		game.gameTime = time_get_seconds(game_time_handle);
		update(&game);

		/* Rendering */

		// begin the command buffer recording
		renderer_begin_frame(game.renderer, 0.01f, 0.1f, 0.2f, 0.4f);

		render_queue_dispatch(game.geometryQueue);
		render_queue_dispatch(game.uiQueue);

		// end command buffer recording
		renderer_end_frame(game.renderer);

		// submit the work to the GPU and poll events for the rendering window
		renderer_update(game.renderer);
	}

	render_queue_destroy(game.geometryQueue);
	render_queue_destroy(game.uiQueue);
	render_queue_release_resources(game.geometryQueue);
	render_queue_release_resources(game.uiQueue);

	game_release_resources(&game);

	renderer_terminate(game.renderer);
	memory_allocator_terminate();
	return 0;
}

static void setup_camera_and_light(Game* game)
{
	game->camera = camera_create(game->renderer, PROJECTION_TYPE_PERSPECTIVE, 65 DEG);
	game->light = light_create(game->renderer);
}

static void setup_render_queues(Game* game)
{
	/*------TEXT-----------------------------*/
	game->textMaterial = material_library_get(game->materialLibrary, "TextMaterial");

	game->font = font_load_and_create("showcase/resource/fonts/Pushster-Regular.ttf");
	game->pool = glyph_mesh_pool_create(game->renderer, game->font);
	// mesh_t* glyph_A = glyph_mesh_pool_get_mesh(pool, 'A');
	// mesh_t* glyph_B = glyph_mesh_pool_get_mesh(pool, 'B');
	game->textMesh = text_mesh_create(game->renderer, game->pool);
	game->fpsTextHandle = text_mesh_string_create(game->textMesh);
	text_mesh_string_set_positionH(game->textMesh, game->fpsTextHandle, vec3(float)(0, 0, -3));
	text_mesh_string_handle_t module_name_handle = text_mesh_string_create(game->textMesh);
	text_mesh_string_set_positionH(game->textMesh, module_name_handle, vec3(float)(0, 2, -3));
	text_mesh_string_set_scaleH(game->textMesh, module_name_handle, vec3(float)(1.5f, 1.5f, 1.5f));
	text_mesh_string_setH(game->textMesh, module_name_handle, "Hello World");
	text_mesh_string_handle_t version_name_handle = text_mesh_string_create(game->textMesh);
	text_mesh_string_set_positionH(game->textMesh, version_name_handle, vec3(float)(0, -2, -3));
	text_mesh_string_set_scaleH(game->textMesh, version_name_handle, vec3(float)(0.7f, 0.7f, 0.7f));
	text_mesh_string_setH(game->textMesh, version_name_handle, "v1.0.0.0.0");
	text_mesh_string_handle_t cpu_usage = text_mesh_string_create(game->textMesh);
	text_mesh_string_set_positionH(game->textMesh, cpu_usage, vec3(float)(0, -4, -3));
	text_mesh_string_set_scaleH(game->textMesh, cpu_usage, vec3(float)(0.7f, 0.7f, 0.7f));
	text_mesh_string_setH(game->textMesh, cpu_usage, "10.99999%");

	/*---------------------------------------*/

	/*-----------GAME UI----------------------*/
	game->gameUIMaterial = material_library_get(game->materialLibrary, "GameUIMaterial");

	game->gameUI = text_mesh_create(game->renderer, game->pool);
	game->scoreTextHandle = text_mesh_string_create(game->gameUI);
	text_mesh_string_set_positionH(game->gameUI, game->scoreTextHandle, vec3(float)(0, 7, 5));
	text_mesh_string_set_scaleH(game->gameUI, game->scoreTextHandle, vec3(float)(0.8f, 0.8f, 0.8f));

	/*--------PLAYGROUND----------------------*/
	mesh3d_t* rock_mesh3d = mesh3d_cube(0.4f);//mesh3d_load("showcase/resource/Rock/Rock.obj");
	mesh3d_make_centroid_origin(rock_mesh3d);
	mesh3d_calculate_tangents(rock_mesh3d);
	game->rockMaterial = material_library_get(game->materialLibrary, "BumpMaterial");
	game->rockTextures[0] = texture_load(game->renderer, TEXTURE_TYPE_ALBEDO, "showcase/resource/Rock/albedo.bmp"),
	game->rockTextures[1] = texture_load(game->renderer, TEXTURE_TYPE_NORMAL, "showcase/resource/Rock/normal.bmp")
	material_set_texture(game->rockMaterial, "albedo", rockTextures[0]);
	material_set_texture(game->rockMaterial, "normal_map", rockTextures[1]);
	game->rock = mesh_create(game->renderer, rock_mesh3d);
	mesh3d_destroy(rock_mesh3d);
	/*---------------------------------------*/

	/*----------- CUBEMAP - SKYBOX ----------------------*/
	game->skyboxTexture = texture_load(game->renderer, TEXTURE_TYPE_CUBE,
											"showcase/resource/skybox_textures/skybox/right.bmp",
											"showcase/resource/skybox_textures/skybox/left.bmp",
											"showcase/resource/skybox_textures/skybox/bottom.bmp",
											"showcase/resource/skybox_textures/skybox/top.bmp",
											"showcase/resource/skybox_textures/skybox/front.bmp", 
											"showcase/resource/skybox_textures/skybox/back.bmp");
	mesh3d_t* skybox_mesh3d = mesh3d_cube(5);
	mesh3d_flip_triangles(skybox_mesh3d);

	game->skyboxMaterial = material_library_get(game->materialLibrary, "SkyboxMaterial");
	material_set_texture(skyboxMaterial, "skybox", game->skyboxTexture);
	game->skybox = mesh_create(game->renderer, skybox_mesh3d);
	mesh3d_destroy(skybox_mesh3d);


	/*----------- QUAD ----------------------*/
	mesh3d_t* quad_mesh3d = mesh3d_plane(0.6f);
	game->quadMaterial = material_library_get(game->materialLibrary, "TransparentMaterial");
	game->quad = mesh_create(game->renderer, quad_mesh3d);


	/*-------------Ground Plane---------------*/
	mesh3d_transform_set(quad_mesh3d, mat4_mul(float)(2, mat4_scale(float)(4.1f, 4.1f, 4.1f), mat4_identity(float)()));
	game->groundMaterial = material_library_get(game->materialLibrary, "GroundMaterial");
	game->groundPlane = mesh_create(game->renderer, quad_mesh3d);
	mesh3d_destroy(quad_mesh3d);

	game->renderObjects = heap_newv(render_object_t*, 6);
	game->renderObjects[0] = render_object_create(game->rock, mesh_draw_indexed, game->rockMaterial);
	game->renderObjects[1] = render_object_create(game->skybox, mesh_draw_indexed, game->skyboxMaterial);
	game->renderObjects[2] = render_object_create(game->groundPlane, mesh_draw_indexed, game->groundMaterial);
	game->renderObjects[3] = render_object_create(game->quad, mesh_draw_indexed, game->quadMaterial);
	game->renderObjects[4] = render_object_create(game->textMesh, text_mesh_draw, game->textMaterial);
	game->renderObjects[5] = render_object_create(game->gameUI, text_mesh_draw, game->gameUIMaterial);

	for(int i = 0; i < 4; i++)
		render_queue_add(game->geometryQueue, game->renderObjects[i]);
	for(int i = 4; i < 6; i++)
		render_queue_add(game->uiQueue, game->renderObjects[i]);
	render_queue_build(game->geometryQueue);
	render_queue_build(game->uiQueue);
}

static void update(Game* game)
{
	material_set_float(game->textMaterial, "ubo.time", game->gameTime);
	material_set_float(game->gameUIMaterial, "ubo.time", game->gameTime);

	mat4_t(float) modelMatrix;
	mat4_move(float)(&modelMatrix, mat4_mul(float)(2, mat4_rotation(float)(game->angle * DEG2RAD, 45 * DEG2RAD, 0), mat4_scale(float)(2.2, 2.2, 2.2)));
	mat4_t(float) mvp = mat4_mul(float)(4, game->clipMatrix, game->projectionMatrix, game->viewMatrix, game->modelMatrix);
	material_set_push_mat4(game->rockMaterial, "push.mvp_matrix", mat4_transpose(float)(mvp));
	material_set_push_mat4(game->rockMaterial, "push.modelMatrix", mat4_transpose(float)(modelMatrix));

	mat4_t(float) skyboxMatrix = game->viewMatrix;
	// skybox should remain at origin, so set the last column of the viewMatrix to zero
	float** elements = (float**)mat4_data(float)(&skyboxMatrix);
	elements[0][3] = 0;
	elements[1][3] = 0;
	elements[2][3] = 0;
	elements[3][3] = 1;
	mat4_move(float)(&mvp, mat4_mul(float)(3, game->clipMatrix, game->projectionMatrix, game->skyboxMatrix));
	material_set_push_mat4(game->skyboxMaterial, "push.mvp_matrix", mat4_transpose(float)(mvp));

	mat4_move(float)(&modelMatrix, mat4_translation(float)(1, -1.0f, 0));
	mat4_move(float)(&mvp, mat4_mul(float)(4, game->clipMatrix, game->projectionMatrix, game->viewMatrix, modelMatrix));
	mat4_move(float)(&mvp, mat4_transpose(float)(mvp));
	material_set_push_mat4(game->groundMaterial, "push.mvp_matrix", mvp);
	material_set_push_mat4(game->groundMaterial, "push.modelMatrix", modelMatrix);

	mat4_move(float)(&modelMatrix, mat4_mul(float)(2, mat4_translation(float)(-0.8f, 0, 0), mat4_rotation(float)(0, 0, 80 * DEG2RAD)));
	mat4_move(float)(&mvp, mat4_mul(float)(4, game->clipMatrix, game->projectionMatrix, game->viewMatrix, modelMatrix));
	mat4_move(float)(&mvp, mat4_transpose(float)(mvp));
	material_set_push_mat4(game->quadMaterial, "push.mvp_matrix", mvp);

	mat4_t(float) canvas_transform = mat4_mul(float)(2, game->clipMatrix, game->screenSpaceMatrix);
	mat4_t(float) _modelMatrix = mat4_mul(float)(2, mat4_translation(float)(0, 0, 0), mat4_scale(float)(0, 50, 50));
	material_set_push_mat4(game->textMaterial, "push.mvp_matrix", mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _modelMatrix)));

	mat4_move(float)(&_modelMatrix, mat4_mul(float)(2, mat4_scale(float)(50, 50, 50), mat4_identity(float)()));
	material_set_push_mat4(game->gameUIMaterial, "push.mvp_matrix", mat4_transpose(float)(mat4_mul(float)(2, canvas_transform, _modelMatrix)));

	game->angle += 180 * game->delta_time * 0.1f;
	if(game->angle >= 360.0f)
		game->angle = 0.0f;

	float seconds = time_get_seconds(game->secondTimeHandle);
	if(seconds >= 0.05f)
	{
		u32 fps = (u32)((int)game->frameCount / seconds);
		char fps_string[32];
		strcpy(fps_string, "FPS: ");
		u32_to_string(fps, fps_string + strlen("FPS: "));
		text_mesh_string_setH(game->textMesh, game->fpsTextHandle, fps_string);
		strcpy(fps_string, "Score: ");
		u32_to_string(game->frameCount, fps_string + strlen("Score: "));
		text_mesh_string_setH(game->gameUI, game->scoreTextHandle, fps_string);
		game->secondTimeHandle = time_get_handle();
		game->frameCount = 0;
	}
	++(game->frameCount);
}

static void game_release_resources(Game* game)
{
	// destroy render objects and release resources
	for(int i = 0; i < 6; i++)
	{
		render_object_destroy(game->renderObjects[i]);
		render_object_release_resources(game->renderObjects[i]);
	}
	heap_free(game->renderObjects);

	// destroy fonts
	font_destroy(game->font);

	// destroy meshes
	glyph_mesh_pool_destroy(game->pool);
	text_mesh_destroy(game->gameUI);
	text_mesh_destroy(game->textMesh);
	mesh_destroy(game->rock);
	mesh_destroy(game->quad);
	mesh_destroy(game->groundPlane);
	mesh_destroy(game->skybox);

	// destroy textures
	texture_destroy(game->skyboxTexture);
	for(int i = 0; i < 2; i++)
		texture_destroy(game->rockTextures[i]);

	// release resources (extra heap allocated memory)

	font_release_resources(game->font);

	glyph_mesh_pool_release_resources(game->pool);
	text_mesh_release_resources(game->gameUI);
	text_mesh_release_resources(game->textMesh);
	mesh_release_resources(game->rock);
	mesh_release_resources(game->quad);
	mesh_release_resources(game->skybox);
	
	for(int i = 0; i < 2; i++)
		texture_release_resources(game->rockTextures[i]);
	texture_release_resources(game->skyboxTexture);

	camera_destroy(game->camera);
	camera_release_resources(game->camera);
	light_destroy(game->light);
	light_release_resources(game->light);
}


static void setup_shaders(Game* game)
{
	shader_library_t* library = renderer_get_shader_library(game->renderer);
	shader_library_load_shader(library, "showcase/resource/shaders/text_shader.sb");
	shader_library_load_shader(library, "showcase/resource/shaders/game_ui_shader.sb");
	shader_library_load_shader(library, "showcase/resource/shaders/bump_shader.sb");
	shader_library_load_shader(library, "showcase/resource/shaders/skybox_shader.sb");
	shader_library_load_shader(library, "showcase/resource/shaders/transparent_shader.sb");
	shader_library_load_shader(library, "showcase/resource/shaders/ground_shader.sb");
}

static void setup_materials(Game* game)
{
	material_library_t* library = renderer_get_material_library(game->renderer);
	material_library_load_material(library, "showcase/resource/materials/text_material.mat");
	material_library_load_material(library, "showcase/resource/materials/game_ui_material.mat");
	material_library_load_material(library, "showcase/resource/materials/bump_material.mat");
	material_library_load_material(library, "showcase/resource/materials/skybox_material.mat");
	material_library_load_material(library, "showcase/resource/materials/transparent_material.mat");
	material_library_load_material(library, "showcase/resource/materials/ground_material.mat");
}
