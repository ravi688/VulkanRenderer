#pragma once

#include <sutk/defines.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::getSizeInCentimeters()

#include <vector> // for std::vector
#include <utility> // for std::forward

namespace SUTK
{
	// Forward declarations for Container classes
	class Container;
	class FullWindowContainer;
	class RenderableContainer;

	// Forward declarations for Renderable classes
	class Renderable;
	class Text;
	class RenderRectOutline;
	class RenderRectFill;
	class RenderRectFillRound;
	
	// Forward declarations for Interface classes
	class IGfxDriver;
	class IInputDriver;

	class UIDriver
	{
	private:
		IGfxDriver& m_gfxDriver;
		IInputDriver* m_inputDriver;
		bool m_isDummyInputDriver;
		std::vector<Renderable*> m_renderables;
		GfxDriverObjectHandleType m_globalTextGroup;

		friend class Renderable;
	public:
		UIDriver(IGfxDriver& gfxDriver, IInputDriver& inputDriver) noexcept;
		UIDriver(IGfxDriver& gfxDriver) noexcept;
		~UIDriver();

		void render();

		IGfxDriver& getGfxDriver() { return m_gfxDriver; }
		const IGfxDriver& getGfxDriver() const { return m_gfxDriver; }
		IInputDriver& getInputDriver() { return *m_inputDriver; }
		const IInputDriver& getInputDriver() const { return *m_inputDriver; }

		Vec2Df getWindowSize() noexcept { return getGfxDriver().getSizeInCentimeters(); }
		GfxDriverObjectHandleType getGlobalTextGroup() noexcept;

		template<typename ContainerType, typename... Args>
		ContainerType* createContainer(Container* parent, Args&&... args)
		{
			ContainerType* cntr = new ContainerType(*this, parent, std::forward<Args&&...>(args)...);
			return cntr;
		}
		template<typename RenderableType, typename... Args>
		RenderableType* createRenderable(RenderableContainer* parent, Args&&... args)
		{
			RenderableType* renderable = new RenderableType(*this, parent, std::forward<Args&&...>(args)...);
			m_renderables.push_back(renderable);
			return renderable;
		}
		template<typename RenderableType, typename... Args>
		std::pair<RenderableType*, RenderableContainer*> createRenderableWithContainer(Container* parent, Args&&... args)
		{
			RenderableContainer* container = createContainer<RenderableContainer>(parent);
			RenderableType* renderable = new RenderableType(*this, container, std::forward<Args&&...>(args)...);
			m_renderables.push_back(renderable);
			return { renderable, container };
		}
		Text* createText(RenderableContainer* parent);
		template<typename RenderRectType>
		RenderRectType* createRenderRect(RenderableContainer* container);
	};

	// declarations for the template specializations for Containers (non-renderable)
	template<>
	Container* UIDriver::createContainer<Container>(Container* parent);
	template<>
	FullWindowContainer* UIDriver::createContainer<FullWindowContainer>(Container* parent);
	template<>
	RenderableContainer* UIDriver::createContainer<RenderableContainer>(Container* parent);

	// declarations for the template specializations for Renderables (renderable)
	template<>
	Text* UIDriver::createRenderable<Text>(RenderableContainer* parent);
	template<>
	RenderRectOutline* UIDriver::createRenderable<RenderRectOutline>(RenderableContainer* parent);
	template<>
	RenderRectFill* UIDriver::createRenderable<RenderRectFill>(RenderableContainer* parent);
	template<>
	RenderRectFillRound* UIDriver::createRenderable<RenderRectFillRound>(RenderableContainer* parent);

	class UIDriverObject
	{
	private:
		UIDriver& m_uiDriver;
	protected:
		UIDriverObject(UIDriver& uiDriver) : m_uiDriver(uiDriver) { }
	public:
		UIDriver& getUIDriver() { return m_uiDriver; }
		const UIDriver& getUIDriver() const { return m_uiDriver; }
		IGfxDriver& getGfxDriver() { return getUIDriver().getGfxDriver(); }
		const IGfxDriver& getGfxDriver() const { return getUIDriver().getGfxDriver(); }
		IInputDriver& getInputDriver() { return getUIDriver().getInputDriver(); }
		const IInputDriver& getInputDriver() const { return getUIDriver().getInputDriver(); }
	};
}