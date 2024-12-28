#pragma once

#include <common/Bool.hpp> // for com::Bool
#include <string> // for std::string
#include <source_location> // for std::source_location

namespace SUTK
{
	// Forward declaration of SUTK::UIDriver
	class UIDriver;
}

namespace SUTK::AutoTests
{
	// Holds Result of a AutoTest, Object of this types are returned by 'run' function in AutoTest class
	struct TestResult
	{
		// By Default com::False
		com::Bool isPassed { };
		// If isPassed holds com::False then this must hold information about the test failure location
		std::source_location failureLocation { };
	};

	// Abstract class from which any Test which is supposed to be run without human in the loop must derive.
	class AutoTest
	{
	private:
		std::string m_name;
		com::Bool m_isUIDriverRequired;

	public:
		// If isUIDriverRequied is set to com::True then 'UIDriver* driver' passed in run functions will be non-null
		// Otherwise 'driver' would be null.
		AutoTest(const std::string_view testName, com::Bool isUIDriverRequied = com::False) noexcept;
		virtual ~AutoTest() noexcept = default;

		// Following set of pure-virtual functions must be implemented by a AutoTest.

		// if isUIDriverRequied was set to com::False (which is by default), then 'driver' will be null here
		// If you want a pointer to a valid UIDriver object then set 'isUIDriverRequired' to com::True when deriving from this class
		virtual TestResult run(UIDriver* driver) = 0;

		const std::string& getName() const noexcept { return m_name; }
		com::Bool isUIDriverRequired() const noexcept { return m_isUIDriverRequired; }
	};
}