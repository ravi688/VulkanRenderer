
#include <Renderer/Renderer.hpp>
#include <Renderer/Mesh3D.hpp>

#include <renderer/renderer.h>				// C header
#include <renderer/memory_allocator.h>		// C header, memory_allocator_init()
#include <stdarg.h> 						// C header, va_list, va_arg etc.

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

	SHOWCASE_API TextMesh Renderer::createTextMesh(GlyphMeshPool pool) const
	{
		return TextMesh(const_cast<Renderer&>(*this), pool);
	}

	SHOWCASE_API GlyphMeshPool Renderer::createGlyphMeshPool(Font font) const
	{
		return GlyphMeshPool(const_cast<Renderer&>(*this), font);
	}

	SHOWCASE_API Mesh Renderer::createMesh(Mesh3D data) const
	{
		return Mesh(const_cast<Renderer&>(*this), data);
	}

	SHOWCASE_API Material Renderer::createMaterial(Shader shader) const
	{
		return Material(const_cast<Renderer&>(*this), shader);
	}

	SHOWCASE_API Shader Renderer::loadShader(const std::string& filePath) const
	{
		return Shader(const_cast<Renderer&>(*this), filePath);
	}

	SHOWCASE_API Texture Renderer::loadTexture(TextureType type, ...) const
	{
		va_list args;
		va_start(args, type);
		Texture texture(const_cast<Renderer&>(*this), type, args);
		va_end(args);
		return texture;
	}

	SHOWCASE_API Mesh Renderer::loadMesh(const std::string& filePath) const
	{
		Mesh3D data = Mesh3D::load(filePath.c_str());
		Mesh mesh(const_cast<Renderer&>(*this), data);
		data.drop();
		return mesh;
	}
}