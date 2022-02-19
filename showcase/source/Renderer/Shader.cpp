
#include <Renderer/Shader.hpp>
#include <Renderer/Renderer.hpp>

namespace V3D
{
	SHOWCASE_API Shader::Shader(Renderer& renderer, const std::string& filePath)
	{
		handle = shader_load(renderer.handle, filePath.c_str());
	}

	SHOWCASE_API void Shader::destroy() const
	{
		shader_destroy(handle);
	}

	SHOWCASE_API void Shader::releaseResources() const
	{
		shader_release_resources(handle);
	}

	SHOWCASE_API void Shader::drop() const
	{
		shader_destroy(handle);
		shader_release_resources(handle);
	}
}
