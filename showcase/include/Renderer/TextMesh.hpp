
#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IRenderObject.hpp>

#include <renderer/text_mesh.h> 			// C header, text_mesh_t

namespace V3D
{
	class Renderer;
	class GlyphMeshPool;

	class SHOWCASE_API TextMesh : IRenderObject
	{
	private:
		text_mesh_t* handle;

		friend class Renderer;

		TextMesh(Renderer* renderer, GlyphMeshPool* pool);

	public:

		void destroy() override;
		void releaseResources() override;
		void drop() override;

		text_mesh_t* getHandle() const;

		void draw();

		using string_handle_t = text_mesh_string_handle_t;

		string_handle_t createString();
		// void setStringPosition(string_handle_t handle, )
	};
}
