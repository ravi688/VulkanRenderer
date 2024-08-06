#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class Renderable;

	class RenderableContainer : public Container
	{
	private:
		Renderable* m_renderable;

		// this can only be called by SUTK::UIDriver
		RenderableContainer(UIDriver& driver, Container* parent) : Container(driver, parent), m_renderable(NULL) { }

		friend class UIDriver;

	protected:
		virtual void onAdd(Container* parent) override;
		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;

	public:
		void setRenderable(Renderable* renderable) noexcept;
		Renderable* getRenderable() noexcept { return m_renderable; }
	};
}