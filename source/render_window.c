
#include <renderer/render_window.h>
#include <renderer/debug.h>

#include <exception/exception.h>
#include <memory_allocator/memory_allocator.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_wrapper.h>
#include <memory.h>


static void glfw_dump_required_extensions();
static tuple_t(uint32_t, ppVkChar_t) glfw_get_required_instance_extensions();
static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, VkInstance vk_instance);

#if GLOBAL_DEBUG
static void glfwErrorCallback(int code, const char* description)
{
	printf("GLFW: %d, %s", code, description);
}
#endif

static void glfwOnWindowResizeCallback(GLFWwindow* window, int width, int height)
{
	render_window_t* _window = glfwGetWindowUserPointer(window);
	_window->width = width;
	_window->height = height;
	if(_window->resize_event != NULL)
		for(int i = 0; i < _window->resize_event->element_count; i++)
			(*(void (**)(void*))buf_getptr_at(_window->resize_event, i))(_window);
}

render_window_t* render_window_init(u32 width, u32 height, const char* title)
{
	render_window_t* window = heap_new(render_window_t);
	memset(window, 0, sizeof(render_window_t));
	glfwInit();
#if GLOBAL_DEBUG
	glfwSetErrorCallback(glfwErrorCallback);
	glfw_dump_required_extensions();
#endif
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetFramebufferSizeCallback(window->handle, glfwOnWindowResizeCallback);
	glfwSetWindowUserPointer(window->handle, window);
	window->width = width;
	window->height = height;
	return window;
}

void render_window_subscribe_on_resize(render_window_t* window, void (*callback)(render_window_t* window))
{
	if(window->resize_event == NULL)
		window->resize_event = BUFcreate(NULL, sizeof(callback), 0, 0);
	buf_push(window->resize_event, &callback);
}

bool render_window_should_close(render_window_t* window)
{
	return glfwWindowShouldClose(window->handle);
}

void render_window_poll_events(render_window_t* window)
{
	glfwPollEvents();
}

void render_window_destroy(render_window_t* window)
{
	glfwDestroyWindow(window->handle);
	glfwTerminate();
	if(window->resize_event != NULL)
		buf_free(window->resize_event);
	heap_free(window);
}

void render_window_get_vulkan_surface(render_window_t* window, void* vk_instance, void* out_surface)
{
	VkSurfaceKHR surface;
	VkInstance instance;
	memcpy(&instance, vk_instance, sizeof(VkInstance));
	surface = glfw_get_vulkan_surface(window->handle, instance);
	memcpy(out_surface, &surface, sizeof(VkSurfaceKHR));
}

static void glfw_dump_required_extensions()
{
	tuple_t(uint32_t, ppVkChar_t) required_extensions = glfw_get_required_instance_extensions(); 
	log_msg("GLFW required_extensions ----------------\n");
	for(uint32_t i = 0; i < required_extensions.value1; i++)
		log_msg("%s\n", required_extensions.value2[i]);
EXCEPTION_BLOCK
(
	if(!vk_check_instance_extension_support(required_extensions))
		throw_exception(VULKAN_EXTENSION_NOT_SUPPORTED);
)
}

static tuple_t(uint32_t, ppVkChar_t) glfw_get_required_instance_extensions()
{
	tuple_t(uint32_t, ppVkChar_t) pair;
	//TODO: SafeMemory improvement: there should be remove_alloc also to unregister the pointer
	pair.value2 = add_alloc((ppVkChar_t)glfwGetRequiredInstanceExtensions(&(pair.value1)), pair.value1);
	return pair;
}

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, VkInstance vk_instance)
{
	VkSurfaceKHR surface; 
	vkCall(glfwCreateWindowSurface(vk_instance, window, NULL, &surface));
	return surface;
}

