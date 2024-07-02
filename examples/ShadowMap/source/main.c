// rendering systems
#include <renderer/renderer.h>
#include <renderer/render_window.h>
#include <renderer/memory_allocator.h>

// scene management
#include <ecs/scene_manager.h>

// game objects and components
#include <Cube.h>
#include <Plane.h>
#include <Camera.h>
#include <FPSText.h>

#include <renderer/time.h>

static void setup_scene(scene_t* scene)
{
	object_t* skyboxObj = object_create("Plane");
	object_t* cubeObj = object_create("Cube");
	object_t* planeObj = object_create("Plane");
	object_t* fpsObj = object_create("FPS Text");
	object_t* cameraObj = object_create("Camera");
	// camera should be added first
	scene_add_object(scene, cameraObj);

	scene_add_object(scene, skyboxObj);
	scene_add_object(scene, cubeObj);
	scene_add_object(scene, planeObj);
	scene_add_object(scene, fpsObj);

	object_attach_component(cubeObj, Cube);
	object_attach_component(planeObj, Plane);
	object_attach_component(cameraObj, Camera);
	object_attach_component(fpsObj, FPSText);
}

render_window_t* WINDOW;
renderer_t* RENDERER;
scene_manager_t* SCENE_MANAGER;
float DELTA_TIME;
float TIME;

int main()
{
	memory_allocator_init(&argc);
	RENDERER = renderer_init(SGE_GPU_TYPE_DISCRETE, 800, 800, "Shadow map", false);
	WINDOW = renderer_get_window(RENDERER);

	SCENE_MANAGER = scene_manager_create();

	scene_t* skyboxScene = scene_create("SkyboxScene");
	setup_scene(skyboxScene);
	scene_manager_add_active_scene(SCENE_MANAGER, skyboxScene);

	time_handle_t deltaTimeHandle = time_get_handle();
	time_handle_t timeHandle = time_get_handle();

	scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_awake);
	scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_start);


	while(renderer_is_running(RENDERER))
	{
		DELTA_TIME = time_get_delta_time(&deltaTimeHandle);
		TIME = time_get_seconds(timeHandle);

		scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_update);
		scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_on_pre_render);
		scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_on_render);
		scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_on_post_render);
		scene_manager_for_each_objects_in_all_scenes(SCENE_MANAGER, object_call_late_update);
	}

	scene_manager_destroy(SCENE_MANAGER);
	renderer_terminate(RENDERER);
	memory_allocator_terminate();
}
