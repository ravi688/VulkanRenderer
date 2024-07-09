#include <sutk/SGEGfxDriver.hpp>

#include <utility> /* for std::pair */

#include <hpml/affine_transformation.h>

namespace SUTK
{	
	SGEGfxDriver::SGEGfxDriver(SGE::Driver& driver) : m_driver(driver), m_id_generator(id_generator_create(0, NULL))
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
		m_material = materialLibrary.createMaterial(shader, "BitmapTextShaderTest");
		m_material.set<float>("parameters.color.r", 1.0f);
		m_material.set<float>("parameters.color.g", 1.0f);
		m_material.set<float>("parameters.color.b", 1.0f);

		m_font = driver.loadFont("../showcase/resource/fonts/arial.ttf");
		m_font.setCharSize(24);

		SGE::BitmapGlyphAtlasTexture::CreateInfo createInfo =
		{
			512,
			512,
			m_font
		};
		m_bgaTexture = m_driver.createBitmapGlyphAtlasTexture(createInfo);

		m_scene.buildQueues();
	}
	
	SGEGfxDriver::~SGEGfxDriver()
	{
		// destroy SGE::BitmapText objects
		for(auto pair : m_bitmapTextMappings)
			pair.second.first.destroy();
		m_bgaTexture.destroy();
		m_font.destroy();
		m_scene.destroy();
		id_generator_destroy(&m_id_generator);
	}

	Vec2D<DisplaySizeType> SGEGfxDriver::getSize()
	{
		std::pair<u32, u32> size = m_driver.getRenderWindow().getSize();
		return { static_cast<DisplaySizeType>(size.first), static_cast<DisplaySizeType>(size.second) };
	}
	
	void SGEGfxDriver::render(UIDriver& driver)
	{
		m_scene.render(RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
	}

	GfxDriverObjectHandleType SGEGfxDriver::createText()
	{
		SGE::BitmapText text = m_driver.createBitmapText(m_bgaTexture);
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Text, SGE::RenderQueue::Type::Geometry);
		object.setMaterial(m_material);
		object.attach(text);
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextMappings.insert({ id, { text, object } });
		return static_cast<GfxDriverObjectHandleType>(id);
	}

	std::unordered_map<GfxDriverObjectHandleType, std::pair<SGE::BitmapText, SGE::RenderObject>>::iterator
	SGEGfxDriver::getBitmapTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextMappings.find(handle);
		_assert(it != m_bitmapTextMappings.end());
		return it;
	}

	void SGEGfxDriver::destroyText(GfxDriverObjectHandleType handle)
	{
		auto it = getBitmapTextIterator(handle);
		std::pair<SGE::BitmapText, SGE::RenderObject> pair = it->second;
		pair.first.destroy();
		// TODO: pair.second.destroy();
		id_generator_return(&m_id_generator, static_cast<u32>(handle));
		m_bitmapTextMappings.erase(it);
	}

	GfxDriverObjectHandleType SGEGfxDriver::createSubtext(GfxDriverObjectHandleType textHandle)
	{
		SGE::BitmapText bitmapText = getBitmapTextIterator(textHandle)->second.first;
		SGE::BitmapTextString subText = bitmapText.createString();
		subText.setPointSize(24);
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextStringMappings.insert({ id, subText });
		return static_cast<GfxDriverObjectHandleType>(id);
	}

	std::unordered_map<GfxDriverObjectHandleType, SGE::BitmapTextString>::iterator
	SGEGfxDriver::getSubTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextStringMappings.find(handle);
		_assert(it != m_bitmapTextStringMappings.end());
		return it;
	}

	void SGEGfxDriver::destroySubText(GfxDriverObjectHandleType subTextHandle)
	{
		auto it = getSubTextIterator(subTextHandle);
		SGE::BitmapTextString subText = it->second;
		subText.destroy();
		id_generator_return(&m_id_generator, static_cast<u32>(subTextHandle));
		m_bitmapTextStringMappings.erase(it);
	}
}
