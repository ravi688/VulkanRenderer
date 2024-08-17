#pragma once

namespace SUTK
{
	class IDebuggable
	{
	public:
		virtual void enableDebug(bool isEnable = true, Color4 color = Color4::green()) noexcept = 0;
	};
}
