
#include <Renderer/TextMesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/GlyphMeshPool.hpp>

#include <iostream>

namespace V3D
{
	SHOWCASE_API TextMesh::TextMesh(Renderer* renderer, GlyphMeshPool* pool)
	{
		handle = text_mesh_create(renderer->handle, pool->handle);
	}

	SHOWCASE_API void TextMesh::destroy()
	{
		text_mesh_destroy(handle);
	}

	SHOWCASE_API void TextMesh::releaseResources()
	{
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API void TextMesh::drop()
	{
		text_mesh_destroy(handle);
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API text_mesh_t* TextMesh::getHandle() const
	{
		return handle;
	}
}