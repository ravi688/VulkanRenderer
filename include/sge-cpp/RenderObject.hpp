#pragma once

#include <sge/render_object.h> /* for render_object_t* */
#include <sge-cpp/Material.hpp> /* for SGE::Material */
#include <sge-cpp/BitmapText.hpp> /* for SGE::BitmapText */

namespace SGE
{
	class SGE_API RenderObject
	{
	private:
		render_object_t* m_handle;
		RenderObject(render_object_t* handle) noexcept : m_handle(handle) { }
		friend class RenderScene;
	public:

		RenderObject() : m_handle(NULL) { }

		enum class Type : u8
		{
			Mesh,
			TextMesh,
			Text
		};

		static render_object_type_t to_render_object_type(Type type);

		render_object_t* getHandle() const noexcept { return m_handle; }

		void attach(BitmapText text) const noexcept
		{
			render_object_attach(m_handle, reinterpret_cast<void*>(text.getHandle()));
		}
		void setMaterial(Material material) const noexcept
		{
			render_object_set_material(m_handle, material.getHandle());
		}
		void setTransform(mat4_t transform) const noexcept
		{
			render_object_set_transform(m_handle, transform);
		}
		void setScissor(irect2d_t rect) const noexcept
		{
			render_object_set_scissor(m_handle, rect);
		}
	};
}