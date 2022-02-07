#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>
#include <renderer/glyph_mesh_pool.h> 		// C header, glyph_mesh_pool_t


namespace V3D
{
	class Renderer;
	class Font;

	class SHOWCASE_API GlyphMeshPool : IRenderObject
	{
	private:
		glyph_mesh_pool_t* handle;

		friend class Renderer;
		friend class TextMesh;

		GlyphMeshPool(Renderer* renderer, Font* font);
	
	public:

		void destroy() override;
		void releaseResources() override;
		void drop() override;

		glyph_mesh_pool_t* getHandle() const;
	};
}
