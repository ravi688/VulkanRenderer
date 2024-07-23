#include <sutk/SGEGfxDriver.hpp>
#include <sge-cpp/Display.hpp>

#include <utility> /* for std::pair */

#include <hpml/affine_transformation.h>

#define BITMAP_TEXT_OVERLOAD_THRESHOLD 500 // 800 number of characters rewrite can be assumed to have near to zero latency

namespace SUTK
{	
	static Vec2D<DisplaySizeType> getVec2DFromStdPair(std::pair<u32, u32> pair)
	{
		return {static_cast<DisplaySizeType>(pair.first), static_cast<DisplaySizeType>(pair.second)};
	}
	SGEGfxDriver::SGEGfxDriver(SGE::Driver& driver, bool autoCmdRecordAndExecute) : m_driver(driver), 
																					m_autoCmdRecordAndExecute(autoCmdRecordAndExecute),
																					m_id_generator(id_generator_create(0, NULL)), 
																					m_currentBitmapTextHandle(GFX_DRIVER_OBJECT_NULL_HANDLE)
	{
		driver.getRenderWindow().getOnResizeEvent().subscribe(
					[](void* publisher, void* handlerData)
					{
						std::pair<u32, u32> pair = SGE::Event::ReinterpretPublisher<SGE::RenderWindow>(publisher).getSize();
						auto& size = *reinterpret_cast<Vec2D<DisplaySizeType>*>(handlerData);
						size = getVec2DFromStdPair(pair);
					}, &m_size);
		m_size = getVec2DFromStdPair(driver.getRenderWindow().getSize());

		SGE::CameraSystem cameraSystem = driver.getCameraSystem();
		SGE::ShaderLibrary shaderLibrary = driver.getShaderLibrary();

		// create camera 
		SGE::Camera camera = cameraSystem.createCamera(SGE::Camera::ProjectionType::Perspective);
		camera.setClear(COLOR_BLACK, 1.0f);
		camera.setActive(true);
		camera.setTransform(mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

		// create scene 
		m_scene = driver.createRenderScene(BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
		// add the camera to the scene 
		m_scene.addCamera(camera);

		m_shader = shaderLibrary.compileAndLoadShader(driver.getBuiltinFileData("/bitmap_text_shader.v3dshader").first);
		auto font_data = driver.getBuiltinFileData("/Calibri Regular.ttf");
		m_font = driver.createFont(font_data.first.data(), font_data.second);

		m_font.setCharSize(15);

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

	void SGEGfxDriver::render(UIDriver& driver)
	{
		if(m_autoCmdRecordAndExecute)
			// begin command buffer recording
			m_driver.beginFrame();
		m_bgaTexture.commit(NULL);
		m_scene.render(RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);
		if(m_autoCmdRecordAndExecute)
		{
			// end command buffer recording
			m_driver.endFrame();
			// dispatch the command buffers for execution on GPU
			m_driver.update();
		}
	}

	#define GET_RENDER_OBJECT_ID(gfxDriverObjectHandle) get_render_object_id(gfxDriverObjectHandle)
	static u32 get_render_object_id(GfxDriverObjectHandleType handle)
	{
		u32 id = BIT64_UNPACK32(handle, 0);
		_assert(id != U32_MAX);
		return id;
	}
	#define GET_ATTACHED_OBJECT_ID(gfxDriverObjectHandle) BIT64_UNPACK32(gfxDriverObjectHandle, 1)

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::createBitmapText()
	{
		debug_log_info("[SGE] Creating new SGE::BitmapText object");
		SGE::BitmapText text = m_driver.createBitmapText(m_bgaTexture);
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Text, SGE::RenderQueue::Type::Geometry);
		SGE::Material material = m_driver.getMaterialLibrary().createMaterial(m_shader, "BitmapTextShaderTest");
		material.set<float>("parameters.color.r", 1.0f);
		material.set<float>("parameters.color.g", 1.0f);
		material.set<float>("parameters.color.b", 1.0f);
		object.setMaterial(material);
		object.attach(text);
		// rebuild render pass graph as new objects have been added into the render scene 
		m_scene.buildQueues();
		
		// add bitmap text into the bitmap text mappings table
		id_generator_id_type_t bitmapTextID = id_generator_get(&m_id_generator);
		m_bitmapTextMappings.insert({ bitmapTextID, { text, 0u } });

		// add corresponding render object into the render object mappings table
		id_generator_id_type_t renderObjectID = id_generator_get(&m_id_generator);
		m_renderObjectMappings.insert({ renderObjectID, object });

		static_assert(sizeof(GfxDriverObjectHandleType) == sizeof(u64));

		return { text, static_cast<GfxDriverObjectHandleType>(BIT64_PACK32(bitmapTextID, renderObjectID)) };
	}

	std::unordered_map<id_generator_id_type_t, SGEBitmapTextData>::iterator
	SGEGfxDriver::getBitmapTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextMappings.find(GET_ATTACHED_OBJECT_ID(handle));
		_assert(it != m_bitmapTextMappings.end());
		return it;
	}

	std::unordered_map<id_generator_id_type_t, SGE::RenderObject>::iterator
	SGEGfxDriver::getRenderObjectIterator(GfxDriverObjectHandleType handle)
	{
		auto it = m_renderObjectMappings.find(GET_RENDER_OBJECT_ID(handle));
		_assert(it != m_renderObjectMappings.end());
		return it;
	}

	void SGEGfxDriver::destroyBitmapText(GfxDriverObjectHandleType handle)
	{
		// erase the bitmap text object
		auto it = getBitmapTextIterator(handle);
		SGEBitmapTextData bitmapTextData = it->second;
		bitmapTextData.text.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_ATTACHED_OBJECT_ID(handle)));
		m_bitmapTextMappings.erase(it);

		// erase the corresponding render object also 
		auto it2 = getRenderObjectIterator(handle);
		// SGE::RenderObject object = *it2;
		// TODO: object.destroy()
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_RENDER_OBJECT_ID(handle)));
		m_renderObjectMappings.erase(it2);
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
		subText.setPointSize(15);
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextStringMappings.insert({ id, { subText,  bitmapText.second } });
		return static_cast<GfxDriverObjectHandleType>(BIT64_PACK32(id, U32_MAX));
	}

	std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData>::iterator
	SGEGfxDriver::getSubTextIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_bitmapTextStringMappings.find(GET_ATTACHED_OBJECT_ID(handle));
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
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(handle));
		m_bitmapTextStringMappings.erase(it);
	}

	void SGEGfxDriver::setTextPosition(GfxDriverObjectHandleType handle, Vec2D<DisplaySizeType> position)
	{
		auto windowSize = getSize();
		getText(handle).setPosition(vec3(0.0f, static_cast<float>(windowSize.height >> 1) - static_cast<float>(position.y), 
											   static_cast<float>(position.x) - static_cast<float>(windowSize.width >> 1)));
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
	}

	GfxDriverObjectHandleType SGEGfxDriver::getTextObject(GfxDriverObjectHandleType handle)
	{
		auto it = getSubTextIterator(handle);
		return it->second.textHandle;
	}

	void SGEGfxDriver::setObjectScissor(GfxDriverObjectHandleType handle, const Rect2D<DisplaySizeType> rect)
	{
		auto it = getRenderObjectIterator(handle);
		it->second.setScissor(irect2d(ioffset2d(rect.x, rect.y), iextent2d(rect.width, rect.height)));
	}

	u32 SGEGfxDriver::getBaselineHeightInPixels()
	{
		return m_font.getFontUnitsToPixels(m_font.getBaselineSpace(), SGE::Display::GetDPI().height);
	}

	u32 SGEGfxDriver::addOnResizeHandler(OnResizeCallbackHandler handler)
	{
		u32 id = id_generator_get(&m_id_generator);
		m_onResizeHandlers.insert({ id, { handler, EVENT_SUBSCRIPTION_HANDLE_INVALID } });
		auto& onResizeHandlerData = m_onResizeHandlers[id];
		SGE::Event::SubscriptionHandle handle = m_driver.getRenderWindow().getOnResizeEvent().subscribe(
			[](void* publisher, void* handlerData)
			{
				std::pair<u32, u32> pair = SGE::Event::ReinterpretPublisher<SGE::RenderWindow>(publisher).getSize();
				auto size = getVec2DFromStdPair(pair);
				auto& _handlerData = *reinterpret_cast<OnResizeCallbackHandlerData*>(handlerData);
				_handlerData.handler(size);
			}, &onResizeHandlerData);
		onResizeHandlerData.handle = handle;
		return id;
	}
	void SGEGfxDriver::removeOnResizeHandler(u32 id)
	{
		auto it = m_onResizeHandlers.find(id);
		if(it == m_onResizeHandlers.end())
			return;
		m_driver.getRenderWindow().getOnResizeEvent().unsubscribe(it->second.handle);
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(id));
		m_onResizeHandlers.erase(it);
	}
}
