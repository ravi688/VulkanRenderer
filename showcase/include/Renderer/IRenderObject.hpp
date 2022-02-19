
#pragma once

namespace V3D
{
	class IRenderObject
	{
	public:
		virtual void destroy() const = 0;
		virtual void releaseResources() const = 0;

		// destroy followed by release resources
		virtual void drop() const = 0;
	};
}
