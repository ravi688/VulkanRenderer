#include <sge-cpp/Camera.hpp>
#include <common/defines.hpp>

namespace SGE
{
	camera_render_target_binding_type_t Camera::to_camera_render_target_binding_type(Camera::RenderTargetBindingType bindingType)
	{
		switch(bindingType)
		{
			case RenderTargetBindingType::Shared: return CAMERA_RENDER_TARGET_BINDING_TYPE_SHARED;
			case RenderTargetBindingType::Exclusive: return CAMERA_RENDER_TARGET_BINDING_TYPE_EXCLUSIVE;
			default: debug_log_error("Unrecognized Camera::RenderTargetBindingType");
		}
		return com::ForceIntToEnumClass<camera_render_target_binding_type_t>(0);
	}

	camera_render_target_type_t Camera::to_camera_render_target_type(Camera::RenderTargetType targetType)
	{
		switch(targetType)
		{
			case RenderTargetType::Undefined: return CAMERA_RENDER_TARGET_TYPE_UNDEFINED;
			case RenderTargetType::Color: return CAMERA_RENDER_TARGET_TYPE_COLOR;
			case RenderTargetType::Depth: return CAMERA_RENDER_TARGET_TYPE_DEPTH;
			default: debug_log_error("Unrecognized Camera::RenderTargetType");
		}
		return CAMERA_RENDER_TARGET_TYPE_UNDEFINED;
	}

	camera_projection_type_t Camera::to_camera_projection_type(Camera::ProjectionType projectionType)
	{
		switch(projectionType)
		{
			case ProjectionType::Undefined : return CAMERA_PROJECTION_TYPE_UNDEFINED;
			case ProjectionType::Orthographic : return CAMERA_PROJECTION_TYPE_ORTHOGRAPHIC;
			case ProjectionType::Perspective : return CAMERA_PROJECTION_TYPE_PERSPECTIVE;
			case ProjectionType::StereoGraphic : return CAMERA_PROJECTION_TYPE_STEROGRAPHIC;
			default: debug_log_error("Unrecognized Camera::ProjectionType");
		};
		return CAMERA_PROJECTION_TYPE_UNDEFINED;
	}
}
