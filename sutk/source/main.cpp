
#include <sutk/debug.h>
#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <chrono>
#include <memory>

int main(int argc, char** argv)
{
	std::unique_ptr<SUTK::ITest> test = SUTK::ITest::Create((argc > 1) ? argv[1] : "");
	if(!test)
	{
		debug_log_error("Invalid test id %s", argv[1]);
		return 0;
	}

	SUTK::DriverInitializationData data = test->getInitializationData();

	SGE::Driver driver(data.gpuType, data.width, data.height, data.title, data.fullScreen, data.resizable, SGE::Requirements { /* require bitmap text: */ true });
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
		test->update(driver, std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() * 0.001f);
		start = end;
	}

	driver.waitIdle();
	
	test->terminate(driver);

	return 0;
}
