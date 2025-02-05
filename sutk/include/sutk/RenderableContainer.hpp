#pragma once

#include <sutk/Container.hpp>

namespace SUTK
{
	class Renderable;

	class RenderableContainer : public Container
	{
		friend class Renderable;
		friend class UIDriver;
	private:
		Renderable* m_renderable;

		// This must never be called outside of the RenderableContainer and Renderable class
		void setRenderable(Renderable* renderable) noexcept;
	protected:

		RenderableContainer(UIDriver& driver, Container* parent = nullptr, com::Bool isLayoutIgnore = com::Bool::False(), Layer layer = InvalidLayer) noexcept;
		~RenderableContainer() noexcept;
		
		virtual void onAdd(Container* parent) override;
		virtual void onParentResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onResize(const Rect2Df& newRect, bool isPositionChanged, bool isSizeChanged) override;
		virtual void onAnscestorChange(Container* anscestor) noexcept override;
		
	public:
		virtual Renderable* getRenderable() noexcept { return m_renderable; }
	};
}