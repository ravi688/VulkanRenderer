
#include <sutk/debug.h>

#include <sge-cpp/sge.hpp>

int main(int argc, char** argv)
{
	debug_log_info("SUTK Repository!");

	SGE::Driver driver(SGE::GPUType::Discrete, 800, 800, argv[0], false, true);

	while(driver.isRunning())
	{
		driver.beginFrame();
		driver.endFrame();

		driver.update();
	}
	return 0;
}
