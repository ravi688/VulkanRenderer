#include <sutk/UIDriver.hpp>

#include <sutk/FullWindowContainer.hpp>
#include <sutk/Text.hpp>
#include <sutk/IGfxDriver.hpp>
#include <sutk/RenderRectOutline.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/RenderRectFillRound.hpp>
#include <sutk/RenderImage.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/DummyInputDriver.hpp> // for STUK::DummyInputDriver
#include <sutk/IRunnable.hpp> // for SUTK::IRunnable::Update

#include <common/assert.h>
#include <common/debug.h> // for debug_log_error

#include <algorithm> // for std::find

namespace SUTK
{
	UIDriver::UIDriver(IGfxDriver& gfxDriver, IInputDriver& inputDriver) noexcept: m_gfxDriver(gfxDriver), m_inputDriver(&inputDriver), m_isDummyInputDriver(false), m_globalTextGroup(GFX_DRIVER_OBJECT_NULL_HANDLE)
	{
		m_debugRootContainer = createContainer<FullWindowContainer>(com::null_pointer<Container>());
	}

	UIDriver::UIDriver(IGfxDriver& gfxDriver) noexcept : m_gfxDriver(gfxDriver), m_inputDriver(new DummyInputDriver{ }), m_isDummyInputDriver(true), m_globalTextGroup(GFX_DRIVER_OBJECT_NULL_HANDLE)
	{
		
	}

	UIDriver::~UIDriver()
	{
		if(m_isDummyInputDriver)
			delete m_inputDriver;
	}

	void UIDriver::render()
	{
		// Update runnables
		if(m_runnables.size() != 0)
			for(IRunnable* &runnable : m_runnables)
				IRunnable::Update(runnable);

		bool isRecalculateDrawOrder = false;
		u32 minDrawOrder = std::numeric_limits<u32>::max();
		u32 maxDrawOrder = 0;
		// update GPU side data
		for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
		{
			Renderable* renderable = *it;
			if(renderable->isDirty())
				renderable->update();
			if(renderable->isDrawOrderDirty())
				isRecalculateDrawOrder = true;
			u32 drawOrder = renderable->getDrawOrder();
			if(drawOrder < minDrawOrder)
				minDrawOrder = drawOrder;
			if(drawOrder > maxDrawOrder)
				maxDrawOrder = drawOrder;
		}

		if(m_renderables.size() > 0)
			_com_assert(minDrawOrder <= maxDrawOrder);

		if(isRecalculateDrawOrder)
		{
			f32 normalizeFactor = 1.0f / static_cast<f32>(maxDrawOrder - minDrawOrder);
			for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
			{
				Renderable* renderable = (*it);
				renderable->updateNormalizedDrawOrder((maxDrawOrder == minDrawOrder) ? 0.0f : ((1.0f - static_cast<f32>(renderable->getDrawOrder() - minDrawOrder) * normalizeFactor) * 99.0f));
			}
		}

		// now record and dispatch rendering commands (delegated that to rendering backend)
		m_gfxDriver.render(*this);
	}


	void UIDriver::addRunnable(IRunnable* runnable) noexcept
	{
		m_runnables.push_back(runnable);
	}

	void UIDriver::removeRunnable(IRunnable* runnable) noexcept
	{
		auto it = std::find(m_runnables.begin(), m_runnables.end(), runnable);
		if(it != m_runnables.end())
		{
			debug_log_error("Failed to remove SUTK::IRunnable: %p, as it doesn't exist in the Registry", runnable);
			return;
		}
		m_runnables.erase(it);
	}


	UIDriver::ImageReference UIDriver::loadImage(std::string_view str) noexcept
	{
		return getGfxDriver().loadTexture(str);
	}

	void UIDriver::unloadImage(ImageReference id) noexcept
	{
		getGfxDriver().unloadTexture(id);
	}

	UIDriver::FontReference UIDriver::loadFont(std::string_view path) noexcept
	{
		return getGfxDriver().loadFont(path);
	}

	void UIDriver::unloadFont(FontReference id) noexcept
	{
		getGfxDriver().unloadFont(id);
	}

	UIDriver::ImageAttributes UIDriver::getImageAttributes(ImageReference id) noexcept
	{
		TextureAttributes attr;
		getGfxDriver().getTextureAttributes(id, attr);
		return attr;
	}

	GfxDriverObjectHandleType UIDriver::getGlobalTextGroup() noexcept
	{
		if(m_globalTextGroup == GFX_DRIVER_OBJECT_NULL_HANDLE)
		{
			m_globalTextGroup = getGfxDriver().createTextGroup(RenderMode::Transparent);
			getGfxDriver().setTextGroupDepth(m_globalTextGroup, 0.0f);
			_com_assert(m_globalTextGroup != GFX_DRIVER_OBJECT_NULL_HANDLE);
		}
		return m_globalTextGroup;
	}
}