
#pragma once

namespace V3D
{
	class IResourceObject
	{
	public:
		virtual void unload() = 0; 		// unloads the resource from memory
		virtual void drop() = 0;		// cleans extra heap allocated objects if any
	};

}