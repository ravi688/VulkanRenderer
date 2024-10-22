#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp> // for SUTK::Label
#include <sutk/SmallText.hpp> // for SUTK::SmallText
#include <sutk/RenderImage.hpp> // for SUTK::RenderImage

#include <common/assert.h> // for _com_assert

namespace SUTK
{
	ColorDriverButtonGraphic::ColorDriverButtonGraphic(UIDriver& driver) noexcept : Runnable(driver), m_cta({ { State::Hover, Color4::grey(0.7f) },
																							{ State::Press, Color4::grey(0.3f) },
																							{ State::Idle, Color4::grey(1.0f) } })
	{
		m_cta.setDefault(State::Idle);
	}

	bool ColorDriverButtonGraphic::isRunning()
	{
		return m_cta.isRunning();
	}
	void ColorDriverButtonGraphic::update()
	{
		if(m_cta.isRunning())
		{
			m_cta.update();
			onColorChange(m_cta.getValue());
		}
	}

	void ColorDriverButtonGraphic::setTransitionDelay(const f32 transitionDelay) noexcept
	{
		m_cta.setTransitionDelay(transitionDelay);
	}

	void ColorDriverButtonGraphic::setHoverColor(Color4 color) noexcept
	{
		m_cta.set(State::Hover, color);
	}
	void ColorDriverButtonGraphic::setPressColor(Color4 color) noexcept
	{
		m_cta.set(State::Press, color);
	}
	void ColorDriverButtonGraphic::setIdleColor(Color4 color) noexcept
	{
		m_cta.set(State::Idle,  color);
		m_cta.setDefault(State::Idle);
		onColorChange(m_cta.getValue());
	}
	void ColorDriverButtonGraphic::setState(State state) noexcept
	{
		m_cta.transitionTo(state);
	}

	/*						____________
						  \|/			|
		Idle Color --> Hover Color --> Press Color
		   /|\				|
			|_______________|

	 */

	void ColorDriverButtonGraphic::onHover(HoverInfo info) noexcept
	{
		if(info.isEnter)
			setState(State::Hover);
		else if(info.isExit)
			setState(State::Idle);
	}
	void ColorDriverButtonGraphic::onPress() noexcept
	{
		setState(State::Press);
	}
	void ColorDriverButtonGraphic::onRelease() noexcept
	{
		setState(State::Hover);
	}

	DefaultButtonGraphicNoLabel::DefaultButtonGraphicNoLabel(UIDriver& driver, Container* parent) noexcept : ColorDriverButtonGraphicContainer<RenderableContainer>(driver, parent)
	{
		_com_assert(parent != NULL);
		
		// size of this graphic should be as that of Button's rect
		alwaysFitInParent();

		m_renderRect = driver.createRenderable<RenderRectFillRound>(this);
	}

	void DefaultButtonGraphicNoLabel::onColorChange(Color4 color) noexcept
	{
		m_renderRect->setColor(color);
	}

	DefaultButtonGraphic::DefaultButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : DefaultButtonGraphicNoLabel(driver, parent)
	{
		m_label = driver.createContainer<Label>(this, textGroup);
		// size of the label's rect should be as that of ButtonGraphic's rect
		m_label->alwaysFitInParent();
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
	}

	Vec2Df DefaultButtonGraphic::getMinBoundSize() noexcept
	{
		f32 xCoord = m_label->getText().getCoordFromColPos(END_OF_LINE);
		Vec2Df size = getSize();
		return { xCoord + m_label->getPosition().x, size.height };
	}

	ImageButtonGraphic::ImageButtonGraphic(UIDriver& driver, Container* parent) noexcept : ColorDriverButtonGraphicContainer<RenderableContainer>(driver, parent)
	{
		alwaysFitInParent();
		m_image = driver.createRenderable<RenderImage>(this);

		setHoverColor(Color4::white());
		setPressColor(Color4::white());
		setIdleColor(Color4::white());
	}

	void ImageButtonGraphic::onColorChange(Color4 color) noexcept
	{
		m_image->setColor(color);
	}

	void ImageButtonGraphic::setImage(UIDriver::ImageReference image) noexcept
	{
		m_image->setImage(image);
	}

	ImageOverDefaultButtonGraphicProxy::ImageOverDefaultButtonGraphicProxy(UIDriver& driver, Container* parent) noexcept : UIDriverObject(driver)
	{
		m_defaultGraphic = driver.createContainer<DefaultButtonGraphicNoLabel>(parent);
		m_imageGraphic = driver.createContainer<ImageButtonGraphic>(parent);
		// Draw the image graphic on top of the default button graphic no label.
		m_imageGraphic->setLayer(m_defaultGraphic->getDepth() + 1);
	}

	void ImageOverDefaultButtonGraphicProxy::onHover(HoverInfo info)
	{
		m_defaultGraphic->onHover(info);
		m_imageGraphic->onHover(info);
	}

	void ImageOverDefaultButtonGraphicProxy::onPress()
	{
		m_defaultGraphic->onPress();
		m_imageGraphic->onPress();
	}

	void ImageOverDefaultButtonGraphicProxy::onRelease()
	{
		m_defaultGraphic->onRelease();
		m_imageGraphic->onRelease();
	}

	Vec2Df ImageOverDefaultButtonGraphicProxy::getMinBoundSize()
	{
		Vec2Df size1 = m_defaultGraphic->getMinBoundSize();
		Vec2Df size2 = m_imageGraphic->getMinBoundSize();
		return { std::max(size1.x, size2.x), std::max(size1.y, size2.y) };
	}

}