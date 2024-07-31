#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class RenderRect;

	class RenderRectContainer : public Container
	{
	private:
		RenderRect* m_renderRect;

		// this can only be called by SUTK::UIDriver
		RenderRectContainer(UIDriver& driver, Container* parent) : Container(driver, parent), m_renderRect(NULL) { }

		friend class UIDriver;

	protected:
		virtual void onAdd(Container* parent) override;
		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

	public:

		void setRenderRect(RenderRect* text);

		RenderRect* getRenderRect() noexcept { return m_renderRect; }
	};
}