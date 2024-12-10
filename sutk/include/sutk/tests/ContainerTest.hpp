#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;

	class ContainerTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IInputDriver* m_inputDriver;
		IGfxDriver* m_gfxDriver;

	public:
		ContainerTest() : m_uiDriver(NULL), m_inputDriver(NULL), m_gfxDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
