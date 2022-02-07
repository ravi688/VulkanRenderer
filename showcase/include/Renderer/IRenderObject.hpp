
#pragma once

namespace V3D
{
	class IRenderObject
	{
	public:
		virtual void destroy() = 0;
		virtual void releaseResources() = 0;

		// destroy followed by release resources
		virtual void drop() = 0;
	};
}
