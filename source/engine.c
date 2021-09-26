#include <engine/engine.h>
#include <stdlib.h>
#include <stdio.h>
#include <garbage_collector/garbage_collector.h>

engine_t* engine_init(uint32_t screen_width, uint32_t screen_height, const char* window_name)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	engine_t* engine = GC_NEW(engine_t);
	engine->window = glfwCreateWindow(screen_width, screen_height, window_name, NULL, NULL);
	engine->scene_manager = scene_manager_init();
	engine->renderer = renderer_init();
	return engine;
}

void engine_terminate(engine_t* engine)
{
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_destroy);
	renderer_terminate(engine_get_renderer(engine));

	glfwDestroyWindow((GLFWwindow*)(engine->window)); 
	glfwTerminate();
}

scene_manager_t* engine_get_scene_manager(engine_t* engine)
{
	return engine->scene_manager;
}

renderer_t* engine_get_renderer(engine_t* engine)
{
	return engine->renderer;
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