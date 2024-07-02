
#include <sutk/debug.h>
#include <sutk/assert.h>

#define SGE_INCLUDE_CORE
#include <sge/sge.h>

int main(int argc, char** argv)
{
	debug_log_info("SUTK Repository!");

	alloc_init(&argv);

	memory_allocator_create_info_t create_info = { };
	memory_allocator_t* allocator = memory_allocator_create(&create_info);

	renderer_create_info_t driver_create_info = { };
	driver_create_info.gpu_type = SGE_GPU_TYPE_DISCRETE;
	driver_create_info.width = 800;
	driver_create_info.height = 800;
	driver_create_info.title = argv[0];
	driver_create_info.full_screen = false;
	driver_create_info.resizable = true;
	
	AUTO driver = renderer_init(allocator, &driver_create_info);

	while(renderer_is_running(driver))
	{
		/* record command buffers for this frame */
		renderer_begin_frame(driver);

		renderer_end_frame(driver);

		/* dispatch command buffers for this frame for execution 
		 * NOTE: if frame pipelining is disabled while creating the driver then call to renderer_update will block the execution
		 * on host until rendering commands for this frame finish their execution */
		renderer_update(driver);
	}

	/* we need to wait for the device to finish any pending tasks, so that all the resources/objects will be unreferenced
	 * , as test->terminate() destroys the test related objects etc. */
	renderer_wait_idle(driver);

	renderer_terminate(driver);
	memory_allocator_destroy(allocator);
	alloc_terminate();
	return 0;
}
