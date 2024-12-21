#pragma once

#include <sutk/ButtonGraphic.hpp> // for SUTK::IButtonGraphic
#include <sutk/Types.hpp>  // for SUTK::ToggleState

namespace SUTK
{
	class SUTK_API IToggleButtonGraphic : public virtual IButtonGraphic
	{
	public:
		virtual void onToggle(ToggleState state) = 0;
	};

	class RenderRectFillCircle;
	class RenderableContainer;
	class SUTK_API DefaultToggleButtonGraphic : public DefaultButtonGraphic, public IToggleButtonGraphic
	{
	private:
		// Visual representation of Toggle State
		RenderableContainer* m_renderRectCont;
		RenderRectFillCircle* m_renderRect;
	protected:
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
	public:
		DefaultToggleButtonGraphic(UIDriver& driver, Container* parent, std::optional<GfxDriverObjectHandleType> textGroup) noexcept;

		virtual void onToggle(ToggleState state) override;

		RenderRectFillCircle& getToggleRenderRect() noexcept { return *m_renderRect; }
	};
}