#include <sge-cpp/RenderScene.hpp>
#include <sge-cpp/sge.hpp> /* for SGE::Driver::getHandle() */

namespace SGE
{
	RenderScene::RenderScene(Driver& driver, u64 queueMask) noexcept
	{
		m_handle = render_scene_create_from_mask(driver.getHandle(), queueMask);
	}
}
