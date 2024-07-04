
#include <sutk/debug.h>
#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <chrono>
#include <memory>

int main(int argc, char** argv)
{
	debug_log_info("SUTK Repository!");

	std::unique_ptr<SUTK::ITest> test = SUTK::ITest::Create((argc > 1) ? argv[1] : "");
	SUTK::DriverInitializationData data = test->getInitializationData();

	SGE::Driver driver(data.gpuType, data.width, data.height, data.title, data.fullScreen, data.resizable);

	test->initialize(driver);

	auto start = std::chrono::high_resolution_clock::now();

	while(driver.isRunning())
	{
		// begin command buffer recording
		driver.beginFrame();
		test->render(driver);
		// end command buffer recording 
		driver.endFrame();

		// dispatch the command buffers for execution
		driver.update();

		// update the scene
		auto end = std::chrono::high_resolution_clock::now();
		test->update(driver, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
	}

	driver.waitIdle();
	
	test->terminate(driver);

	return 0;
}
