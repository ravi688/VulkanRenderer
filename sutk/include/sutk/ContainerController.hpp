#pragma once

#include <sutk/defines.hpp>

#include <sutk/Container.hpp>
#include <sutk/IRunnable.hpp>

#include <common/TransitionAutomaton.hpp>

namespace SUTK
{
	class SUTK_API ContainerController : public IRunnable
	{
	public:
		// States
		enum State : typename com::TransitionAutomaton<f32>::StateType
		{
			Visible,
			Hidden
		};

	private:
		Container* m_container;
		com::TransitionAutomaton<f32> m_transAutomaton;


		void setActive(com::Bool isActive) noexcept;
		void setAlpha(f32 alpha) noexcept;
	public:
		ContainerController(Container* container, bool isActive = true) noexcept;

		virtual bool isRunning() override;
		virtual void update() override;

		void present() noexcept;
		void hide() noexcept;

		void presentImmediate() noexcept;
		void hideImmediate() noexcept;

		com::TransitionAutomaton<f32>& getAutomaton() noexcept { return m_transAutomaton; }
	};
}