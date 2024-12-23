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
#include <sutk/tests/LunaSVGTest.hpp>
#include <sutk/tests/ThemeTest.hpp>
#include <sutk/tests/ThemeLoadTest.hpp>
#include <sutk/tests/TransientContTest.hpp>
#include <sutk/tests/ImageAspectRatioTest.hpp>
#include <sutk/tests/ImageButtonGraphicTest.hpp>
#include <sutk/tests/ToggleButtonTest.hpp>
#include <sutk/tests/ToggleButtonGroupTest.hpp>
#include <sutk/tests/ContainerTest.hpp>
#include <sutk/tests/FontTest.hpp>
#include <sutk/tests/NotebookTest.hpp>
#include <sutk/tests/MultipleThemeModelTest.hpp>
#include <sutk/tests/BaseThemeTest.hpp>
#include <sutk/tests/DarkLightThemeTest.hpp>
#include <sutk/tests/PanelTest.hpp>
#include <sutk/tests/LabelTest.hpp>
#include <sutk/tests/PassiveLabelTest.hpp>

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
		{ "TRANSIENT_CONT", [] () { return std::unique_ptr<ITest>(new TransientContTest()); }},
		{ "IMAGE_ASPECT_RATIO", [] () { return std::unique_ptr<ITest>(new ImageAspectRatioTest()); }},
		{ "IMAGE_BUTTON_GRAPHIC", [] () { return std::unique_ptr<ITest>(new ImageButtonGraphicTest()); }},
		{ "TOGGLE_BUTTON", [] () { return std::unique_ptr<ITest>(new ToggleButtonTest()); }},
		{ "TOGGLE_BUTTON_GROUP", [] () { return std::unique_ptr<ITest>(new ToggleButtonGroupTest()); }},
		{ "CONTAINER", [] () { return std::unique_ptr<ITest>(new ContainerTest()); }},
		{ "FONT", [] () { return std::unique_ptr<ITest>(new FontTest()); }},
		{ "NOTEBOOK", [] () { return std::unique_ptr<ITest>(new NotebookTest()); }},
		{ "LUNASVG", [] () { return std::unique_ptr<ITest>(new LunaSVGTest()); }},
		{ "MULTIPLE_THEME_MODEL", [] () { return std::unique_ptr<ITest>(new MultipleThemeModelTest()); }},
		{ "BASE_THEME", [] () { return std::unique_ptr<ITest>(new BaseThemeTest()); }},
		{ "DARK_LIGHT_THEME", [] () { return std::unique_ptr<ITest>(new DarkLightThemeTest()); }},
		{ "PANEL", [] () { return std::unique_ptr<ITest>(new PanelTest()); }},
		{ "LABEL", [] () { return std::unique_ptr<ITest>(new LabelTest()); }},
		{ "PASSIVE_LABEL", [] () { return std::unique_ptr<ITest>(new PassiveLabelTest()); }}
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
