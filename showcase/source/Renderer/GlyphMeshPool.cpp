
#include <Renderer/GlyphMeshPool.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Font.hpp>
#include <renderer/glyph_mesh_pool.h> 

namespace V3D
{
	SHOWCASE_API GlyphMeshPool::GlyphMeshPool(Renderer* renderer, Font* font)
	{
		handle = glyph_mesh_pool_create(renderer->handle, font->handle);
	}

	SHOWCASE_API void GlyphMeshPool::destroy()
	{
		glyph_mesh_pool_destroy(handle);
	}

	SHOWCASE_API void GlyphMeshPool::releaseResources()
	{
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API void GlyphMeshPool::drop()
	{
		glyph_mesh_pool_destroy(handle);
		glyph_mesh_pool_release_resources(handle);
	}

	SHOWCASE_API glyph_mesh_pool_t* GlyphMeshPool::getHandle() const
	{
		return handle;
	}
}
