
#include <Renderer/Mesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/Mesh3D.hpp>

namespace V3D
{
	SHOWCASE_API Mesh::Mesh(Renderer& renderer, Mesh3D& mesh_data)
	{
		handle = mesh_create(renderer.handle, mesh_data.handle);
	}

	SHOWCASE_API void Mesh::destroy() const
	{
		mesh_destroy(handle);
	}

	SHOWCASE_API void Mesh::releaseResources() const
	{
		mesh_release_resources(handle);
	}

	SHOWCASE_API void Mesh::drop() const
	{
		mesh_destroy(handle);
		mesh_release_resources(handle);
	}

	SHOWCASE_API void Mesh::drawIndexed() const
	{
		mesh_draw_indexed(handle);
	}

	SHOWCASE_API void Mesh::drawInstanced(u32 instanceCount) const
	{
		mesh_draw_instanced(handle, instanceCount);
	}

	SHOWCASE_API void Mesh::drawNonIndexed() const
	{
		mesh_draw(handle);
	}

	SHOWCASE_API void Mesh::drawIndexedInstanced(u32 instanceCount) const
	{
		mesh_draw_indexed_instanced(handle, instanceCount);
	}
}