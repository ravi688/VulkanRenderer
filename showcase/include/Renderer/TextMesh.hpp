
#pragma once

#include <Renderer/IRenderObject.hpp>

#include <renderer/text_mesh.h> 			// C header, text_mesh_t

namespace V3D
{
	class TextMesh : V3D::IRenderObject
	{
	private:
		text_mesh_t* handle;

		friend class Renderer;

		TextMesh();

	public:
		
	};
}
