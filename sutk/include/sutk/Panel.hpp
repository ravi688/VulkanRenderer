#include <sutk/PanelContainer.hpp>
#include <sutk/RenderableContainer.hpp>

namespace SUTK
{
    class RenderRectFill;
    // Renders a Solid Colored Box while also blocking any events through it
    class SUTK_API Panel : public TPanelContainer<RenderableContainer>
    {
    private:
        RenderRectFill* m_renderRect;
    public:
        Panel(UIDriver& driver, Container* parent) noexcept;
        virtual ~Panel() noexcept;
        void setColor(Color4 color) noexcept;
    };
} // namespace SUTK
