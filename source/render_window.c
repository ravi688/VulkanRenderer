
#include <renderer/render_window.h>
#include <renderer/debug.h>
#include <renderer/assert.h>

#include <exception/exception.h>
#include <memory_allocator/memory_allocator.h>
#include <tuple/header_config.h>
#include <tuple/tuple.h>
#include <stdio.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_wrapper.h>
#include <memory.h>

typedef struct event_args_t
{
	void* user_data;
	void (*callback)(render_window_t* window, void* user_data);
} event_args_t;

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
		{
			event_args_t* args = buf_getptr_at(_window->resize_event, i);
			args->callback(_window, args->user_data);
		}
}

render_window_t* render_window_init(u32 width, u32 height, const char* title, bool full_screen)
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
	if(full_screen)
		window->handle = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
	else
		window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetFramebufferSizeCallback(window->handle, glfwOnWindowResizeCallback);
	glfwSetWindowUserPointer(window->handle, window);
	window->width = width;
	window->height = height;
	log_msg("Render window created successfully\n");
	return window;
}
static bool comparer(void* callback, event_args_t* args) { return callback == (void*)args->callback; }

void render_window_subscribe_on_resize(render_window_t* window, void (*callback)(render_window_t* window, void* user_data), void* user_data)
{
	if(window->resize_event == NULL)
		window->resize_event = BUFcreate(NULL, sizeof(event_args_t), 0, 0);
	event_args_t args = { .user_data = user_data, .callback = callback };
#if GLOBAL_DEBUG
	if(buf_find_index_of(window->resize_event, callback, (bool (*)(void*, void*))comparer) != BUF_INVALID_INDEX)
	{
		log_wrn("Event Handler %u is already subscribed\n", callback);
	}
#endif
	buf_push(window->resize_event, &args);
}


void render_window_unsubscribe_on_resize(render_window_t* window, void (*callback)(render_window_t* window, void* user_data))
{
	if(window->resize_event == NULL) return;
	bool result = buf_remove(window->resize_event, callback, (bool (*)(void*, void*))comparer);
	ASSERT(result == true, "Failed to unsubscribe %u from render_window_t::resize_event\n", callback);
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
	log_msg("Render window destroyed successfully\n");
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
	pair.value2 = (ppVkChar_t)glfwGetRequiredInstanceExtensions(&(pair.value1));
	return pair;
}

static VkSurfaceKHR glfw_get_vulkan_surface(GLFWwindow* window, VkInstance vk_instance)
{
	VkSurfaceKHR surface; 
	vkCall(glfwCreateWindowSurface(vk_instance, window, NULL, &surface));
	log_msg("Vulkan surface created successfully\n");
	return surface;
}

