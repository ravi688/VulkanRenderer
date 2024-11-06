#include <sutk/IRunnable.hpp>
#include <sutk/UIDriver.hpp>

#include <common/assert.h> // for _com_asssert

namespace SUTK
{
	void IRunnable::Update(IRunnable* runnable) noexcept
	{
		_com_assert(runnable != NULL);
		if(runnable->isRunning())
			runnable->update();
	}

	Runnable::Runnable(UIDriver& driver) noexcept : m_uiDriver(driver)
	{
		driver.addRunnable(this);
	}
	Runnable::~Runnable() noexcept
	{
		m_uiDriver.removeRunnable(this);
	}
}