#pragma once

#include <sge/camera_system.h>
#include <sge-cpp/Camera.hpp>

namespace SGE
{
	class SGE_API CameraSystem
	{
	private:
		camera_system_t* m_handle;
		CameraSystem(camera_system_t* handle) noexcept : m_handle(handle) { }

		friend class Driver;

	public:
		camera_system_t* getHandle() { return m_handle; }

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
