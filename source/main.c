
#include <renderer/test.h>

#define RENDERER_INCLUDE_TIMING
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

#include <conio.h>

static allocate_result_t allocate(u32 size, u32 align)
{
	if((size == 0) || (align == 0))
		return (allocate_result_t) { ALLOCATE_RESULT_INVALID_REQUEST, NULL };

	void* ptr = heap_alloc(size);
	if(ptr == NULL)
		return (allocate_result_t) { ALLOCATE_RESULT_OUT_OF_MEMORY, NULL };

	return (allocate_result_t) { ALLOCATE_RESULT_SUCCESS, ptr };
}

static void deallocate(void* ptr)
{
	heap_free(ptr);
}

int main(int argc, const char** argv)
{
	alloc_init(&argv);

	memory_allocator_create_info_t create_info = 
	{
		.allocate = allocate,
		.deallocate = deallocate
	};
	memory_allocator_t* allocator = memory_allocator_create(&create_info);

	test_t* test = test_create(allocator, (argc > 1) ? argv[1] : "");
	AUTO driver = renderer_init(allocator, GPU_TYPE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, FULL_SCREEN ? true : false, RESIZABLE ? true : false);

	test->initialize(driver, test->user_data);

	time_handle_t tHandle = time_get_handle();

	while(renderer_is_running(driver))
	{
		if(kbhit())
		{
			if(getch() == 'd')
			{
				/* this should be pushed into another thread to not to halt the main thread! */
				debug_log_info("Building memory allocation tree...");
				memory_allocation_tree_t* tree = memory_allocator_build_allocation_tree(allocator);
				memory_allocation_tree_serialize_to_file(tree, "memory_allocation_tree.dump");
				memory_allocation_tree_destroy(tree);
				debug_log_info("Memory allocation tree built successfully");
			}
		}
		float deltaTime = time_get_delta_time(&tHandle);
		renderer_begin_frame(driver);
		test->render(driver, test->user_data);
		renderer_end_frame(driver);
		renderer_update(driver);
		test->update(driver, deltaTime, test->user_data);
	}

	test->terminate(driver, test->user_data);
	test_destroy(test);

	renderer_terminate(driver);
	memory_allocator_destroy(allocator);
	alloc_terminate();
	return 0;
}