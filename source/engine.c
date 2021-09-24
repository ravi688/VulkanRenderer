
#include <engine/engine.h>
#include <stdlib.h>

engine_t* engine_init(uint32_t screen_width, uint32_t screen_height, const char* window_name)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, window_name, NULL, NULL); 
	engine_t* engine = (engine_t*)malloc(sizeof(engine_t)); 
	

	scene_manager_t* scene_manager = scene_manager_init();
	engine->window = window;
	engine->scene_manager = scene_manager;
	return engine;
}

void engine_terminate(engine_t* engine)
{
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_destroy);
	glfwDestroyWindow(engine->window); 
	glfwTerminate();
}

scene_manager_t* engine_get_scene_manager(engine_t* engine)
{
	return engine->scene_manager;
}

void engine_start(engine_t* engine)
{
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_awake);
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_start);
}
void engine_update(engine_t* engine)
{
	//physics
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_update);
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_late_update);

	//rendering
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_on_pre_render);
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_call_on_post_render);	
}

bool engine_is_running(engine_t* engine)
{
	return !glfwWindowShouldClose(engine->window);
}

void engine_poll_events(engine_t* engine)
{
	glfwPollEvents();
}