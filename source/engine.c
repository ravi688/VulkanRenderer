#include <engine/engine.h>
#include <stdlib.h>
#include <stdio.h>
#include <garbage_collector/garbage_collector.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_wrapper.h>

typedef struct engine_t
{
	void* window;
	scene_manager_t* scene_manager;
	renderer_t* renderer;
} engine_t;

void* engine_get_window(engine_t* engine) { return engine->window; }
scene_manager_t* engine_get_scene_manager(engine_t* engine) { return engine->scene_manager; }
renderer_t* engine_get_renderer(engine_t* engine) { return engine->renderer; }

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, renderer_t* renderer);
static tuple_t(uint32_t, ppVkChar_t) glfw_get_required_instance_extensions();
static void glfw_dump_required_extensions();

engine_t* engine_init(uint32_t screen_width, uint32_t screen_height, const char* window_name)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//TODO: Swapchain recreation when window get resized
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	engine_t* engine = GC_NEW(engine_t);
	engine->window = glfwCreateWindow(screen_width, screen_height, window_name, NULL, NULL);
	engine->scene_manager = scene_manager_init();
	engine->renderer = renderer_init();
#if DEBUG
	glfw_dump_required_extensions();
#endif
	VkSurfaceKHR surface = glfw_get_vulkan_surface((GLFWwindow*)(engine->window), engine->renderer);
	renderer_init_surface(engine->renderer, (void*)(&surface), screen_width, screen_height);
	return engine;
}


static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, renderer_t* renderer)
{
	VkInstance instance = *((VkInstance*)renderer_get_vulkan_instance(renderer)); 
	VkSurfaceKHR surface; 
	vkCall(glfwCreateWindowSurface(instance, window, NULL, &surface));
	return surface;
}

static tuple_t(uint32_t, ppVkChar_t) glfw_get_required_instance_extensions()
{
	tuple_t(uint32_t, ppVkChar_t) pair;
	pair.value2 = (ppVkChar_t)glfwGetRequiredInstanceExtensions(&(pair.value1));
	return pair;
}

static void glfw_dump_required_extensions()
{
	tuple_t(uint32_t, ppVkChar_t) required_extensions = glfw_get_required_instance_extensions(); 
EXCEPTION_BLOCK
(
	if(!vk_check_instance_extension_support(required_extensions))
		throw_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
)
	puts("GLFW required_extensions ----------------");
	for(uint32_t i = 0; i < required_extensions.value1; i++)
		puts(required_extensions.value2[i]);
}

void engine_terminate(engine_t* engine)
{
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_destroy);
	renderer_terminate(engine_get_renderer(engine));

	glfwDestroyWindow((GLFWwindow*)(engine->window)); 
	glfwTerminate();
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
	renderer_update(engine->renderer);
}

bool engine_is_running(engine_t* engine)
{
	return !glfwWindowShouldClose(engine->window);
}

void engine_poll_events(engine_t* engine)
{
	glfwPollEvents();
}