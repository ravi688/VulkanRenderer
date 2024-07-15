#include <sutk/tests/BitmapTextTest.hpp>

#include <sge-cpp/Material.hpp>
#include <sge-cpp/Camera.hpp>

#include <hpml/affine_transformation.h>
#include <hpml/mat4.h>

#include <sge/conio.h>

namespace SUTK
{
	DriverInitializationData BitmapTextTest::getInitializationData()
	{
		auto data = ITest::getInitializationData();
		data.title = "Bitmap Text Test";
		return data;
	}

	void BitmapTextTest::initialize(SGE::Driver& driver)
	{
		SGE::CameraSystem cameraSystem = driver.getCameraSystem();
		SGE::ShaderLibrary shaderLibrary = driver.getShaderLibrary();
		SGE::MaterialLibrary materialLibrary = driver.getMaterialLibrary();

		// create camera 
		SGE::Camera camera = cameraSystem.createCamera(SGE::Camera::ProjectionType::Perspective);
		camera.setClear(COLOR_BLACK, 1.0f);
		camera.setActive(true);
		camera.setTransform(mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

		// create scene 
		m_scene = driver.createRenderScene(BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
		// add the camera to the scene 
		m_scene.addCamera(camera);

		SGE::Shader shader = shaderLibrary.loadShader("../shaders/builtins/bitmap_text_shader.sb");
		SGE::Material material = materialLibrary.createMaterial(shader, "BitmapTextShaderTest");
		SGE::Material material2 = materialLibrary.createMaterial(shader, "BitmapTextShaderTest");

		m_font = driver.loadFont("../showcase/resource/fonts/arial.ttf");
		m_font.setCharSize(24);

		// bitmap text 
		SGE::BitmapGlyphAtlasTexture::CreateInfo textureCreateInfo = { 1024, 1024, m_font };
		m_texture = driver.createBitmapGlyphAtlasTexture(textureCreateInfo);

		material.set<float>("parameters.color.r", 1.0f);
		material.set<float>("parameters.color.g", 1.0f);
		material.set<float>("parameters.color.b", 1.0f);

		material2.set<float>("parameters.color.r", 1.0f);
		material2.set<float>("parameters.color.g", 1.0f);
		material2.set<float>("parameters.color.b", 1.0f);

		m_text = driver.createBitmapText(m_texture);
		m_text2 = driver.createBitmapText(m_texture);
		m_textString = m_text.createString();
		m_anotherString = m_text.createString();

		m_anotherString.setPointSize(10);
		m_textString.setPointSize(10);
		m_anotherString.set("Hardwork with dedication suffices to c");
		m_textString.set("Hardwork with dedication suffices to c: 1234324");

		m_textObject2 = m_scene.createObject(SGE::RenderObject::Type::Text, SGE::RenderQueue::Type::Geometry);
		m_textObject2.setMaterial(material2);
		m_textObject2.attach(m_text2);

		m_textObject = m_scene.createObject(SGE::RenderObject::Type::Text, SGE::RenderQueue::Type::Geometry);
		m_textObject.setMaterial(material);
		m_textObject.attach(m_text);

		m_textString.setTransform(mat4_translation(0.0f, 400.0f, -400.0f));

		m_scene.buildQueues();
	}

	void BitmapTextTest::terminate(SGE::Driver& driver)
	{
		m_text.destroy();
		m_text2.destroy();
		m_texture.destroy();
		m_font.destroy();
		m_scene.destroy();
	}

	void BitmapTextTest::render(SGE::Driver& driver)
	{
		m_scene.render(RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
	}

	void BitmapTextTest::update(SGE::Driver& driver, float deltaTime)
	{
		static bool isScreenSpace = true;
		if(kbhit())
		{
			char ch = getch();
			if((ch != 'p') && (ch != 'q') && (ch != 'd') && (ch != 'u') && (ch != 'n'))
			{
				isScreenSpace = !isScreenSpace;
				if(isScreenSpace)
				{
					m_text.setRenderSpaceType(SGE::BitmapText::RenderSpaceType::Plain);
					debug_log_info("BITMAP_TEXT_RENDER_SPACE_TYPE_2D");
					m_textObject.setTransform(mat4_identity());
					m_textString.setTransform(mat4_translation(0.0f, 400.0f, -400.0f));
				}
				else
				{
					m_text.setRenderSpaceType(SGE::BitmapText::RenderSpaceType::Volume);
					debug_log_info("BITMAP_TEXT_RENDER_SPACE_TYPE_3D");
					m_textObject.setTransform(mat4_mul(3, mat4_rotation_y(45 DEG), mat4_scale(0.005f, 0.005f, 0.005f), mat4_translation(0.0f, 0.0f, -0.5f)));
					m_textString.setTransform(mat4_translation(0.0f, 30.0f, 0.0f));
				}
			}
			else if(ch == 'p')
			{
				m_anotherString.setPointSize(m_anotherString.getPointSize() * 2);
				m_textString.setPointSize(m_textString.getPointSize() * 2);
			}
			else if(ch == 'q')
			{
				m_anotherString.setPointSize(std::min(1u, m_anotherString.getPointSize() / 2));
				m_textString.setPointSize(std::min(1u, m_textString.getPointSize() / 2));
			}
			else if(ch == 'n')
			{
				static float yCoord = 300.0f;
				auto handle = m_text2.createString();
				handle.setPointSize(24);
				char buffer[128] = { };
				sprintf(buffer, "${@&ABCD:%.2f}", yCoord);
				handle.set(buffer);
				handle.setTransform(mat4_translation(0.0f, yCoord, -400.0f));
				yCoord -= 100.0f;
				m_texture.dump("BitmapTextTest.bmp");
			}
		}

		static int counter = 0;
		counter++;
		if(counter == 66000)
		counter = 0;
		char buffer[128] =  { };
		sprintf(buffer, "%d", counter);
		m_textString.set(buffer);
		sprintf(buffer, "%d", counter);
		m_anotherString.set(buffer);

		m_texture.commit(NULL);
	}
}
