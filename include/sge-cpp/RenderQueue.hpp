#pragma once

#include <sge/render_queue.h> /* for render_queue_t* */
#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference

namespace SGE
{
	class RenderQueue : public PtrReference<render_queue_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(RenderQueue);
		
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
	};
}