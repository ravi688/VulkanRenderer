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

		friend class Renderer;
		friend class GlyphMeshPool;


	public:

		Font(const std::string& filePath);

		void unload() override;
		void drop() override;

		font_t* getHandle() const;
	};
}
