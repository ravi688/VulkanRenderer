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

#include <common/assert.h>

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

		_com_assert(minDrawOrder <= maxDrawOrder);

		if(isRecalculateDrawOrder)
		{
			for(auto it = m_renderables.begin(); it != m_renderables.end(); it++)
			{
				f32 normalizeFactor = 1.0f / static_cast<f32>(maxDrawOrder - minDrawOrder);
				Renderable* renderable = (*it);
				renderable->updateNormalizedDrawOrder((maxDrawOrder == minDrawOrder) ? 0.0f : ((1.0f - static_cast<f32>(renderable->getDrawOrder() - minDrawOrder) * normalizeFactor) * 40.0f));
			}
		}

		// now record and dispatch rendering commands (delegated that to rendering backend)
		m_gfxDriver.render(*this);
	}

	UIDriver::ImageData::IDType UIDriver::lazyImageLoad(std::string_view str) noexcept
	{
		return m_imageData.add(std::make_pair(std::string { str }, GFX_DRIVER_OBJECT_NULL_HANDLE));
	}

	void UIDriver::lazyImageUnload(ImageData::IDType id) noexcept
	{
		m_imageData.remove(id);
	}

	std::string_view UIDriver::getImagePath(ImageData::IDType id) noexcept
	{
		return { m_imageData.get(id).first };
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