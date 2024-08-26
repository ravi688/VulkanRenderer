#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

namespace SUTK
{
	class IGfxDriver;

	class HBoxContainer;

	class HBoxTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		HBoxContainer* m_hBoxContainer;

	public:
		HBoxTest() : m_uiDriver(NULL), m_gfxDriver(NULL) { }

		DriverInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
