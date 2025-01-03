#pragma once

#include <sutk/ITest.hpp>

#include <sge-cpp/sge.hpp>

#include <sutk/UIDriver.hpp>

#include <vector>

namespace SUTK
{
	class IGfxDriver;
	class FullWindowContainer;
	class NotebookView;
	class Label;

	class NotebookTest : public ITest
	{
	private:
		UIDriver* m_uiDriver;
		IGfxDriver* m_gfxDriver;
		IInputDriver* m_inputDriver;
		FullWindowContainer* m_rootContainer {};
		NotebookView* m_notebookView {};
		std::vector<Label*> m_labels;

	public:
		NotebookTest() : m_uiDriver(NULL), m_gfxDriver(NULL), m_inputDriver(NULL) { }

		TestInitializationData getInitializationData() override;

		void initialize(SGE::Driver& driver) override;

		void terminate(SGE::Driver& driver) override;

		void render(SGE::Driver& driver) override;

		void update(SGE::Driver& driver, float deltaTime) override;
	};
}
