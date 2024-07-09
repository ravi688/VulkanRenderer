#include <sge/render_window.h>

#include <utility> /* for std::pair */

namespace SGE
{
	class SGE_API RenderWindow
	{
	private:
		render_window_t* m_handle;
		RenderWindow(render_window_t* window) : m_handle(window) { }
		friend class Driver;
	public:

		std::pair<u32, u32> getSize() const noexcept
		{
			u32 width, height;
			render_window_get_framebuffer_extent(m_handle, &width, &height);
			return { width, height };
		}
	};
}