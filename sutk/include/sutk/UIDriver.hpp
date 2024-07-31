#pragma once

#include <sutk/defines.hpp>

#include <vector> /* for std::vector */

namespace SUTK
{
	class IRenderable;
	class Container;
	class FullWindowContainer;
	class TextContainer;
	class RenderRectContainer;
	class Text;
	class RenderRectOutline;
	class RenderRectFill;
	class IGfxDriver;

	template<typename RenderableType>
	struct RenderableContainer { };

	template<>
	struct RenderableContainer<Text> { typedef TextContainer type; };

	template<>
	struct RenderableContainer<RenderRectOutline> { typedef RenderRectContainer type; };

	template<>
	struct RenderableContainer<RenderRectFill> { typedef RenderRectContainer type; };

	class UIDriver
	{
	private:
		IGfxDriver& m_gfxDriver;
		std::vector<IRenderable*> m_renderables;
	public:
		UIDriver(IGfxDriver& gfxDriver);

		void render();

		IGfxDriver& getGfxDriver() { return m_gfxDriver; }

		template<typename ContainerType>
		ContainerType* createContainer(Container* parent = NULL) { return NULL; }
		template<typename RenderableType>
		RenderableType* createRenderable(typename RenderableContainer<RenderableType>::type* parent = NULL) { return NULL; }
		Text* createText(TextContainer* parent);
		template<typename RenderRectType>
		RenderRectType* createRenderRect(RenderRectContainer* container);
	};

	// declarations for the template specializations for Containers (non-renderable)
	template<>
	Container* UIDriver::createContainer<Container>(Container* parent);
	template<>
	FullWindowContainer* UIDriver::createContainer<FullWindowContainer>(Container* parent);
	template<>
	TextContainer* UIDriver::createContainer<TextContainer>(Container* parent);
	template<>
	RenderRectContainer* UIDriver::createContainer<RenderRectContainer>(Container* parent);

	// declarations for the template specializations for Renderables (renderable)
	template<>
	Text* UIDriver::createRenderable<Text>(typename RenderableContainer<Text>::type* parent);
	template<>
	RenderRectOutline* UIDriver::createRenderable<RenderRectOutline>(typename RenderableContainer<RenderRectOutline>::type* parent);
	template<>
	RenderRectFill* UIDriver::createRenderable<RenderRectFill>(typename RenderableContainer<RenderRectFill>::type* parent);

	class UIDriverObject
	{
	private:
		UIDriver& m_uiDriver;
	protected:
		UIDriverObject(UIDriver& uiDriver) : m_uiDriver(uiDriver) { }
	public:
		UIDriver& getUIDriver() { return m_uiDriver; }
		IGfxDriver& getGfxDriver() { return getUIDriver().getGfxDriver(); }
	};
}