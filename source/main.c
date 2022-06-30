
#include <renderer/test.h>

#define RENDERER_INCLUDE_TIMING
#define RENDERER_INCLUDE_CORE
#include <renderer/renderer.h>

int main(int argc, const char** argv)
{
	memory_allocator_init(&argv);

	test_t* test;
	if(argc < 2)
		test = test_create("DEPTH_RENDER_TEXTURE");
	else 
		test = test_create(argv[1]);

	AUTO driver = renderer_init(GPU_TYPE, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, FULL_SCREEN, RESIZABLE);

	test->initialize(driver, test->user_data);

	time_handle_t tHandle = time_get_handle();

	while(renderer_is_running(driver))
	{
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
	memory_allocator_terminate();
	return 0;
}