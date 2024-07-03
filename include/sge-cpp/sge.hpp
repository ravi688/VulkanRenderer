
#include <sge/defines.hpp>
#include <sge/sge.h>

#include <string> /* for std::string */

namespace SGE
{
	enum class GPUType : u8
	{
		Auto = 0,
		Integrated,
		Discrete,
		CPU
	};

	struct Requirements
	{
		u32 maxPointLights;
		u32 maxSpotLights;
		u32 maxFarLights;

		Requirements() noexcept : maxPointLights(0), maxSpotLights(0), maxFarLights(0) { }
	};

	class SGE_API Driver final
	{
	private:
		memory_allocator_t* m_allocator;
		renderer_t* m_driver;

	public:
		Driver(GPUType type = GPUType::Discrete, 
				u32 width = 800, 
				u32 height = 800, 
				const std::string& title = "SGE",
				bool fullScreen = false,
				bool resizable = true,
				com::OptionalReference<Requirements> requirements = { });
		~Driver();

		void update() { renderer_update(m_driver); }
		bool isRunning() { return renderer_is_running(m_driver); }
		void beginFrame() { renderer_begin_frame(m_driver); }
		void endFrame() { renderer_end_frame(m_driver); }
		void waitIdle() { renderer_wait_idle(m_driver); }
	};
};