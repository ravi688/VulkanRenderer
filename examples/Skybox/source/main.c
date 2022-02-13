// rendering systems
#include <renderer/renderer.h>
#include <renderer/material.h>
#include <renderer/shader.h>
#include <renderer/mesh.h>
#include <renderer/texture.h>
#include <renderer/render_window.h>
#include <renderer/text_mesh.h>

// utilities and non-rendering systems
#include <renderer/font.h>
#include <renderer/time.h>
#include <renderer/mesh3d.h>
#include <renderer/memory_allocator.h>

// scene management
#include <ecs/scene_manager.h>

// maths
#include <hpml/memory.h>
#include <hpml/vec2.h>
#include <hpml/vec3.h>
#include <hpml/vec4.h>
#include <hpml/mat4.h>
#include <hpml/mat2.h>
#include <hpml/affine_transformation.h>

// game objects and components
#include <Rock.h>
#include <Camera.h>

static void setup_scene(scene_t* scene)
{
	object_t* skyboxObj = object_create("Skybox");
	object_t* rockObj = object_create("Rock");
	object_t* cameraObj = object_create("Camera");
	scene_add_object(scene, skyboxObj);
	scene_add_object(scene, rockObj);
	scene_add_object(scene, cameraObj);

	object_attach_component(rockObj, Rock);
	object_attach_component(cameraObj, Camera);
}

int main()
{
	memory_allocator_init(&argc);
	renderer_t* renderer = renderer_init(RENDERER_GPU_TYPE_DISCRETE, 800, 800, "Skybox", false);
	
	scene_manager_t* sceneManager = scene_manager_create();

	scene_t* skyboxScene = scene_create("SkyboxScene");
	setup_scene(skyboxScene);
	scene_manager_add_active_scene(sceneManager, skyboxScene);

	scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_awake);
	scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_start);


	while(renderer_is_running(renderer))
	{
		scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_update);
		renderer_begin_frame(renderer, 0, 0, 0, 0);
		scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_on_pre_render);
		scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_on_render);
		scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_on_post_render);
		renderer_end_frame(renderer);
		scene_manager_for_each_objects_in_all_scenes(sceneManager, object_call_late_update);
		renderer_update(renderer);
	}

	scene_manager_destroy(sceneManager);
	renderer_terminate(renderer);
	memory_allocator_terminate();
}
