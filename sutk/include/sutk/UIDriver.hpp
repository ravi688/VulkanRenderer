#pragma once

#include <sutk/defines.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::getSizeInCentimeters()
#include <sutk/Concepts.hpp> // for SUTK::ContainerT concept

#include <common/Reference.hpp> // for com::Reference
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
	class RenderImage;
	
	// Forward declarations for Interface classes
	class IGfxDriver;
	class IInputDriver;
	class IRunnable;

	class OrderedInputEventsDispatcher;

	class UIDriver
	{
	public:
		struct AuxImage { };
		typedef com::Reference<GfxDriverObjectHandleType, GFX_DRIVER_OBJECT_NULL_HANDLE, AuxImage> ImageReference;
		static constexpr ImageReference InvalidImage = ImageReference::Null;
		typedef TextureAttributes ImageAttributes;
		struct AuxFont { };
		typedef com::Reference<GfxDriverObjectHandleType, GFX_DRIVER_OBJECT_NULL_HANDLE, AuxFont> FontReference;
		static constexpr FontReference InvalidFont = FontReference::Null;
	private:
		IGfxDriver& m_gfxDriver;
		IInputDriver* m_inputDriver;
		bool m_isDummyInputDriver;
		std::vector<Renderable*> m_renderables;
		GfxDriverObjectHandleType m_globalTextGroup;
		// Container for accomodating debug rectangles etc.
		Container* m_debugRootContainer;
		// Runnables
		// TODO: We might prefer using std::map or std::forward_list here
		// if removal of RunnableService is frequent,
		// that's because, erasing an element from std::vector results in 
		// shifting elements after the erased point.
		std::vector<IRunnable*> m_runnables;
		OrderedInputEventsDispatcher* m_orderedEventsDispatcher;

		friend class Renderable;
	public:
		UIDriver(IGfxDriver& gfxDriver, IInputDriver& inputDriver) noexcept;
		UIDriver(IGfxDriver& gfxDriver) noexcept;
		~UIDriver();

		void render();

		void addRunnable(IRunnable* runnable) noexcept;
		void removeRunnable(IRunnable* runnable) noexcept;

		ImageReference loadImage(std::string_view path) noexcept;
		void unloadImage(ImageReference id) noexcept;
		ImageAttributes getImageAttributes(ImageReference id) noexcept;
		FontReference loadFont(std::string_view path) noexcept;
		void unloadFont(FontReference id) noexcept;

		Container* getDebugRootContainer() noexcept { return m_debugRootContainer; }
		OrderedInputEventsDispatcher& getOrderedInputEventsDispatcher() noexcept { return *m_orderedEventsDispatcher; }

		IGfxDriver& getGfxDriver() { return m_gfxDriver; }
		const IGfxDriver& getGfxDriver() const { return m_gfxDriver; }
		IInputDriver& getInputDriver() { return *m_inputDriver; }
		const IInputDriver& getInputDriver() const { return *m_inputDriver; }

		Vec2Df getWindowSize() noexcept { return getGfxDriver().getSizeInCentimeters(); }
		GfxDriverObjectHandleType getGlobalTextGroup() noexcept;

		template<UIDriverObjectT ObjectType, typename... Args>
		ObjectType* createObject(Args&&... args) noexcept
		{
			ObjectType* obj = new ObjectType(*this, std::forward<Args&&>(args)...);
			return obj;
		}

		// TODO: create a concept to add constraint on the typename to allow only objects identical or derived from SUTK::Container object
		template<ContainerT ContainerType, ContainerT ParentContainerType, typename... Args>
		ContainerType* createContainer(ParentContainerType* parent, Args&&... args)
		{
			return createObject<ContainerType>(parent, std::forward<Args&&>(args)...);
		}
		// TODO: create a concept to add constraint on the typename to allow only renderables derived from SUTK::Renderable object
		template<RenderableT RenderableType, RenderableContainerT RenderableContainerType, typename... Args>
		RenderableType* createRenderable(RenderableContainerType* parent, Args&&... args)
		{
			return createObject<RenderableType>(parent, std::forward<Args&&>(args)...);
		}
		template<RenderableT RenderableType, ContainerT ParentContainerType, typename... Args>
		std::pair<RenderableType*, RenderableContainer*> createRenderableWithContainer(ParentContainerType* parent, Args&&... args)
		{
			RenderableContainer* container = createContainer<RenderableContainer>(parent);
			RenderableType* renderable = createRenderable<RenderableType>(container, std::forward<Args&&>(args)...);
			return { renderable, container };
		}
	};
}