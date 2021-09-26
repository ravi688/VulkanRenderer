
#include <garbage_collector/garbage_collector.h>


#include <engine/engine.h>
#include "TestBehaviour.h"
#include "Ammo.h"

static void prepare_scene(engine_t* engine);

int main(int argc, char** argv)
{
	GC_START(&argc);

	engine_t* engine = engine_init(600, 500, "Vulkan 3D Engine"); 

	//prepare_scene(engine);

	engine_start(engine);
	while(engine_is_running(engine))
	{
		engine_update(engine);
		engine_poll_events(engine);
	}

	engine_terminate(engine);

	GC_RUN();
	GC_STOP();
	return 0;
}

static scene_manager_t* sceneManager;

static void prepare_scene(engine_t* engine)
{
	sceneManager = engine_get_scene_manager(engine);
	scene_t* my_scene = scene_new("MyScene");
	scene_manager_add_active_scene(sceneManager, my_scene);

	object_t* object = object_new("Cube"); 
	object_attach_component(object, TestBehaviour);
	object_attach_component(object, mesh_renderer_t);
	object_attach_component(object, mesh_filter_t);
	scene_add_object(my_scene, object); 
}