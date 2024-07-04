#include <sge-cpp/RenderQueue.hpp>

namespace SGE
{
	render_queue_type_t RenderQueue::to_render_queue_type(Type type)
	{
		switch(type)
		{
			case Type::Undefined : return RENDER_QUEUE_TYPE_UNDEFINED;
			case Type::Background : return RENDER_QUEUE_TYPE_BACKGROUND;
			case Type::Geometry : return RENDER_QUEUE_TYPE_GEOMETRY;
			case Type::AlphaTested : return RENDER_QUEUE_TYPE_ALPH_TESTED;
			case Type::GeometryLast : return RENDER_QUEUE_TYPE_GEOMETRY_LAST;
			case Type::Transparent : return RENDER_QUEUE_TYPE_TRANSPARENT;
			case Type::Overlay : return RENDER_QUEUE_TYPE_OVERLAY;
			case Type::Queue0 : return RENDER_QUEUE_TYPE_QUEUE0;
			case Type::Queue1 : return RENDER_QUEUE_TYPE_QUEUE1;
			case Type::Queue2 : return RENDER_QUEUE_TYPE_QUEUE2;
			case Type::Queue3 : return RENDER_QUEUE_TYPE_QUEUE3;
			case Type::Queue4 : return RENDER_QUEUE_TYPE_QUEUE4;
			case Type::Queue5 : return RENDER_QUEUE_TYPE_QUEUE5;
			case Type::Queue6 : return RENDER_QUEUE_TYPE_QUEUE6;
			case Type::Queue7 : return RENDER_QUEUE_TYPE_QUEUE7;
			default: debug_log_error("Unrecognized RenderQueue::Type");
		}

		return RENDER_QUEUE_TYPE_UNDEFINED;
	}
}