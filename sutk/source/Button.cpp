#include <sutk/Button.hpp>

#include <sutk/SmallText.hpp> // for SUTK::SmallText
#include <sutk/Label.hpp> // for SUTK::Label

namespace SUTK
{
	VisualButtonRect::VisualButtonRect(SUTK::UIDriver& driver, SUTK::RenderableContainer* container) noexcept : RenderRectFillRound(driver, container),
																							  m_cta({ { State::Hover, Color4::grey(0.7f) },
																							  		  { State::Press, Color4::grey(0.3f) },
																							  		  { State::Idle, Color4::grey(1.0f) } })
	{
		m_cta.setDefault(State::Idle);
	}

	void VisualButtonRect::setActive(bool isActive) noexcept
	{
		RenderRectFillRound::setActive(isActive);
		if(isActive)
		{
			m_cta.setDefault(State::Idle);
			setColor(m_cta.getValue());
		}
	}

	bool VisualButtonRect::isDirty() noexcept
	{
		return m_cta.isRunning() || RenderRectFillRound::isDirty();
	}
	void VisualButtonRect::update() noexcept
	{
		if(m_cta.isRunning())
		{
			m_cta.update();
			setColor(m_cta.getValue());
		}
		RenderRectFillRound::update();
	}

	void VisualButtonRect::setTransitionDelay(const f32 transitionDelay) noexcept
	{
		m_cta.setTransitionDelay(transitionDelay);
	}

	void VisualButtonRect::setHoverColor(Color4 color) noexcept
	{
		m_cta.set(State::Hover, color);
	}
	void VisualButtonRect::setPressColor(Color4 color) noexcept
	{
		m_cta.set(State::Press, color);
	}
	void VisualButtonRect::setIdleColor(Color4 color) noexcept
	{
		m_cta.set(State::Idle,  color);
		m_cta.setDefault(State::Idle);
		setColor(m_cta.getValue());
	}
	void VisualButtonRect::setState(State state) noexcept
	{
		m_cta.transitionTo(state);
	}

	DefaultButtonGraphic::DefaultButtonGraphic(UIDriver& driver, Container* parent) noexcept : RenderableContainer(driver, parent)
	{
		_com_assert(parent != NULL);
		
		// size of this graphic should be as that of Button's rect
		setRect({ { 0, 0 }, parent->getSize() });
		getAnchorRect()->setRect( { 0, 0, 1, 1 });

		m_visualButton = driver.createRenderable<VisualButtonRect>(this);
		
		m_label = driver.createContainer<Label>(this);
		// size of the label's rect should be as that of ButtonGraphic's rect
		m_label->setRect({ { 0, 0 }, getSize() });
		m_label->getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
	}

	/*						____________
						  \|/			|
		Idle Color --> Hover Color --> Press Color
		   /|\				|
			|_______________|

	 */

	void DefaultButtonGraphic::onHover(HoverInfo info) noexcept
	{
		if(info.isEnter)
			m_visualButton->setState(VisualButtonRect::State::Hover);
		else if(info.isExit)
			m_visualButton->setState(VisualButtonRect::State::Idle);
	}
	void DefaultButtonGraphic::onPress() noexcept
	{
		m_visualButton->setState(VisualButtonRect::State::Press);
	}
	void DefaultButtonGraphic::onRelease() noexcept
	{
		m_visualButton->setState(VisualButtonRect::State::Hover);
	}

	Button::Button(UIDriver& driver, Container* parent, bool isCreateDefaultGraphic) noexcept : Container(driver, parent),
																									MouseMoveHandlerObject(driver, this),
																									MouseClickHandlerObject(driver, this),
																									m_graphic(NULL)
	{ 
		if(isCreateDefaultGraphic)
			m_graphic = driver.createContainer<DefaultButtonGraphic>(this);
		enableMouseEnter(true);
		enableMouseExit(true);
	}

	void Button::onMouseEnter() noexcept
	{
		if(m_graphic != NULL)
		{
			m_onEnterEvent.publish();
			HoverInfo info { };
			info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
			info.isEnter = true;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseExit() noexcept
	{
		if(m_graphic != NULL)
		{
			m_onExitEvent.publish();
			HoverInfo info { };
			info.position = getScreenCoordsToLocalCoords(getInputDriver().getMousePosition());
			info.isExit = true;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseMove(Vec2Df position) noexcept
	{
		if(m_graphic != NULL)
		{
			HoverInfo info { };
			info.position = position;
			m_graphic->onHover(info);
		}
	}

	void Button::onMouseClick(MouseButton button, KeyEvent action) noexcept
	{
		if(button != MouseButton::Left)
			return;
		if(action == KeyEvent::Press)
		{
			m_onPressEvent.publish();
			m_graphic->onPress();
		}
		else if(action == KeyEvent::Release)
		{
			m_onReleaseEvent.publish();
			m_graphic->onRelease();
		}
	}
}