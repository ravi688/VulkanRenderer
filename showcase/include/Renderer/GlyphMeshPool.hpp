#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>
#include <Renderer/Mesh.hpp> 				// Mesh
#include <renderer/glyph_mesh_pool.h> 		// C header, glyph_mesh_pool_t


namespace V3D
{
	class Renderer;
	class Font;

	class SHOWCASE_API GlyphMeshPool : IRenderObject
	{
	private:
		glyph_mesh_pool_t* handle;

		friend Renderer;
		friend class TextMesh;

		GlyphMeshPool(Renderer& renderer, Font& font);
	
	public:

		inline GlyphMeshPool(glyph_mesh_pool_t* pool) : handle(pool) { }

		// Implementation of IRenderObject interface
		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;


		Mesh getMesh(u16 glyph) const;
	};
}
