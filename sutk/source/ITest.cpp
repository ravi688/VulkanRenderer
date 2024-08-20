#include <sutk/ITest.hpp>

#include <unordered_map>
#include <memory>


#include <sutk/tests/BitmapTextTest.hpp>
#include <sutk/tests/TextTest.hpp>
#include <sutk/tests/AnchorTest.hpp>
#include <sutk/tests/MouseTest.hpp>
#include <sutk/tests/RoundRectTest.hpp>

namespace SUTK
{

	typedef std::unique_ptr<ITest> (*TestCreateCallback)();

	std::unordered_map<std::string, TestCreateCallback> gTestCreateCallbackMap =
	{
		{ "BITMAP_TEXT", [] () { return std::unique_ptr<ITest>(new BitmapTextTest()); }},
		{ "TEXT", [] () { return std::unique_ptr<ITest>(new TextTest()); }},
		{ "ANCHOR", [] () { return std::unique_ptr<ITest>(new AnchorTest()); }},
		{ "MOUSE", [] () { return std::unique_ptr<ITest>(new MouseTest()); }},
		{ "ROUND_RECT", [] () { return std::unique_ptr<ITest>(new RoundRectTest()); }}
	};

	std::unique_ptr<ITest> ITest::Create(const std::string& testName)
	{
		auto it = gTestCreateCallbackMap.find(testName);
		if(it == gTestCreateCallbackMap.end())
			return { };
		else
			return it->second();
	}
}
