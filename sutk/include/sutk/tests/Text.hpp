#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

namespace SUTK
{
	class Text : public ITest
	{
	private:

	public:
		Text() = default;

		DriverInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
