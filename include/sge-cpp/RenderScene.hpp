#pragma once

#include <sge/render_scene.h> /* for render_scene_t* */
#include <sge-cpp/RenderObject.hpp> /* for SGE::RenderObject */
#include <sge-cpp/RenderQueue.hpp> /* for SGE::RenderQueue */
#include <sge-cpp/Camera.hpp> /* for SGE::Camera */

namespace SGE
{
	/* forward declaration of SGE::Driver */
	class Driver;

	class SGE_API RenderScene
	{
	private:
		render_scene_t* m_handle;
		RenderScene(render_scene_t* handle) noexcept : m_handle(handle) { }
	public:
		RenderScene() : m_handle(NULL) { }
		RenderScene(Driver& driver, u64 queueMask) noexcept;

		void destroy() noexcept
		{
			render_scene_destroy(m_handle);
			render_scene_release_resources(m_handle);
			m_handle = NULL;
		}

		RenderObject createObject(RenderObject::Type objectType, RenderQueue::Type queueType) const noexcept
		{
			auto handle = render_scene_create_object(m_handle, RenderObject::to_render_object_type(objectType), RenderQueue::to_render_queue_type(queueType));
			return render_scene_getH(m_handle, handle);
		}

		void destroyObject(RenderObject& obj) const noexcept
		{
			render_scene_destroy_object(m_handle, obj.getHandle());
		}

		void buildQueues() const noexcept
		{
			render_scene_build_queues(m_handle);
		}

		void addCamera(Camera camera) const noexcept
		{
			render_scene_add_camera(m_handle, camera.getHandle());
		}

		void removeCamera(Camera camera) const noexcept
		{
			render_scene_remove_camera(m_handle, camera.getHandle());
		}

		void render(u64 queueMask, u32 flags) const noexcept
		{
			render_scene_render(m_handle, queueMask, flags);
		}
	};
}