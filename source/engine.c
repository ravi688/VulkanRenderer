#include <engine/renderer/renderer.h>
#include <engine/engine.h>
#include <stdlib.h>
#include <stdio.h>

engine_t* engine_init(uint32_t screen_width, uint32_t screen_height, const char* window_name)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, window_name, NULL, NULL); 


	// VkApplicationInfo application_info = {}; 
	// application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	// application_info.pNext = NULL; 
	// applicaiton_info.pApplicationName = "Vulkan Renderer"; 



	VkInstanceCreateInfo instance_create_info =  { };
	instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
	instance_create_info.pNext = NULL; 
	instance_create_info.flags = 0; 
	instance_create_info.pApplicationInfo = NULL; 
	instance_create_info.enabledLayerCount = 0;
	instance_create_info.ppEnabledLayerNames = NULL; 
	instance_create_info.enabledExtensionCount = 0; 
	instance_create_info.ppEnabledExtensionNames = NULL; 

	VkInstance instance = vk_create_instance(&instance_create_info, NULL); 
	tuple_t(uint32_t, pVkPhysicalDevice_t) physical_devices = vk_get_physical_devices(instance); 
	//iterate through each physical device connected to this host device
	for(uint32_t i = 0; i < physical_devices.value1; i++)
	{
		VkPhysicalDevice device = physical_devices.value2[i];
		VkPhysicalDeviceProperties properties = vk_get_physical_device_properties(device);
		log_msg(properties.deviceName);
	}

	engine_t* engine = (engine_t*)malloc(sizeof(engine_t)); 
	scene_manager_t* scene_manager = scene_manager_init();
	engine->window = (void*)window;
	engine->scene_manager = scene_manager;
	return engine;
}

void engine_terminate(engine_t* engine)
{
	scene_manager_for_each_objects_in_all_scenes(engine->scene_manager, object_destroy);
	glfwDestroyWindow((GLFWwindow*)(engine->window)); 
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