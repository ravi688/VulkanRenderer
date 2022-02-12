
#include <Renderer/TextMesh.hpp>
#include <Renderer/Renderer.hpp>
#include <Renderer/GlyphMeshPool.hpp>

#include <iostream>

namespace V3D
{
	SHOWCASE_API TextMesh::TextMesh(Renderer& renderer, GlyphMeshPool& pool)
	{
		handle = text_mesh_create(renderer.handle, pool.handle);
	}

	SHOWCASE_API void TextMesh::destroy() const
	{
		text_mesh_destroy(handle);
	}

	SHOWCASE_API void TextMesh::releaseResources() const
	{
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API void TextMesh::drop() const
	{
		text_mesh_destroy(handle);
		text_mesh_release_resources(handle);
	}

	SHOWCASE_API TextMesh::StringHandle TextMesh::createString() const
	{
		return text_mesh_string_create(handle);
	}

	SHOWCASE_API void TextMesh::destroyString(StringHandle shandle) const
	{
		text_mesh_string_destroyH(handle, shandle);
	}

	SHOWCASE_API void TextMesh::draw() const
	{
		text_mesh_draw(handle);
	}

	SHOWCASE_API void TextMesh::setString(StringHandle shandle, const char* ptr) const
	{
		text_mesh_string_setH(handle, shandle, ptr);
	}

	SHOWCASE_API void TextMesh::setStringPosition(StringHandle shandle, const Math::Vec3& position) const
	{
		text_mesh_string_set_positionH(handle, shandle, const_cast<Math::Vec3&>(position));
	}

	SHOWCASE_API void TextMesh::setStringRotation(StringHandle shandle, const Math::Vec3& euler) const
	{
		text_mesh_string_set_rotationH(handle, shandle, const_cast<Math::Vec3&>(euler));
	}

	SHOWCASE_API void TextMesh::setStringScale(StringHandle shandle, const Math::Vec3& scaleVector) const
	{
		text_mesh_string_set_scaleH(handle, shandle, const_cast<Math::Vec3&>(scaleVector));
	}
}