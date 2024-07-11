#include <sutk/defines.hpp>

namespace SUTK
{
	/* This interface should be implemented by any class which models a visual element, i.e. which can be seen on the display */
	class IRenderable
	{
	public:
		/* returns true, if GPU side data is out of sync with respect to the CPU side data, otherwise false */
		virtual bool isDirty() = 0;
		/* updates (copies CPU side data to) GPU side data, and it may also create or recreate exisiting GPU Driver objects */
		virtual void update() = 0;
	};
}