#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IResourceObject.hpp>
#include <renderer/font.h>			// C header, font_t
#include <string>

namespace V3D
{
	class SHOWCASE_API Font : IResourceObject
	{
	private:
		font_t* handle;

		friend class GlyphMeshPool;


	public:

		Font(const std::string& filePath);
		inline Font(font_t* font) : handle(font) { }

		// Implementation of IResourceObject interface
		void unload() const override;
		void drop() const override { unload(); }
	};
}
