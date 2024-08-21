#include <sutk/ColorTransitionAutomaton.hpp>

#include <common/assert.h> // for _com_assert

namespace SUTK
{
	ColorTransitionAutomaton::ColorTransitionAutomaton(std::initializer_list<std::pair<const u32, Color4>> stateColorPairs) noexcept : m_stateColorMap(stateColorPairs),
																																 m_isRunning(false),
																																 m_transitionDelay(0.3f)
	{
		_com_assert(stateColorPairs.size() > 0);
		if(stateColorPairs.size() > 0)
		{
			m_currentColor = stateColorPairs.begin()->second;
			m_prevColor = m_currentColor;
			m_destColor = m_currentColor;
		}
	}

	bool ColorTransitionAutomaton::isRunning() noexcept
	{
		return m_isRunning;
	}

	void ColorTransitionAutomaton::update() noexcept
	{
		if(m_currentColor.equalsApprox(m_destColor))
		{
			m_currentColor = m_destColor;
			m_isRunning = false;
		}
		else
		{
			auto currTime = std::chrono::high_resolution_clock::now();
			f32 timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currTime - m_prevTime).count() * 0.001f;
			m_currentColor = Color4::Lerp(m_prevColor, m_destColor, timeElapsed / m_transitionDelay);
		}
	}

	void ColorTransitionAutomaton::set(u32 state, Color4 color) noexcept
	{
		m_stateColorMap[state] = color;
	}

	void ColorTransitionAutomaton::setDefault(u32 state) noexcept
	{
		auto it = m_stateColorMap.find(state);
		_com_assert(it != m_stateColorMap.end());
		m_currentColor = it->second;
		m_prevColor = m_currentColor;
	}

	void ColorTransitionAutomaton::transitionTo(u32 state) noexcept
	{
		m_isRunning = true;
		auto it = m_stateColorMap.find(state);
		_com_assert(it != m_stateColorMap.end());
		m_destColor = it->second;
		m_prevColor = m_currentColor;
		m_prevTime = std::chrono::high_resolution_clock::now();
	}
}