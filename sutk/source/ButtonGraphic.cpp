#include <sutk/ButtonGraphic.hpp>
#include <sutk/Label.hpp> // for SUTK::Label
#include <sutk/SmallText.hpp> // for SUTK::SmallText

#include <common/assert.h> // for _com_assert

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

	DefaultButtonGraphicNoLabel::DefaultButtonGraphicNoLabel(UIDriver& driver, Container* parent) noexcept : RenderableContainer(driver, parent)
	{
		_com_assert(parent != NULL);
		
		// size of this graphic should be as that of Button's rect
		setRect({ { 0, 0 }, parent->getSize() });
		getAnchorRect()->setRect( { 0, 0, 1, 1 });

		m_visualButton = driver.createRenderable<VisualButtonRect>(this);
	}

	/*						____________
						  \|/			|
		Idle Color --> Hover Color --> Press Color
		   /|\				|
			|_______________|

	 */

	void DefaultButtonGraphicNoLabel::onHover(HoverInfo info) noexcept
	{
		if(info.isEnter)
			m_visualButton->setState(VisualButtonRect::State::Hover);
		else if(info.isExit)
			m_visualButton->setState(VisualButtonRect::State::Idle);
	}
	void DefaultButtonGraphicNoLabel::onPress() noexcept
	{
		m_visualButton->setState(VisualButtonRect::State::Press);
	}
	void DefaultButtonGraphicNoLabel::onRelease() noexcept
	{
		m_visualButton->setState(VisualButtonRect::State::Hover);
	}
	Vec2Df DefaultButtonGraphicNoLabel::getMinBoundSize() noexcept
	{
		return getSize();
	}

	DefaultButtonGraphic::DefaultButtonGraphic(UIDriver& driver, Container* parent, GfxDriverObjectHandleType textGroup) noexcept : DefaultButtonGraphicNoLabel(driver, parent)
	{
		m_label = driver.createContainer<Label>(this, textGroup);
		// size of the label's rect should be as that of ButtonGraphic's rect
		m_label->setRect({ { 0, 0 }, getSize() });
		m_label->getAnchorRect()->setRect({ 0.0f, 0.0f, 1.0f, 1.0f });
		m_label->setAlignment(HorizontalAlignment::Middle, VerticalAlignment::Middle);
	}

	Vec2Df DefaultButtonGraphic::getMinBoundSize() noexcept
	{
		f32 xCoord = m_label->getText().getCoordFromColPos(END_OF_LINE);
		Vec2Df size = getSize();
		return { xCoord, size.height };
	}
}