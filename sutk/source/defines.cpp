#include <sutk/defines.hpp>

namespace SUTK
{
	SUTK_API std::ostream& operator<<(std::ostream& stream, CardinalDir cd) noexcept
	{
		switch(cd)
		{
			case CardinalDir::Left: stream << "CardinalDir::Left"; break;
			case CardinalDir::Right: stream << "CardinalDir::Right"; break;
			case CardinalDir::Up: stream << "CardinalDir::Up"; break;
			case CardinalDir::Down: stream << "CardinalDir::Down"; break;
		}
		return stream;
	}
}