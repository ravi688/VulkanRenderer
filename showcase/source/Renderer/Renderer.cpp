
#include <Renderer/Renderer.hpp>

#include <renderer/renderer.h>				// C header
#include <renderer/memory_allocator.h>		// C header, memory_allocator_init()


namespace V3D
{
	SHOWCASE_API Renderer::Renderer(RendererGPUType gpuType, u32 width, u32 height, const std::string& windowTitle, bool isFullScreen)
	{
		int _;
		memory_allocator_init(&_);
		handle = renderer_init((renderer_gpu_type_t)gpuType, width, height, windowTitle.c_str(), isFullScreen);
	}
	
	SHOWCASE_API void Renderer::init()
	{

	}

	SHOWCASE_API void Renderer::terminate()
	{
		renderer_terminate(handle);
		memory_allocator_terminate();
	}

	SHOWCASE_API bool Renderer::isRunning() const
	{
		return renderer_is_running(handle);
	}

	SHOWCASE_API void Renderer::beginFrame(float r, float g, float b, float a) const
	{
		renderer_begin_frame(handle, r, g, b, a);
	}

	SHOWCASE_API void Renderer::endFrame() const
	{
		renderer_end_frame(handle);
	}

	SHOWCASE_API void Renderer::update() const
	{
		renderer_update(handle);
	}

	SHOWCASE_API renderer_t* Renderer::getHandle() const
	{
		return handle;
	}

	SHOWCASE_API TextMesh Renderer::createTextMesh(GlyphMeshPool pool)
	{
		return TextMesh(this, &pool);
	}

	SHOWCASE_API GlyphMeshPool Renderer::createGlyphMeshPool(Font font)
	{
		return GlyphMeshPool(this, &font);
	}

}