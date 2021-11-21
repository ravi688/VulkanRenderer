//For handling rendering
#include <renderer/renderer.h>

//For creating objects and attaching components on them
#include <ecs/object.h>
#include <ecs/scene_manager.h>
#include <Cube.h>

int main(int argc, char** argv)
{
	renderer_t* renderer = renderer_init(800, 800, "Vulkan 3D Renderer"); 
	// scene_manager_t* scene_manager = scene_manager_init();

	// object_t* cube = object_new("Cube");
	// scene_t* world = scene_new("3D World");
	// object_attach_component(cube, Cube);
	// scene_add_object(world, cube);
	// scene_manager_add_active_scene(world);

	// scene_manager_for_each_objects_in_all_scenes(scene_manager, object_call_awake);
	// scene_manager_for_each_objects_in_all_scenes(scene_manager, object_call_start);
	
	//TODO: render loop should run on separate thread -> render thread
	while(renderer_is_running(renderer))
	{
		//TODO: updates for each entity should run on separate thread -> main thread
		// scene_manager_for_each_objects_in_all_scenes(scene_manager, object_call_update);
		renderer_update(renderer);
	}

	// scene_manager_destroy(scene_manager);
	renderer_terminate(renderer);
	return 0;
}