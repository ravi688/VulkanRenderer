
#include <Renderer/GlyphMeshPool.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Font.hpp>
#include <renderer/glyph_mesh_pool.h> 

namespace V3D
{
	SHOWCASE_API GlyphMeshPool::GlyphMeshPool(Renderer& renderer, Font& font)
	{
		handle = glyph_mesh_pool_create(renderer.handle, font.handle);
	}

	SHOWCASE_API void GlyphMeshPool::destroy() const
	{
		glyph_mesh_pool_destroy(handle);
	}

	SHOWCASE_API void GlyphMeshPool::releaseResources() const
	{
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API void GlyphMeshPool::drop() const
	{
		glyph_mesh_pool_destroy(handle);
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API Mesh GlyphMeshPool::getMesh(u16 glyph) const
	{
		return glyph_mesh_pool_get_mesh(handle, glyph);
	}
}
