#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

namespace SUTK
{
	class TextTest : public ITest
	{
	private:

	public:
		TextTest() = default;

		DriverInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
