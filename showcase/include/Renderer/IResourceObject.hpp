
#pragma once

#include <Showcase/Defines.hpp>

namespace V3D
{
	class SHOWCASE_API IResourceObject
	{
	public:
		virtual void unload() const = 0; 		// unloads the resource from memory
		virtual void drop() const = 0;		// cleans extra heap allocated objects if any
	};

}