#include <sutk/Panel.hpp>
#include <sutk/UIDriver.hpp>
#include <sutk/RenderRectFill.hpp>

namespace SUTK
{
    Panel::Panel(UIDriver& driver, Container* parent) noexcept : TPanelContainer<RenderableContainer>(driver, parent)
    {
        m_renderRect = getUIDriver().createRenderable<RenderRectFill>(this);
    }

    Panel::~Panel() noexcept
    {
        getUIDriver().destroyRenderable<RenderRectFill>(m_renderRect);
    }

    void Panel::setColor(Color4 color) noexcept
    {
        m_renderRect->setColor(color);
    }
}