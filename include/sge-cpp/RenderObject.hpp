#pragma once

#include <sge/render_object.h> /* for render_object_t* */
#include <sge-cpp/Material.hpp> /* for SGE::Material */
#include <sge-cpp/BitmapText.hpp> /* for SGE::BitmapText */
#include <sge-cpp/Mesh.hpp> // for SGE::Mesh
#include <common/PtrReference.hpp> // for com::PtrReference

namespace SGE
{
	class SGE_API RenderObject : public com::PtrReference<render_object_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(RenderObject);

	private:
		friend class RenderScene;
	public:

		enum class Type : u8
		{
			Mesh,
			TextMesh,
			Text
		};

		static render_object_type_t to_render_object_type(Type type);

		void setActive(bool isActive) noexcept
		{
			render_object_set_active(m_handle, isActive);
		}
		bool isActive() const noexcept
		{
			return render_object_is_active(m_handle);
		}

		void attach(BitmapText text) const noexcept
		{
			render_object_attach(m_handle, reinterpret_cast<void*>(text.getHandle()));
		}
		void attach(Mesh mesh) const noexcept
		{
			render_object_attach(m_handle, reinterpret_cast<void*>(mesh.getHandle()));
		}
		void setMaterial(Material material) const noexcept
		{
			render_object_set_material(m_handle, material.getHandle());
		}
		Material getMaterial() const noexcept
		{
			return Material(render_object_get_material(m_handle));
		}
		void setTransform(mat4_t transform) const noexcept
		{
			render_object_set_transform(m_handle, transform);
		}
		mat4_t getTransform() const noexcept
		{
			return render_object_get_transform(m_handle);
		}
		void setPosition(vec3_t position) const noexcept
		{
			render_object_set_position(m_handle, position);
		}
		vec3_t getPosition() const noexcept
		{
			mat4_t m = getTransform();
			return vec3(m.m03, m.m13, m.m23);
		}
		void setScissor(irect2d_t rect) const noexcept
		{
			render_object_set_scissor(m_handle, rect);
		}
	};
}