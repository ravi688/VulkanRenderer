#include <sutk/defines.hpp> // for SUTK_API and SUTK::Colo4

#include <chrono> // for timing
#include <initializer_list> // for std::initializer_list
#include <unordered_map> // for std::unordered_map

namespace SUTK
{
	class ColorTransitionAutomaton
	{
		std::unordered_map<u32, Color4> m_stateColorMap;

		bool m_isRunning;

		// in seconds
		f32 m_transitionDelay;
		Color4 m_prevColor;
		// Color of the rect at any given point of time.
		Color4 m_currentColor;
		// Final Color being chased during the course of transition.
		Color4 m_destColor;

		std::chrono::time_point<std::chrono::high_resolution_clock> m_prevTime;

	public:
		ColorTransitionAutomaton(std::initializer_list<std::pair<const u32, Color4>> stateColorPairs) noexcept;
		bool isRunning() noexcept;
		void update() noexcept;

		Color4 getColor() const noexcept { return m_currentColor; }

		void set(u32 state, Color4 color) noexcept;

		void setTransitionDelay(f32 transitionDelay) noexcept { m_transitionDelay = transitionDelay; }
		void setDefault(u32 state) noexcept;
		void transitionTo(u32 state) noexcept;
	};
}
