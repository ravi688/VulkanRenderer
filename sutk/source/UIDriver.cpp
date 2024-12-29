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
#include <sutk/OrderedInputEventsDispatcher.hpp> // for SUTK::OrderedInputEventsDispatcher
#include <sutk/AnimationEngine.hpp> // for SUTK::AnimationEngine
#include <sutk/BuiltinThemeManager.hpp> // for SUTK::BuiltinThemeManager

#include <common/assert.h>
#include <common/debug.h> // for debug_log_error

#include <algorithm> // for std::find

namespace SUTK
{
	UIDriver::UIDriver(IGfxDriver& gfxDriver, IInputDriver& inputDriver) noexcept: m_gfxDriver(gfxDriver), 
																					m_inputDriver(&inputDriver), 
																					m_isDummyInputDriver(false), 
																					m_globalTextGroup(GFX_DRIVER_OBJECT_NULL_HANDLE),
																					m_isCalledFirstTime(com::True),
																					m_prevTime(std::chrono::steady_clock::now()),
																					m_deltaTime(0),
																					// Render for the first time to let the human user see black screen or anything other color IGfxDriver implementation might want to display for the first time.
																					m_isRenderThisFrame(com::True)
	{
		m_orderedEventsDispatcher = new OrderedInputEventsDispatcher(*m_inputDriver);
		m_animationEngine = new AnimationEngine(*this);
		m_themeManager = new BuiltinThemeManager(*this);
		gfxDriver.addOnResizeHandler([this](Vec2Df) noexcept
		{
			this->m_isRenderThisFrame = com::True;
		});
	}

	UIDriver::UIDriver(IGfxDriver& gfxDriver) noexcept : UIDriver(gfxDriver, *new DummyInputDriver{ })
	{
		m_isDummyInputDriver = true;
	}

	UIDriver::~UIDriver()
	{
		delete m_orderedEventsDispatcher;
		delete m_animationEngine;
		delete m_themeManager;
		if(m_isDummyInputDriver)
			delete m_inputDriver;
	}

	void UIDriver::addRenderable(Renderable* renderable) noexcept
	{
		m_renderables.push_back(renderable);
	}

	void UIDriver::removeRenderable(Renderable* renderable) noexcept
	{
		auto it = std::find(m_renderables.begin(), m_renderables.end(), renderable);
		if(it != m_renderables.end())
			m_renderables.erase(it);
		else debug_log_error("Failed to remove SUTK::Renderable: %p, as it doesn't exist in the Registry", renderable);
	}

	com::Bool UIDriver::isDirty() noexcept
	{
		if(m_isCalledFirstTime)
		{
			m_prevTime = std::chrono::steady_clock::now();
			m_isCalledFirstTime = com::False;
		}
		std::chrono::time_point<std::chrono::steady_clock> nowTime = std::chrono::steady_clock::now();
		m_deltaTime = std::chrono::duration_cast<std::chrono::duration<f32, std::ratio<1, 1>>>(nowTime - m_prevTime).count();
		m_prevTime = nowTime;
		// For now return true to render at every single iteration even though UI might not be dirty
		return com::True;
	}

	void UIDriver::render()
	{
		// Update runnables
		if(m_runnables.size() != 0)
			m_runnables.traverse(IRunnable::Update);

		com::Bool isRerender = m_isRenderThisFrame;
		com::Bool isRecalculateDrawOrder = com::False;
		u32 minDrawOrder = std::numeric_limits<u32>::max();
		u32 maxDrawOrder = 0;
		// update GPU side data
		for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
		{
			Renderable* renderable = *it;
			if(!isRerender && renderable->isActiveForThisFrame() && renderable->isRedraw())
			{
				isRerender = com::True;
				renderable->setRedraw(com::False);
			}
			// Only update Renderables which are active and dirty
			if(renderable->isActiveForThisFrame() && renderable->isDirty())
				renderable->update();
			// If at least Renderable's draw order is dirty then normalized draw orders need to be re-calculated
			// and passed to the GPU driver
			if(!isRecalculateDrawOrder && renderable->isActiveForThisFrame() && renderable->isDrawOrderDirty())
				isRecalculateDrawOrder = com::True;
			// Keep track of minimum and maximum draw order values
			u32 drawOrder = renderable->getDrawOrder();
			if(drawOrder < minDrawOrder)
				minDrawOrder = drawOrder;
			if(drawOrder > maxDrawOrder)
				maxDrawOrder = drawOrder;
		}

		if(isRecalculateDrawOrder)
		{
			_com_assert(minDrawOrder <= maxDrawOrder);
			// Proof:
			//
			// let x1 = minDrawOrder
			// let x2 = maxDrawOrder
			//
			// k - (x1 + lambda1)  				   k - x1
			// ----------------------------  !=  -----------
			// x2 - x2 + (lambda2- lambda1) 	   x2 - x1
			//
			// Both the sides will be equal only when lambda1 and lambda2 are both zero
			com::Bool isDrawOrderRangeChanged { (m_maxDrawOrder != maxDrawOrder) || (m_minDrawOrder != minDrawOrder) };
			u32 drawOrderRange = maxDrawOrder - minDrawOrder;
			f32 normalizeFactor = 1.0f / static_cast<f32>(drawOrderRange);
			for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
			{
				Renderable* renderable = (*it);
				if(isDrawOrderRangeChanged || (renderable->isActiveForThisFrame() && renderable->isDrawOrderDirty()))
				{
					auto normDrawOrder = drawOrderRange ? ((1.0f - static_cast<f32>(renderable->getDrawOrder() - minDrawOrder) * normalizeFactor) * 99.0f) : 0;
					renderable->updateNormalizedDrawOrder(normDrawOrder);
				}
			}
			m_maxDrawOrder = maxDrawOrder;
			m_minDrawOrder = minDrawOrder;
		}

		if(isRerender)
		{
			// now record and dispatch rendering commands (delegated that to rendering backend)
			m_gfxDriver.render(*this);
			m_isRenderThisFrame = com::False;
		}
	}


	void UIDriver::addRunnable(IRunnable* runnable) noexcept
	{
		m_runnables.push_back(runnable);
	}

	void UIDriver::removeRunnable(IRunnable* runnable) noexcept
	{
		auto it = m_runnables.find(runnable);
		if(m_runnables.isValidIterator(it))
			m_runnables.erase(it);
		else debug_log_error("Failed to remove SUTK::IRunnable: %p, as it doesn't exist in the Registry", runnable);
	}


	UIDriver::ImageReference UIDriver::loadImage(std::string_view str) noexcept
	{
		return getGfxDriver().loadTexture(str);
	}

	UIDriver::ImageReference UIDriver::loadImage(const std::span<const u8> span) noexcept
	{
		return getGfxDriver().loadTexture(span);
	}

	UIDriver::ImageReference UIDriver::loadImage(const u8* pixelData, u32 width, u32 height, u32 numChannels) noexcept
	{
		return getGfxDriver().loadTexture(pixelData, width, height, numChannels);
	}

	UIDriver::ImageReference UIDriver::loadImage(com::BinaryFileLoadResult result) noexcept
	{
		_com_assert(result);
		return loadImage(result.span());
	}

	void UIDriver::unloadImage(ImageReference id) noexcept
	{
		getGfxDriver().unloadTexture(id.getHandle());
	}

	UIDriver::FontReference UIDriver::loadFont(std::string_view path) noexcept
	{
		return getGfxDriver().loadFont(path);
	}

	UIDriver::FontReference UIDriver::loadFont(com::BinaryFileLoadResult result) noexcept
	{
		_com_assert(result);
		return getGfxDriver().loadFont(result.span());
	}

	void UIDriver::unloadFont(FontReference id) noexcept
	{
		getGfxDriver().unloadFont(id.getHandle());
	}

	UIDriver::ImageAttributes UIDriver::getImageAttributes(ImageReference id) noexcept
	{
		TextureAttributes attr;
		getGfxDriver().getTextureAttributes(id.getHandle(), attr);
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