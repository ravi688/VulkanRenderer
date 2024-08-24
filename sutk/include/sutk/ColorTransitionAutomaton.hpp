#include <sutk/defines.hpp> // for SUTK_API and SUTK::Colo4

#include <common/TransitionAutomaton.hpp> // for com::TransitionAutomaton

namespace SUTK
{
	typedef com::TransitionAutomaton<Color4, Color4::Lerp> ColorTransitionAutomaton;
}