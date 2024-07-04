#include <sge-cpp/RenderObject.hpp>

#include <sge/debug.h>

namespace SGE
{
	render_object_type_t RenderObject::to_render_object_type(Type type)
	{
		switch(type)
		{
			case Type::Mesh: return RENDER_OBJECT_TYPE_MESH;
			case Type::TextMesh: return RENDER_OBJECT_TYPE_TEXT_MESH;
			case Type::Text: return RENDER_OBJECT_TYPE_TEXT;
			default: debug_log_error("Unrecognized RenderObject::Type");
		}
		return RENDER_OBJECT_TYPE_MESH;
	}
}
