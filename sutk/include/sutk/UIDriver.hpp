#pragma once

#include <sutk/defines.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::getSizeInCentimeters()

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
		typedef u32 Layer;
		static constexpr Layer InvalidLayer = U32_MAX;
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

		IGfxDriver& getGfxDriver() { return m_gfxDriver; }
		const IGfxDriver& getGfxDriver() const { return m_gfxDriver; }
		IInputDriver& getInputDriver() { return *m_inputDriver; }
		const IInputDriver& getInputDriver() const { return *m_inputDriver; }

		Vec2Df getWindowSize() noexcept { return getGfxDriver().getSizeInCentimeters(); }
		GfxDriverObjectHandleType getGlobalTextGroup() noexcept;

		// TODO: create a concept to add constraint on the typename to allow only objects identical or derived from SUTK::Container object
		template<typename ContainerType, typename... Args>
		ContainerType* createContainer(Container* parent, Args&&... args)
		{
			ContainerType* cntr = new ContainerType(*this, parent, std::forward<Args&&>(args)...);
			return cntr;
		}
		// TODO: create a concept to add constraint on the typename to allow only renderables derived from SUTK::Renderable object
		template<typename RenderableType, typename... Args>
		RenderableType* createRenderable(RenderableContainer* parent, Args&&... args)
		{
			RenderableType* renderable = new RenderableType(*this, parent, std::forward<Args&&>(args)...);
			m_renderables.push_back(renderable);
			return renderable;
		}
		template<typename RenderableType, typename... Args>
		std::pair<RenderableType*, RenderableContainer*> createRenderableWithContainer(Container* parent, Args&&... args)
		{
			RenderableContainer* container = createContainer<RenderableContainer>(parent);
			RenderableType* renderable = new RenderableType(*this, container, std::forward<Args&&>(args)...);
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
	template<>
	RenderImage* UIDriver::createRenderable<RenderImage>(RenderableContainer* parent);

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