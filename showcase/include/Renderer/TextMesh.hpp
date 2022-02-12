
#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>
#include <Math/Vec3.hpp>

#include <renderer/text_mesh.h> 			// C header, text_mesh_t

namespace V3D
{
	class Renderer;
	class GlyphMeshPool;

	class SHOWCASE_API TextMesh : IRenderObject
	{
	private:
		text_mesh_t* handle;

		friend Renderer;

		TextMesh(Renderer& renderer, GlyphMeshPool& pool);

	public:
		typedef text_mesh_string_handle_t StringHandle;

		inline TextMesh(text_mesh_t* textMesh) : handle(textMesh) { }

		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;
		void draw() const;

		StringHandle createString() const;
		void destroyString(StringHandle handle) const;

		void setString(StringHandle handle, const char* ptr) const;
		void setStringPosition(StringHandle handle, const Math::Vec3& position) const;
		void setStringRotation(StringHandle handle, const Math::Vec3& euler) const;
		void setStringScale(StringHandle handle, const Math::Vec3& scaleVector) const;
	};
}
