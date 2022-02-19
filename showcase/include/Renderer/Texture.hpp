
#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>
#include <renderer/texture.h>

#include <stdarg.h>		// va_list

namespace V3D
{
	enum class TextureType
	{
		NORMAL = TEXTURE_TYPE_NORMAL,
		ALBEDO = TEXTURE_TYPE_ALBEDO,
		CUBE_COMBINED = TEXTURE_TYPE_CUBE_COMBINED,
		CUBE_SEPARATED = TEXTURE_TYPE_CUBE_SEPARATED,
		CUBE = TEXTURE_TYPE_CUBE
	};

	class Renderer;

	class SHOWCASE_API Texture : IRenderObject
	{
	private:
		texture_t* handle;

		friend Renderer;
		friend class Material;

		// variable number of file paths (const char*)
		Texture(Renderer& renderer, TextureType type, va_list filePaths);
		Texture(Renderer& renderer, TextureType type, ...);
		//Texture(Renderer* renderer, /*Create info*/)

	public:

		inline Texture(texture_t* texture) : handle(texture) { }

		// Implementation of IRenderObject
		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;
	};
}