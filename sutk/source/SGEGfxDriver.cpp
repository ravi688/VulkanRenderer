#include <sutk/SGEGfxDriver.hpp>

#include <utility> /* for std::pair */

#include <hpml/affine_transformation.h>

#define BITMAP_TEXT_OVERLOAD_THRESHOLD 800 // 800 number of characters rewrite can be assumed to have near to zero latency

namespace SUTK
{	
	SGEGfxDriver::SGEGfxDriver(SGE::Driver& driver) : m_driver(driver), m_id_generator(id_generator_create(0, NULL)), m_currentBitmapTextHandle(GFX_DRIVER_OBJECT_NULL_HANDLE)
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
			pair.second.text.destroy();
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

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::createBitmapText()
	{
		SGE::BitmapText text = m_driver.createBitmapText(m_bgaTexture);
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Text, SGE::RenderQueue::Type::Geometry);
		object.setMaterial(m_material);
		object.attach(text);
		// rebuild render pass graph as new objects have been added into the render scene 
		m_scene.buildQueues();
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextMappings.insert({ id, { text, object, 0u } });
		return { text, static_cast<GfxDriverObjectHandleType>(id) };
	}

	std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextData>::iterator
	SGEGfxDriver::getBitmapTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextMappings.find(handle);
		_assert(it != m_bitmapTextMappings.end());
		return it;
	}

	void SGEGfxDriver::destroyBitmapText(GfxDriverObjectHandleType handle)
	{
		auto it = getBitmapTextIterator(handle);
		SGEBitmapTextData bitmapTextData = it->second;
		bitmapTextData.text.destroy();
		// TODO: pair.second.destroy();
		id_generator_return(&m_id_generator, static_cast<u32>(handle));
		m_bitmapTextMappings.erase(it);
	}

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::getOrCreateBitmapText()
	{
		if(m_bitmapTextMappings.size() == 0)
		{
			auto pair = createBitmapText();
			m_currentBitmapTextHandle = pair.second;
			return pair;
		}
		SGEBitmapTextData& textData = getBitmapTextIterator(m_currentBitmapTextHandle)->second;
		if(textData.charCount > BITMAP_TEXT_OVERLOAD_THRESHOLD)
		{
			auto pair = createBitmapText();
			m_currentBitmapTextHandle = pair.second;
			return pair;
		}
		return { textData.text, m_currentBitmapTextHandle };
	}

	GfxDriverObjectHandleType SGEGfxDriver::createText()
	{
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> bitmapText = getOrCreateBitmapText();
		SGE::BitmapTextString subText = bitmapText.first.createString();
		subText.setPointSize(24);
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextStringMappings.insert({ id, { subText,  bitmapText.second } });
		return static_cast<GfxDriverObjectHandleType>(id);
	}

	std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData>::iterator
	SGEGfxDriver::getSubTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextStringMappings.find(handle);
		_assert(it != m_bitmapTextStringMappings.end());
		return it;
	}

	SGE::BitmapTextString SGEGfxDriver::getText(GfxDriverObjectHandleType handle)
	{
		auto it = getSubTextIterator(handle);
		return it->second.textString;
	}

	void SGEGfxDriver::destroyText(GfxDriverObjectHandleType handle)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString subText = it->second.textString;
		subText.destroy();
		id_generator_return(&m_id_generator, static_cast<u32>(handle));
		m_bitmapTextStringMappings.erase(it);
	}

	void SGEGfxDriver::setTextPosition(GfxDriverObjectHandleType handle, Vec2D<DisplaySizeType> position)
	{
		getText(handle).setPosition(vec3(0.0f, position.y, position.x));
	}

	void SGEGfxDriver::setTextData(GfxDriverObjectHandleType handle, const std::string& data)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString& textString = it->second.textString;
		SGEBitmapTextData& bitmapTextData = getBitmapTextIterator(it->second.textHandle)->second;
		_assert(bitmapTextData.charCount >= textString.getLength());
		bitmapTextData.charCount -= textString.getLength();
		textString.set(data);
		bitmapTextData.charCount += textString.getLength();
		m_bgaTexture.commit(NULL);
	}
}