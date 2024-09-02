#pragma once

#include <sge/camera_system.h>
#include <sge-cpp/Camera.hpp>
#include <common/PtrReference.hpp> // for com::PtrReference

namespace SGE
{
	class SGE_API CameraSystem : public com::PtrReference<camera_system_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(CameraSystem);

		friend class Driver;

	public:
		u32 getCount() const noexcept { return camera_system_get_count(m_handle); }
		Camera getAt(u32 index) const noexcept { return Camera(camera_system_get_at(m_handle, index)); }

		Camera createCamera(Camera::ProjectionType projectionType)
		{ 
			auto handle = camera_system_create_camera(m_handle, Camera::to_camera_projection_type(projectionType));
			auto camera = camera_system_getH(m_handle, handle);
			return Camera(camera); 
		}
	};
}
