#include <sutk/ITest.hpp>

#include <unordered_map>
#include <memory>


#include <sutk/tests/BitmapTextTest.hpp>
#include <sutk/tests/TextTest.hpp>
#include <sutk/tests/AnchorTest.hpp>
#include <sutk/tests/MouseTest.hpp>
#include <sutk/tests/RoundRectTest.hpp>
#include <sutk/tests/ButtonTest.hpp>
#include <sutk/tests/VBoxTest.hpp>
#include <sutk/tests/HBoxTest.hpp>
#include <sutk/tests/HPaneTest.hpp>
#include <sutk/tests/RenderImageTest.hpp>
#include <sutk/tests/ThemeTest.hpp>
#include <sutk/tests/ThemeLoadTest.hpp>
#include <sutk/tests/TransientContTest.hpp>

namespace SUTK
{

	typedef std::unique_ptr<ITest> (*TestCreateCallback)();

	std::unordered_map<std::string, TestCreateCallback> gTestCreateCallbackMap =
	{
		{ "BITMAP_TEXT", [] () { return std::unique_ptr<ITest>(new BitmapTextTest()); }},
		{ "TEXT", [] () { return std::unique_ptr<ITest>(new TextTest()); }},
		{ "ANCHOR", [] () { return std::unique_ptr<ITest>(new AnchorTest()); }},
		{ "MOUSE", [] () { return std::unique_ptr<ITest>(new MouseTest()); }},
		{ "ROUND_RECT", [] () { return std::unique_ptr<ITest>(new RoundRectTest()); }},
		{ "BUTTON", [] () { return std::unique_ptr<ITest>(new ButtonTest()); }},
		{ "VBOX", [] () { return std::unique_ptr<ITest>(new VBoxTest()); }},
		{ "HBOX", [] () { return std::unique_ptr<ITest>(new HBoxTest()); }},
		{ "HPANE", [] () { return std::unique_ptr<ITest>(new HPaneTest()); }},
		{ "RENDER_IMAGE", [] () { return std::unique_ptr<ITest>(new RenderImageTest()); }},
		{ "THEME", [] () { return std::unique_ptr<ITest>(new ThemeTest()); }},
		{ "THEME_LOAD", [] () { return std::unique_ptr<ITest>(new ThemeLoadTest()); }},
		{ "TRANSIENT_CONT", [] () { return std::unique_ptr<ITest>(new TransientContTest()); }}
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
