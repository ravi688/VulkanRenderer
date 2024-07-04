#pragma once

#include <sge/render_queue.h> /* for render_queue_t* */

namespace SGE
{
	class RenderQueue
	{
	private:
		render_queue_t* m_handle;
		RenderQueue(render_queue_t* handle) noexcept : m_handle(handle) { }
	public:
		enum class Type : u8
		{
			Undefined,
			Background,
			Geometry,
			AlphaTested,
			GeometryLast,
			Transparent,
			Overlay,
			Queue0,
			Queue1,
			Queue2,
			Queue3,
			Queue4,
			Queue5,
			Queue6,
			Queue7
		};
		static render_queue_type_t to_render_queue_type(Type type);
		render_queue_t* getHandle() const noexcept { return m_handle; }
	};
}