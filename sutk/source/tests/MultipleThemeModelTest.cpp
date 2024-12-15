#include <sutk/tests/MultipleThemeModelTest.hpp>

#include <sutk/RenderImage.hpp>
#include <sutk/RenderRectFill.hpp>
#include <sutk/SGEGfxDriver.hpp>
#include <sutk/SGEInputDriver.hpp>
#include <sutk/RenderableContainer.hpp>
#include <sutk/FullWindowContainer.hpp>
#include <sutk/ThemeManager.hpp>
#include <sutk/Label.hpp>

namespace SUTK
{
	ITest::TestInitializationData MultipleThemeModelTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.driverInitializationData.title = "Multiple Theme Load Test (press B to toggle themes)";
		return data;
	}

	void MultipleThemeModelTest::initialize(SGE::Driver& driver)
	{
		m_gfxDriver = new SGEGfxDriver(driver);
		m_inputDriver = new SGEInputDriver(driver);
		m_uiDriver = new UIDriver(*m_gfxDriver, *m_inputDriver);

		ThemeManager<std::string, std::string_view>* themeManager = new ThemeManager<std::string, std::string_view>(*m_uiDriver);
		ThemeInterface<std::string, std::string_view>* themeInt = themeManager->loadThemeInterface("themes/multiple_theme_model_test/model1.tmdl");
		if(themeInt)
			themeInt->dump();
		else
			DEBUG_LOG_FETAL_ERROR("Failed to load or parse the themes/multiple_theme_model_test/model1.tmdl file");
		ThemeInterface<std::string, std::string_view>* themeInt2 = themeManager->loadThemeInterface("themes/multiple_theme_model_test/model2.tmdl");
		if(themeInt2)
			themeInt2->dump();
		else
			DEBUG_LOG_FETAL_ERROR("Failed to load or parse the themes/multiple_theme_model_test/model2.tmdl file");
		Theme<std::string, std::string_view>* theme1 = themeManager->loadTheme("themes/multiple_theme_model_test/theme1.theme");
		Theme<std::string, std::string_view>* theme2 = themeManager->loadTheme("themes/multiple_theme_model_test/theme2.theme");
		Theme<std::string, std::string_view>* theme3 = themeManager->loadTheme("themes/multiple_theme_model_test/theme3.theme");

		FullWindowContainer* rootContainer = m_uiDriver->createContainer<FullWindowContainer>(com::null_pointer<Container>());
		Container* emptyContainer = m_uiDriver->createContainer<Container>(rootContainer);
		emptyContainer->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor = emptyContainer->getAnchorRect();
		anchor->setTopLeft({ 0, 0 });
		anchor->setBottomRight({ 1, 1 });
		m_renderRectContainer = m_uiDriver->createContainer<RenderableContainer>(emptyContainer);
		m_renderRectContainer->setRect({ 1.0f, 1.0f, 5, 5 });
		anchor = m_renderRectContainer->getAnchorRect();
		anchor->setTopLeft({ 1, 1 });
		anchor->setBottomRight({ 1, 1 });
		emptyContainer->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });
		m_renderRect = m_uiDriver->createRenderable<RenderImage>(m_renderRectContainer);
		themeInt->bind<UIDriver::ImageReference>("Folder.Open", std::bind(&RenderImage::setImage, m_renderRect, std::placeholders::_1));

		Container* emptyContainer1 = m_uiDriver->createContainer<Container>(rootContainer);
		emptyContainer1->setRect({ 1.0f, 1.0f, 7.0f, 7.0f });
		AnchorRect* anchor1 = emptyContainer1->getAnchorRect();
		anchor1->setTopLeft({ 0, 0 });
		anchor1->setBottomRight({ 1, 1 });
		RenderableContainer* renderRectContainer1 = m_uiDriver->createContainer<RenderableContainer>(emptyContainer1);
		renderRectContainer1->setRect({ 7.0f, 1.0f, 5, 5 });
		anchor1 = renderRectContainer1->getAnchorRect();
		anchor1->setTopLeft({ 1, 1 });
		anchor1->setBottomRight({ 1, 1 });
		emptyContainer1->setRect({ 1.0f, 1.0f, 9.0f, 7.0f });
		emptyContainer1->setRect({ 1.0f, 1.0f, 11.0f, 7.0f });
		RenderImage* renderImage = m_uiDriver->createRenderable<RenderImage>(renderRectContainer1);
		themeInt->bind<UIDriver::ImageReference>("Folder.Open.VG", std::bind(&RenderImage::setImage, renderImage, std::placeholders::_1));
		renderImage->setColor(Color4::white());

		RenderableContainer* renderRectContainer2 = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		RenderRectFill* renderRect2 = m_uiDriver->createRenderable<RenderRectFill>(renderRectContainer2);
		renderRectContainer2->setRect({ 5.0f, 10.0f, 5.0f, 5.0f });
		AnchorRect* anchor2 = renderRectContainer2->getAnchorRect();
		anchor2->setTopLeft({ 0.0f, 0.0f });
		anchor2->setBottomRight({ 1.0f, 1.0f });
		themeInt->bind<Color4>("LargeButton.HoverColor", std::bind(&RenderRectFill::setColor, renderRect2, std::placeholders::_1));
		renderRect2->setColor(Color4::red());

		RenderableContainer* renderRectContainer3 = m_uiDriver->createContainer<RenderableContainer>(rootContainer);
		RenderRectFill* renderRect3 = m_uiDriver->createRenderable<RenderRectFill>(renderRectContainer3);
		renderRectContainer3->setRect({ 11.0f, 10.0f, 5.0f, 5.0f });
		AnchorRect* anchor3 = renderRectContainer3->getAnchorRect();
		anchor3->setTopLeft({ 0.0f, 0.0f });
		anchor3->setBottomRight({ 1.0f, 1.0f });
		themeInt2->bind<f32>("AnotherButton.AlphaValue", [renderRect3](f32 value) noexcept
			{ 
				auto color = renderRect3->getColor(); 
				std::cout << "Float value: " << value << std::endl;
				color.alpha(value * 255);
				renderRect3->setColor(color);
			});
		renderRect3->setColor(Color4::yellow());

		Label* label = m_uiDriver->createContainer<Label>(rootContainer);
		label->setRect({ 11.0f, 8.0f, 5.0f, 5.0f });
		AnchorRect* anchor4 = label->getAnchorRect();
		anchor4->setTopLeft({ 0.0f, 0.0f });
		anchor4->setBottomRight({ 1.0f, 1.0f });
		// It is expected that "Hello World From Theme 3" will never be set here
		// As the theme 3 doesn't implement theme model 2
		themeInt2->bind<std::string>("Label.Value", [label](const std::string& value) noexcept
			{
				std::cout << "String value: " << value << std::endl;
				label->set(value);
			});

		themeInt->applyTheme(theme1);

		m_inputDriver->getOnKeyEvent().subscribe([theme1, theme2, theme3](IInputDriver* driver, KeyCode keycode, KeyEvent event, ModifierKeys)
		{
			static u32 index = 0;
			if((keycode == KeyCode::B) && (event == KeyEvent::Press))
			{
				switch(index)
				{
					case 0:
						theme1->apply();
						break;
					case 1:
						theme2->apply();
						break;
					case 2:
						theme3->apply();
						break;
				}
				index = (index + 1) % 3;
			}
		});
	}

	void MultipleThemeModelTest::terminate(SGE::Driver& driver)
	{
		// TODO: m_uiDriver->unloadImage(image);
		delete m_uiDriver;
		delete m_gfxDriver;
	}

	void MultipleThemeModelTest::render(SGE::Driver& driver)
	{
		// SUTK::UIDriver::render() should only be called when there is an update in the UI or screen resize
		m_uiDriver->render();
	}

	void MultipleThemeModelTest::update(SGE::Driver& driver, float deltaTime)
	{
	}
}
