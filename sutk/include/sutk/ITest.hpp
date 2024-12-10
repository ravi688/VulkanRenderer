#pragma once

#include <sge-cpp/sge.hpp>
#include <common/Bool.hpp> // for com::Bool

#include <memory>

namespace SUTK
{
	struct DriverInitializationData
	{
		SGE::GPUType gpuType;
		u32 width;
		u32 height;
		std::string title;
		bool fullScreen;
		bool resizable;
	};

	class ITest
	{

	public:
		virtual ~ITest() = default;

		static std::unique_ptr<ITest> Create(const std::string& testName);

		struct TestInitializationData
		{
			DriverInitializationData driverInitializationData;
			com::Bool isBeginEndDispatchFrame;
		};

		virtual TestInitializationData getInitializationData()
		{
			TestInitializationData data;
			data.driverInitializationData = 
			{
				SGE::GPUType::Discrete,
				800,
				800,
				"Untitled Test",
				false,
				true
			};
			// By default do not call beginFrame() before render(), and endFrame() after render(); and then dispatchFrame()
			data.isBeginEndDispatchFrame = com::False;
			return data;
		}
		virtual void initialize(SGE::Driver& driver) { }
		virtual void terminate(SGE::Driver& driver) { }
		virtual void render(SGE::Driver& driver) { }
		virtual void update(SGE::Driver& driver, float deltaTime) { }
	};
}
