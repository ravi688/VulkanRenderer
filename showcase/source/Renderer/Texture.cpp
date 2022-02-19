
#include <Renderer/Texture.hpp>
#include <Renderer/Renderer.hpp>
#include <stdarg.h>

namespace V3D
{
	SHOWCASE_API Texture::Texture(Renderer& renderer, TextureType type, va_list filePaths)
	{
		handle = texture_loadv(renderer.handle, static_cast<texture_type_t>(type), filePaths);
	}

	SHOWCASE_API Texture::Texture(Renderer& renderer, TextureType type, ...)
	{
		va_list args;
		va_start(args, type);
		handle = texture_loadv(renderer.handle, static_cast<texture_type_t>(type), args);
		va_end(args);
	}

	SHOWCASE_API void Texture::destroy() const
	{
		texture_destroy(handle);
	}

	SHOWCASE_API void Texture::releaseResources() const
	{
		texture_release_resources(handle);
	}

	SHOWCASE_API void Texture::drop() const
	{
		texture_destroy(handle);
		texture_release_resources(handle);
	}
}