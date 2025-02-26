
#define SGE_INCLUDE_CORE 
#include <sge-cpp/sge.hpp>

#include <sge/debug.h>

namespace SGE
{
	static sge_gpu_type_t getSGEGPUType(GPUType type)
	{
		switch(type)
		{
			case GPUType::Auto : return SGE_GPU_TYPE_AUTO;
			case GPUType::Integrated : return SGE_GPU_TYPE_INTEGRATED;
			case GPUType::Discrete : return SGE_GPU_TYPE_DISCRETE;
			case GPUType::CPU : return SGE_GPU_TYPE_CPU;
			default:
				debug_log_error("Unsupported GPUType is provided");
		};
		return SGE_GPU_TYPE_AUTO;
	}

	Driver::Driver(GPUType type,
				u32 width,
				u32 height,
				const std::string_view title,
				bool fullScreen,
				bool resizable,
				com::OptionalReference<const Requirements> requirements)
	{
		alloc_init(&argv);

		memory_allocator_create_info_t create_info = { };
		m_allocator = memory_allocator_create(&create_info);

		sge_driver_create_info_t driver_create_info = { };
		driver_create_info.gpu_type = getSGEGPUType(type);
		driver_create_info.width = width;
		driver_create_info.height = height;
		// Make sure we have null-terminated string; std::string_view may not be null-terminated
		std::string titleStr { title };
		driver_create_info.title = titleStr.c_str();
		driver_create_info.full_screen = fullScreen;
		driver_create_info.resizable = resizable;
		if(requirements.has_value())
		{
			driver_create_info.max_point_lights = requirements->maxPointLights;
			driver_create_info.max_spot_lights = requirements->maxSpotLights;
			driver_create_info.max_far_lights = requirements->maxFarLights;
			driver_create_info.require_bitmap_text = requirements->bitmapText;
		}
		m_handle = renderer_init(m_allocator, &driver_create_info);
	}

	Driver::~Driver()
	{ 
		/* we need to wait for the device to finish any pending tasks, so that all the resources/objects will be unreferenced
		 * , as test->terminate() destroys the test related objects etc. */
		renderer_wait_idle(m_handle);

		renderer_terminate(m_handle);
		memory_allocator_destroy(m_allocator);
		alloc_terminate();
		m_handle = NULL;
	}
}