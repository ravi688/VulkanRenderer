#include <sutk/IRunnable.hpp>

#include <common/assert.h> // for _com_asssert

namespace SUTK
{
	void IRunnable::Update(IRunnable* runnable) noexcept
	{
		_com_assert(runnable != NULL);
		if(runnable->isRunning())
			runnable->update();
	}
}