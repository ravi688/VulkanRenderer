#pragma once

#include <sutk/defines.hpp>
#include <sutk/IGfxDriver.hpp> // for SUTK::IGfxDriver::getSizeInCentimeters()
#include <sutk/Concepts.hpp> // for SUTK::ContainerT concept

#include <common/Reference.hpp> // for com::Reference
#include <common/UnorderedEraseSafeVectorProxy.hpp> // for com::UnorderedEraseSafeVectorProxy<>
#include <common/Utility.hpp> // for com::BinaryFileLoadResult
#include <vector> // for std::vector
#include <utility> // for std::forward
#include <chrono> // for std::chrono

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

	class AnimationEngine;
	class BuiltinThemeManager;

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
		// Runnables
		com::UnorderedEraseSafeVectorProxy<IRunnable*> m_runnables;
		OrderedInputEventsDispatcher* m_orderedEventsDispatcher;
		AnimationEngine* m_animationEngine;
		BuiltinThemeManager* m_themeManager;

		com::Bool m_isCalledFirstTime;
		std::chrono::time_point<std::chrono::steady_clock> m_prevTime;
		f32 m_deltaTime;
		// Caches the draw order range calculated in the last draw order recalculation
		// This value is used to determine if new draw order range is different from the last calculated one
		u32 m_drawOrderRange { U32_MAX };

		// It should be set com::True if the current frame needs to be drawn
		// Generally, it should remain set to com::False unless some UI component/widget has been marked as dirty or updated its appearance
		com::Bool m_isRenderThisFrame;

		friend class Renderable;

		// Only meant to be used by Renderable
		void addRenderable(Renderable* renderable) noexcept;
		void removeRenderable(Renderable* renderable) noexcept;

	public:
		UIDriver(IGfxDriver& gfxDriver, IInputDriver& inputDriver) noexcept;
		UIDriver(IGfxDriver& gfxDriver) noexcept;
		~UIDriver();

		com::Bool isDirty() noexcept;
		void render();

		void addRunnable(IRunnable* runnable) noexcept;
		void removeRunnable(IRunnable* runnable) noexcept;

		ImageReference loadImage(std::string_view path) noexcept;
		ImageReference loadImage(const std::span<const u8> span) noexcept;
		ImageReference loadImage(std::span<const std::byte> span) noexcept { return loadImage(std::span<const u8> { reinterpret_cast<const u8*>(span.data()), span.size() }); }
		ImageReference loadImage(const u8* pixelData, u32 width, u32 height, u32 numChannels) noexcept;
		ImageReference loadImage(com::BinaryFileLoadResult result) noexcept;
		void unloadImage(ImageReference id) noexcept;
		ImageAttributes getImageAttributes(ImageReference id) noexcept;
		FontReference loadFont(std::string_view path) noexcept;
		FontReference loadFont(com::BinaryFileLoadResult result) noexcept;
		void unloadFont(FontReference id) noexcept;

		f32 getDeltaTime() const noexcept { return m_deltaTime; }

		OrderedInputEventsDispatcher& getOrderedInputEventsDispatcher() noexcept { return *m_orderedEventsDispatcher; }
		AnimationEngine& getAnimationEngine() noexcept { return *m_animationEngine; }
		BuiltinThemeManager& getBuiltinThemeManager() noexcept { return *m_themeManager; }

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

		template<UIDriverObjectT ObjectType>
		void destroyObject(ObjectType* obj) noexcept
		{
			delete obj;
		}

		template<ContainerT ContainerType, typename... Args>
		ContainerType* createContainer(Args&&... args) noexcept
		{
			return createObject<ContainerType>(std::forward<Args&&>(args)...);
		}

		template<ContainerT ContainerType>
		void destroyContainer(ContainerType* container) noexcept
		{
			destroyObject<ContainerType>(container);
		}

		template<RenderableT RenderableType, RenderableContainerT RenderableContainerType, typename... Args>
		RenderableType* createRenderable(RenderableContainerType* parent, Args&&... args) noexcept
		{
			return createObject<RenderableType>(parent, std::forward<Args&&>(args)...);
		}

		template<RenderableT RenderableType>
		void destroyRenderable(RenderableType* renderable) noexcept
		{
			destroyObject<RenderableType>(renderable);
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