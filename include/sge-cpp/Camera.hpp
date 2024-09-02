#pragma once

#include <sge/camera.h>
#include <sge-cpp/Texture.hpp>
#include <common/PtrReference.hpp> // for com::PtrReference

namespace SGE
{
	class SGE_API Camera : public com::PtrReference<camera_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Camera);

	public:
		enum class ProjectionType : u8
		{
			Undefined,
			Orthographic,
			Perspective,
			StereoGraphic
		};

		enum class RenderTargetType : u8
		{
			Undefined,
			Color,
			Depth
		};

		enum class RenderTargetBindingType : u8
		{
			Shared,
			Exclusive
		};

		friend class CameraSystem;
	public:

		static camera_render_target_binding_type_t to_camera_render_target_binding_type(RenderTargetBindingType bindingType);
		static camera_render_target_type_t to_camera_render_target_type(RenderTargetType targetType);
		static camera_projection_type_t to_camera_projection_type(ProjectionType projectionType);

		void setClear(color_t color, float depth) const noexcept { camera_set_clear(m_handle, color, depth); }
		void setActive(bool isActive) const noexcept { camera_set_active(m_handle, isActive); }
		void setRenderTarget(RenderTargetType targetType, RenderTargetBindingType bindingType, Texture texture) const noexcept
		{
			camera_set_render_target(m_handle, to_camera_render_target_type(targetType), to_camera_render_target_binding_type(bindingType), texture.getHandle());
		}
		bool isActive() const noexcept { return camera_is_active(m_handle); }

		/* getters */
		mat4_t getView() const noexcept { return camera_get_view(m_handle); }
		mat4_t getTransform() const noexcept { return camera_get_transform(m_handle); }
		mat4_t getProjection() const noexcept { return camera_get_projection(m_handle); }
		vec3_t getPosition() const noexcept { return camera_get_position(m_handle); }
		vec3_t getRotation() const noexcept { return camera_get_rotation(m_handle); }
		vec2_t getClip_planes() const noexcept { return camera_get_clip_planes(m_handle); }
		float getNear_clip_plane() const noexcept { return camera_get_near_clip_plane(m_handle); }
		float getFar_clip_plane() const noexcept { return camera_get_far_clip_plane(m_handle); }
		float getField_of_view() const noexcept { return camera_get_field_of_view(m_handle); }
		float getHeight() const noexcept { return camera_get_height(m_handle); }

		/* setters */
		void setTransform(mat4_t transform) const noexcept { camera_set_transform(m_handle, transform); }
		void setPosition(vec3_t position) const noexcept { camera_set_position(m_handle, position); }
		void setRotation(vec3_t rotation) const noexcept { camera_set_rotation(m_handle, rotation); }
		void setClipPlanes(float nearClipPlane, float farClipPlane) const noexcept { camera_set_clip_planes(m_handle, nearClipPlane, farClipPlane); }
		void setFieldOfView(float fov) const noexcept { camera_set_field_of_view(m_handle, fov); }
		void setHeight(float height) const noexcept { camera_set_height(m_handle, height); }
		void setRender_area(u32 offsetX, u32 offsetY, u32 width, u32 height) const noexcept { camera_set_render_area(m_handle, offsetX, offsetY, width, height); }
		void setRender_area_relative(u32 offsetX, u32 offsetY, u32 width, u32 height) const noexcept { camera_set_render_area_relative(m_handle, offsetX, offsetY, width, height); }
		void setRenderAreaDefault() const noexcept { camera_set_render_area_default(m_handle); }
	};
}
