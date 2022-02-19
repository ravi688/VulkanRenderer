#pragma once

#include <Showcase/Defines.hpp>		// SHOWCASE_API
#include <Renderer/IRenderObject.hpp>
#include <renderer/mesh.h>			// C header

namespace V3D
{
	class Renderer;
	class Mesh3D;
	
	class SHOWCASE_API Mesh : IRenderObject
	{
	private:
		mesh_t* handle;

		friend Renderer;

		Mesh(Renderer& renderer, Mesh3D& mesh_data);
	
	public:
		inline Mesh(mesh_t* mesh) : handle(mesh) { }

		void destroy() const override;
		void releaseResources() const override;
		void drop() const override;
		
		inline void draw() const { drawIndexed(); }
		void drawIndexed() const;
		void drawIndexedInstanced(u32 instanceCount) const;
		void drawInstanced(u32 instanceCount) const;
		void drawNonIndexed() const;
	};
}
