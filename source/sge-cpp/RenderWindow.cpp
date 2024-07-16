#include <sge-cpp/RenderWindow.hpp>

namespace SGE
{
	template<> RenderWindow Event::ReinterpretPublisher<RenderWindow>(void* publisher) { return RenderWindow(RENDER_WINDOW(publisher)); }
}
