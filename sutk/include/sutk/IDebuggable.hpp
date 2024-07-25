#pragma once

namespace SUTK
{
	class IDebuggable
	{
	public:
		virtual void enableDebug(bool isEnable) noexcept = 0;
	};
}
