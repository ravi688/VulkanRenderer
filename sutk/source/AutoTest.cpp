#include <sutk/AutoTest.hpp>
#include <sge-cpp/sge.hpp> // for SGE::Driver
#include <sutk/SGEGfxDriver.hpp> // for SUTK::SGEGfxDriver
#include <sutk/SGEInputDriver.hpp> // for SUTK::SGEInputDriver
#include <sutk/UIDriver.hpp> // for SUTK::UIDriver
#include <common/Formatters.hpp> // for formatting std::source_location
#include <vector> // for std::vector<>
#include <memory> // for std::unique_ptr<>

namespace SUTK::AutoTests
{
	typedef std::vector<AutoTest*> AutoTestList;

	// We are using lazy instantiation here (by dynamically instantiating AutoTestList object)
	// This avoids so called 'Static Order Initialization Fiasco".
	static std::shared_ptr<AutoTestList> gAutoTests;

	AutoTest::AutoTest(const std::string_view str, com::Bool isUIDriverRequired) noexcept : m_name(str), m_isUIDriverRequired(isUIDriverRequired)
	{
		if(gAutoTests == nullptr)
			gAutoTests = std::make_shared<AutoTestList>();
		gAutoTests->push_back(this);
	}

	// Returns list of AutoTest objects in the project
	std::shared_ptr<AutoTestList> GetTests() noexcept
	{
		return gAutoTests;
	}
}


struct UIDriverContext
{
	std::unique_ptr<SGE::Driver> gpuDriver;
	std::unique_ptr<SUTK::SGEGfxDriver> gfxDriver;
	std::unique_ptr<SUTK::SGEInputDriver> inputDriver;
	std::unique_ptr<SUTK::UIDriver> uiDriver;
};

static std::unique_ptr<SGE::Driver> CreateGPUDriver(const std::string_view testName)
{
	std::unique_ptr<SGE::Driver> driver = std::make_unique<SGE::Driver>(
												SGE::GPUType::Discrete, 
												800, 
												800, 
												testName, 
												false, 
												true, 
												SGE::Requirements { /* require bitmap text: */ true });
	// NRVO would kick-in here
	return driver;
}

static UIDriverContext StartUIDriverContext(const std::string_view testName)
{
	UIDriverContext ctx { };
	ctx.gpuDriver = CreateGPUDriver(testName);
	ctx.gfxDriver = std::make_unique<SUTK::SGEGfxDriver>(*ctx.gpuDriver);
	ctx.inputDriver = std::make_unique<SUTK::SGEInputDriver>(*ctx.gpuDriver);
	ctx.uiDriver = std::make_unique<SUTK::UIDriver>(*ctx.gfxDriver, *ctx.inputDriver);

	// std::move(ctx) would be called here implicitly
	return ctx;
}

static void RenderOneFrame(UIDriverContext& ctx)
{
	// NOTE: Internally, SGE::Driver calls glfwWindowShouldClose, so it must always be called in a while loop
	// But here we are test running an automated test which just requires one frame.
	if(ctx.gpuDriver->isRunning())
	{
		// Now when and what to render is handled by SUTK::UIDriver
		// NOTE: SUTK::UIDriver::render() internally calls SUTK::IGfxDriver::render() which in turn calls SGE::Driver::render()
		if(ctx.uiDriver->isDirty())
			ctx.uiDriver->render();
		// Poll Window events here
		ctx.gpuDriver->pollEvents();
	}
}

using namespace com::formatters::source_location;

int main()
{
	auto tests = SUTK::AutoTests::GetTests();
	if(!tests)
	{
		std::cout << "No tests to run" << std::endl;
		return 0;
	}
	std::size_t failCount = 0;
	for(std::size_t i = 0; auto& test : *tests)
	{
		std::cout << std::format("[{}] Running: ", i)  << test->getName() << std::endl;
		SUTK::AutoTests::TestResult result;
		if(test->isUIDriverRequired())
		{
			UIDriverContext ctx = StartUIDriverContext(test->getName());
			result = test->run(ctx.uiDriver.get());
			// Try rendering one frame to check if the UIDriver's API is correctly used or the program exits abnormally
			RenderOneFrame(ctx);
		}
		else result = test->run(nullptr);
		std::cout << std::format("[{}] Result: ", i) << (result.isPassed ? "Passed" : "Failed");
		// If test failed then print the location of the check which led to the failure
		if(!result.isPassed)
		{
			failCount += 1;
			std::cout << ": " << result.whatFailed <<", at: " << result.failureLocation;
		}
		std::cout << std::endl;
		i += 1;
	}
	if(failCount == 0)
		std::cout << "All tests passed" << std::endl;
	else
		std::cout << "Number of Failed Tests: " << failCount << std::endl;
	return static_cast<int>(failCount);
}