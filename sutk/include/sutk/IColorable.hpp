#pragma once

#include <sutk/defines.hpp> // for SUTK::Colo4 and SUTK_API

namespace SUTK
{
	class SUTK_API IColorable
	{
	public:
		virtual void setColor(Color4 color) = 0;
		virtual Color4 getColor() const = 0;
	};
}