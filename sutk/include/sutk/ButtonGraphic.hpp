#pragma once

#include <sutk/defines.hpp>
#include <sutk/RenderRectFillRound.hpp> // for SUTK::RenderRectFillRound
#include <sutk/ColorTransitionAutomaton.hpp> // for SUTK::ColorTransitionAutomaton
#include <sutk/RenderableContainer.hpp> // for SUTK::RenderableContainer
#include <sutk/Concepts.hpp> // for SUTK::ContainerT concept
#include <sutk/IRunnable.hpp> // for SUTK::Runnable

namespace SUTK
{
	struct HoverInfo
	{
		Vec2Df position;
		bool isEnter;
		bool isExit;
	};

	class SUTK_API IButtonGraphic
	{
	public:
		// Called every time mouse pointer changes its position but still stays within the boundaries of Button's Rect
		// position is in local coordinates of the Button's Rect
		virtual void onHover(HoverInfo info) = 0;
		// Called once for each left-mouse-button down within the boundaries of Button's Rect
		virtual void onPress() = 0;
		// Called once for each left-mouse-button up/release within the boundaries of Button's Rect or even outside if earlier press was within the boundaries.
		virtual void onRelease() = 0;
		virtual Vec2Df getMinBoundSize() = 0;
	};

	class SUTK_API ColorDriverButtonGraphic : public IButtonGraphic,
											  public Runnable
	{
	public:
		// States
		enum State : u32
		{
			Hover,
			Press,
			Idle
		};

	protected:
		ColorTransitionAutomaton m_cta;

		void setState(State state) noexcept;
		// Must be implemented by the deriving class
		virtual void onColorChange(Color4 color) noexcept = 0;

	public:
		ColorDriverButtonGraphic(UIDriver& driver) noexcept;

		// Implementation of Runnable
		virtual bool isRunning() override;
		virtual void update() override;

		// Implementation of IButtonGraphic's functions onHover, onPress, and onRelease
		virtual void onHover(HoverInfo info) noexcept override;
		virtual void onPress() noexcept override;
		virtual void onRelease() noexcept override;

		// Redeclaration of IButtonGraphic::getMinBoundSize() virtual function
		// Must be implemented by the deriving class
		virtual Vec2Df getMinBoundSize() = 0;

		void setTransitionDelay(const f32 transitionDelay) noexcept;

		void setHoverColor(Color4 color) noexcept;
		void setPressColor(Color4 color) noexcept;
		void setIdleColor(Color4 color) noexcept;
	};

	template<ContainerT ContainerType>
	class SUTK_API ColorDriverButtonGraphicContainer : public ColorDriverButtonGraphic,
													   public ContainerType
	{
	public:
		ColorDriverButtonGraphicContainer(UIDriver& driver, Container* parent) noexcept;
		// Override of ContainerType::setActive();
		void setActive(bool isActive) noexcept override;
	};

	template<ContainerT ContainerType>
	ColorDriverButtonGraphicContainer<ContainerType>::ColorDriverButtonGraphicContainer(UIDriver& driver, Container* parent) noexcept : ColorDriverButtonGraphic(driver), ContainerType(driver, parent)
	{

	}

	template<ContainerT ContainerType>
	void ColorDriverButtonGraphicContainer<ContainerType>::setActive(bool isActive) noexcept
	{
		ContainerType::setActive(isActive);
		if(isActive)
		{
			m_cta.setDefault(State::Idle);
			onColorChange(m_cta.getValue());
		}
	}

	class SUTK_API DefaultButtonGraphicNoLabel : public ColorDriverButtonGraphicContainer<RenderableContainer>
	{
	private:
		// Visual Representation of the Button's existence
		RenderRectFillRound* m_renderRect;

	protected:
		// Implementation of ColorDriverButtonGraphic::onColorChange()
		// Must be called in the overriding method
		virtual void onColorChange(Color4 color) noexcept override;

	public:
		DefaultButtonGraphicNoLabel(UIDriver& driver, Container* parent) noexcept;

		// Implementation of ColorDriverButtonGraphicContainer<RenderableContainer>::getMinBoundSize()
		virtual Vec2Df getMinBoundSize() noexcept override;

		RenderRectFillRound& getRenderRect() noexcept { return *m_renderRect; }
	};

	class Label;
	class SUTK_API DefaultButtonGraphic : public DefaultButtonGraphicNoLabel
	{
		private:		
			Label* m_label;
		public:
			DefaultButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup = GFX_DRIVER_OBJECT_NULL_HANDLE) noexcept;

			// Overrides
			virtual Vec2Df getMinBoundSize() noexcept override;

			Label& getLabel() noexcept { return *m_label; }
	};

	class RenderImage;
	class SUTK_API ImageButtonGraphic : public ColorDriverButtonGraphicContainer<RenderableContainer>
	{
	private:
		RenderableContainer* m_imageCont;
		RenderImage* m_image;
	protected:
		// Implementaion of ColorDriverButtonGraphicContainer<RenderableContainer>::onColorChange()
		virtual void onColorChange(Color4 color) noexcept override;
	public:
		ImageButtonGraphic(UIDriver& driver, Container* parent) noexcept;

		void setImage(UIDriver::ImageReference image) noexcept;
		RenderImage& getRenderImage() noexcept { return *m_image; }
	};
}