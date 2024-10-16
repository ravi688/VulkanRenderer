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
		m_debugRootContainer = createContainer<FullWindowContainer>(NULL);
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
				renderable->updateNormalizedDrawOrder((maxDrawOrder == minDrawOrder) ? 0.0f : ((1.0f - static_cast<f32>(renderable->getDrawOrder() - minDrawOrder) * normalizeFactor) * 40.0f));
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
			m_globalTextGroup = getGfxDriver().createTextGroup();
			_com_assert(m_globalTextGroup != GFX_DRIVER_OBJECT_NULL_HANDLE);
		}
		return m_globalTextGroup;
	}

	// Containers

	template<>
	Container* UIDriver::createContainer<Container>(Container* parent)
	{
		Container* cntr = new Container(*this, parent);
		return cntr;
	}

	template<>
	FullWindowContainer* UIDriver::createContainer<FullWindowContainer>(Container* parent)
	{
		FullWindowContainer* cntr = new FullWindowContainer(*this);
		return cntr;
	}

	template<>
	RenderableContainer* UIDriver::createContainer<RenderableContainer>(Container* parent)
	{
		RenderableContainer* rectCntr = new RenderableContainer(*this, parent);
		return rectCntr;
	}


	// Renderables 

	Text* UIDriver::createText(RenderableContainer* container)
	{
		_assert(container != NULL);
		Text* text = new Text(*this, container);
		return text;
	}

	template<typename RenderRectType>
	RenderRectType* UIDriver::createRenderRect(RenderableContainer* container)
	{
		_assert(container != NULL);
		RenderRectType* rect = new RenderRectType(*this, container);
		return rect;
	}

	template RenderRectOutline* UIDriver::createRenderRect<RenderRectOutline>(RenderableContainer* container);
	template RenderRectFill* UIDriver::createRenderRect<RenderRectFill>(RenderableContainer* container);
	template RenderRectFillRound* UIDriver::createRenderRect<RenderRectFillRound>(RenderableContainer* container);
	template RenderImage* UIDriver::createRenderable<RenderImage>(RenderableContainer* parent);

	template<>
	Text* UIDriver::createRenderable<Text>(RenderableContainer* parent) { return createText(parent); }

	template<>
	RenderRectOutline* UIDriver::createRenderable<RenderRectOutline>(RenderableContainer* parent) { return createRenderRect<RenderRectOutline>(parent); }

	template<>
	RenderRectFill* UIDriver::createRenderable<RenderRectFill>(RenderableContainer* parent) { return createRenderRect<RenderRectFill>(parent); }

	template<>
	RenderRectFillRound* UIDriver::createRenderable<RenderRectFillRound>(RenderableContainer* parent) { return createRenderRect<RenderRectFillRound>(parent); }

	template<>
	RenderImage* UIDriver::createRenderable<RenderImage>(RenderableContainer* parent) { return createRenderRect<RenderImage>(parent); }
}