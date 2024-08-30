#pragma once

#include <sutk/defines.hpp> // for SUTK_API

namespace SUTK
{
	class SUTK_API IRunnable
	{
	public:
		virtual ~IRunnable() noexcept = default;
		virtual bool isRunning() = 0;
		virtual void update() = 0;

		static void Update(IRunnable* runnable) noexcept;
	};
}