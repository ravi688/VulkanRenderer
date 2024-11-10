#include <sutk/SGEGfxDriver.hpp>
#include <sutk/Geometry.hpp> // for SGE::Geometry
#include <sge-cpp/Display.hpp>
#include <common/TimeTaken.hpp> // for com::TimeTaken

#include <utility> /* for std::pair */
#include <sstream> // for std::ostringstream

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
																					m_id_generator(id_generator_create(0, NULL))
	{
		driver.getRenderWindow().getOnResizeEvent().subscribe(
					[](void* publisher, void* handlerData)
					{
						std::pair<u32, u32> pair = SGE::Event::ReinterpretPublisher<SGE::RenderWindow>(publisher).getSize();
						auto& gfxDriver = *reinterpret_cast<SGEGfxDriver*>(handlerData);
						gfxDriver.m_size = getVec2DFromStdPair(pair);
						extent2d_t inches = SGE::Display::ConvertPixelsToInches({ static_cast<f32>(pair.first), static_cast<f32>(pair.second) });
						gfxDriver.m_sizeCentimeters = { inches.width * CENTIMETERS_PER_INCH, inches.height * CENTIMETERS_PER_INCH };
					}, reinterpret_cast<void*>(this));
		m_size = getVec2DFromStdPair(driver.getRenderWindow().getSize());
		extent2d_t inches = SGE::Display::ConvertPixelsToInches({ static_cast<f32>(m_size.width), static_cast<f32>(m_size.height) });
		m_sizeCentimeters = { inches.width * CENTIMETERS_PER_INCH, inches.height * CENTIMETERS_PER_INCH };

		SGE::CameraSystem cameraSystem = driver.getCameraSystem();
		SGE::ShaderLibrary shaderLibrary = driver.getShaderLibrary();

		// create camera 
		SGE::Camera camera = cameraSystem.createCamera(SGE::Camera::ProjectionType::Perspective);
		camera.setClear(COLOR_BLACK, 1.0f);
		camera.setActive(true);
		camera.setTransform(mat4_mul(2, mat4_translation(-1.8f, 0.6f, 0), mat4_rotation(0, 0, -22 * DEG2RAD)));

		// create scene 
		m_scene = driver.createRenderScene(RENDER_QUEUE_TYPE_GEOMETRY_BIT | RENDER_QUEUE_TYPE_TRANSPARENT_BIT);
		// add the camera to the scene 
		m_scene.addCamera(camera);

		std::string_view source = driver.getBuiltinFileData("/bitmap_text_shader.v3dshader").first;
		double elapsedTime;
		SGE::Shader shader;
		{
			com::TimeTaken<double> _(elapsedTime);
			m_shader = shaderLibrary.compileAndLoadShader(source, "BitmapShader", SHADER_CACHE_WRITE_BIT | SHADER_CACHE_LOOKUP_BIT, std::hash<std::string_view> { } (source));
		}
		std::cout << "Time taken to bitmap Shader: " << elapsedTime << " milliseconds" << std::endl;

		SGE::BitmapGlyphAtlasTexture::CreateInfo createInfo =
		{
			512,
			512
		};
		m_bgaTexture = m_driver.createBitmapGlyphAtlasTexture(createInfo);
	}
	
	SGEGfxDriver::~SGEGfxDriver()
	{
		// destroy SGE::BitmapText objects
		for(auto it = m_bitmapTextGroups.begin(); it != m_bitmapTextGroups.end(); it++)
			destroyTextGroup(it, false);
		m_bitmapTextGroups.clear();
		m_bgaTexture.destroy();
		if(m_defaultFont)
			m_defaultFont.destroy();
		m_scene.destroy();
		id_generator_destroy(&m_id_generator);
	}

	u32 SGEGfxDriver::getNativeWindowHandleSize()
	{
		return m_driver.getRenderWindow().getNativeSize();
	}

	void SGEGfxDriver::getNativeWindowHandle(void* const out)
	{
		m_driver.getRenderWindow().getNative(out);
	}

	void SGEGfxDriver::render(UIDriver& driver)
	{
		if(m_autoCmdRecordAndExecute)
			// begin command buffer recording
			m_driver.beginFrame();

		// update device side memory of the BGA texture
		m_bgaTexture.commit(NULL);

		// record commands
		m_scene.render(RENDER_SCENE_ALL_QUEUES, RENDER_SCENE_CLEAR);

		if(m_autoCmdRecordAndExecute)
		{
			// end command buffer recording
			m_driver.endFrame();
			// dispatch the command buffers for execution on GPU
			m_driver.dispatchFrame();
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

	static constexpr SGE::RenderQueue::Type getRenderQueueTypeFromRenderMode(RenderMode renderMode) noexcept
	{
		switch(renderMode)
		{
			case RenderMode::Opaque:
				return SGE::RenderQueue::Type::Geometry;
			case RenderMode::Transparent:
				return SGE::RenderQueue::Type::Transparent;
		};
		return SGE::RenderQueue::Type::Geometry;
	}

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::createBitmapText(SGEBitmapTextGroup& group)
	{
		debug_log_info("[SGE] Creating new SGE::BitmapText object");
		SGE::BitmapText text = m_driver.createBitmapText(m_bgaTexture, group.font);
		SGE::RenderQueue::Type queueType = getRenderQueueTypeFromRenderMode(group.renderMode);
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Text, queueType);
		SGE::Material material = m_driver.getMaterialLibrary().createMaterial(m_shader, "BitmapTextShaderTest");
		material.set<float>("parameters.color.r", 1.0f);
		material.set<float>("parameters.color.g", 1.0f);
		material.set<float>("parameters.color.b", 1.0f);
		// TEXT_RENDER_SURFACE_TYPE_SCREEN
		material.set<s32>("parameters.surface_type", 1);
		// TEXT_RENDER_SPACE_TYPE_2D
		material.set<s32>("parameters.space_type", 0);
		object.setMaterial(material);
		object.attach(text);
		auto pos = object.getPosition();
		pos.x = group.depth;
		object.setPosition(pos);
		
		// add bitmap text into the bitmap text mappings table
		id_generator_id_type_t bitmapTextID = id_generator_get(&m_id_generator);
		group.bitmapTextTable.insert({ bitmapTextID, { text, object, 0u } });

		// add corresponding render object into the render object mappings table
		id_generator_id_type_t renderObjectID = id_generator_get(&m_id_generator);
		m_renderObjectMappings.insert({ renderObjectID, object });

		static_assert(sizeof(GfxDriverObjectHandleType) == sizeof(u64));

		return { text, static_cast<GfxDriverObjectHandleType>(BIT64_PACK32(bitmapTextID, renderObjectID)) };
	}

	std::unordered_map<id_generator_id_type_t, SGEGfxDriver::SGEBitmapTextGroup>::iterator
	SGEGfxDriver::getBitmapTextGroupIterator(GfxDriverObjectHandleType groupHandle)
	{
		auto it = m_bitmapTextGroups.find(static_cast<id_generator_id_type_t>(groupHandle));
		_com_assert(it != m_bitmapTextGroups.end());
		return it;
	}

	std::unordered_map<id_generator_id_type_t, SGEBitmapTextData>::iterator
	SGEGfxDriver::getBitmapTextIterator(GfxDriverObjectHandleType groupHandle, GfxDriverObjectHandleType handle)
	{	
		SGEBitmapTextGroup& group = getBitmapTextGroupIterator(groupHandle)->second;
		auto it = group.bitmapTextTable.find(GET_ATTACHED_OBJECT_ID(handle));
		_assert(it != group.bitmapTextTable.end());
		return it;
	}

	std::unordered_map<id_generator_id_type_t, SGE::RenderObject>::iterator
	SGEGfxDriver::getRenderObjectIterator(GfxDriverObjectHandleType handle)
	{
		auto it = m_renderObjectMappings.find(GET_RENDER_OBJECT_ID(handle));
		_assert(it != m_renderObjectMappings.end());
		return it;
	}

	void SGEGfxDriver::destroyBitmapText(GfxDriverObjectHandleType groupHandle, GfxDriverObjectHandleType handle)
	{
		// erase the bitmap text object
		auto it = getBitmapTextIterator(groupHandle, handle);
		SGEBitmapTextData bitmapTextData = it->second;
		bitmapTextData.text.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_ATTACHED_OBJECT_ID(handle)));
		SGEBitmapTextGroup& group = getBitmapTextGroupIterator(groupHandle)->second;
		group.bitmapTextTable.erase(it);

		// erase the corresponding render object also 
		auto it2 = getRenderObjectIterator(handle);
		// SGE::RenderObject object = *it2;
		// TODO: object.destroy()
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_RENDER_OBJECT_ID(handle)));
		m_renderObjectMappings.erase(it2);
	}

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::getOrCreateBitmapText(GfxDriverObjectHandleType textGroup)
	{
		SGEBitmapTextGroup& group = getBitmapTextGroupIterator(textGroup)->second;
		if(group.bitmapTextTable.size() == 0)
		{
			auto pair = createBitmapText(group);
			group.currentBitmapTextHandle = pair.second;
			return pair;
		}
		SGEBitmapTextData& textData = getBitmapTextIterator(textGroup, group.currentBitmapTextHandle)->second;
		if(textData.charCount > BITMAP_TEXT_OVERLOAD_THRESHOLD)
		{
			auto pair = createBitmapText(group);
			group.currentBitmapTextHandle = pair.second;
			return pair;
		}
		return { textData.text, group.currentBitmapTextHandle };
	}

	GfxDriverObjectHandleType SGEGfxDriver::createTextGroup(RenderMode renderMode)
	{
		id_generator_id_type_t id = id_generator_get(&m_id_generator);
		SGEBitmapTextGroup& group = m_bitmapTextGroups[id];
		group.font = getDefaultFont();
		group.currentBitmapTextHandle = GFX_DRIVER_OBJECT_NULL_HANDLE;
		group.renderMode = renderMode;
		group.depth = 0.0f;
		return static_cast<GfxDriverObjectHandleType>(id);
	}

	void SGEGfxDriver::setTextGroupFont(GfxDriverObjectHandleType textGroup, GfxDriverObjectHandleType font)
	{
		SGEBitmapTextGroup& group = getTextGroup(textGroup);
		auto& map = group.bitmapTextTable;
		// Try to make fewer calls to getFont() as possible
		// because it may also lazily load the fonts from disk.
		SGE::Font sgeFont = getFont(font);
		for(auto& pair : map)
		{
			SGEBitmapTextData& data = pair.second;
			data.text.setFontUpdateAll(sgeFont);
		}
		group.font = sgeFont;
	}

	void SGEGfxDriver::setTextGroupDepth(GfxDriverObjectHandleType textGroup, f32 depth)
	{
		SGEBitmapTextGroup& group = getTextGroup(textGroup);
		auto& map = group.bitmapTextTable;
		for(auto& pair : map)
		{
			SGEBitmapTextData& data = pair.second;
			SGE::RenderObject object = data.object;
			vec3_t pos = object.getPosition();
			pos.x = depth;
			object.setPosition(pos);
		}
		group.depth = depth;
	}

	SGEGfxDriver::SGEBitmapTextGroup& SGEGfxDriver::getTextGroup(GfxDriverObjectHandleType handle) noexcept
	{
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		return com::find_value(m_bitmapTextGroups, static_cast<id_generator_id_type_t>(handle));
	}

	void SGEGfxDriver::destroyTextGroup(std::unordered_map<id_generator_id_type_t, SGEBitmapTextGroup>::iterator it, bool isErase)
	{
		for(auto pair : it->second.bitmapTextTable)
			pair.second.text.destroy();
		it->second.bitmapTextTable.clear();
		if(isErase)
			m_bitmapTextGroups.erase(it);
	}

	void SGEGfxDriver::destroyTextGroup(GfxDriverObjectHandleType group)
	{
		auto it = getBitmapTextGroupIterator(group);
		destroyTextGroup(it);
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(group));
	}

	GfxDriverObjectHandleType SGEGfxDriver::createText(GfxDriverObjectHandleType textGroup)
	{
		// TODO: This should be ... = getOrCreateUsableBitmapText or getOrCreateLeastLoadedBitmapText
		// WARNING: removing destroying bitmap text and trying to createText might lead to segfaults or unexpected code execution.
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> bitmapText = getOrCreateBitmapText(textGroup);
		SGE::BitmapTextString subText = bitmapText.first.createString();
		subText.setPointSize(12);
		u32 id = id_generator_get(&m_id_generator);
		m_bitmapTextStringMappings.insert({ id, { subText,  bitmapText.second, textGroup } });
		m_typeTable.insert({ id, ObjectType::Text });
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

	void SGEGfxDriver::removeIDFromTypeTable(u32 id)
	{
		auto it = m_typeTable.find(id);
		_assert(it != m_typeTable.end());
		m_typeTable.erase(it);
	}

	void SGEGfxDriver::destroyText(GfxDriverObjectHandleType handle)
	{
		_com_assert(handle != GFX_DRIVER_OBJECT_NULL_HANDLE);
		
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString subText = it->second.textString;
		subText.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_ATTACHED_OBJECT_ID(handle)));
		m_bitmapTextStringMappings.erase(it);
		removeIDFromTypeTable(GET_ATTACHED_OBJECT_ID(handle));
	}

	vec3_t SGEGfxDriver::SUTKToSGECoordTransform(const Vec2Df position)
	{
		// SUTK's coordinate system is in centimeters, while SGE's coordinate system is in pixels
		extent2d_t pixelPosition = SGE::Display::ConvertInchesToPixels({ position.x * INCHES_PER_CENTIMETER, position.y * INCHES_PER_CENTIMETER });
		auto windowSize = getSizeInPixels();
		return vec3(0.0f, windowSize.height * 0.5f - pixelPosition.height, pixelPosition.width - windowSize.width * 0.5f);
	}

	Vec2Df SGEGfxDriver::SGEToSUTKCoordTransform(const vec3_t position)
	{
		auto windowSize = getSizeInPixels();
		Vec2Df sutkPosInPixels = { windowSize.height * 0.5f - position.y, windowSize.width * 0.5f + position.x };
		extent2d_t sutkPosInInches = SGE::Display::ConvertPixelsToInches({ sutkPosInPixels.x, sutkPosInPixels.y });
		return { sutkPosInInches.x * CENTIMETERS_PER_INCH, sutkPosInInches.y * CENTIMETERS_PER_INCH };
	}

	#define MIN_DEPTH -0.001f
	#define MAX_DEPTH 99.0f

	void SGEGfxDriver::setTextPosition(GfxDriverObjectHandleType handle, Vec3Df position)
	{
		vec3_t pos = SUTKToSGECoordTransform(position.xy);
		pos.x = position.z;
		getText(handle).setPosition(pos);
	}

	void SGEGfxDriver::setTextDepth(GfxDriverObjectHandleType handle, f32 depth)
	{
		// Since all text strings (SGE::BitmapTextString) are derived from the same SGE::BitmapText object
		// And that SGE::BitmapTextObject is attached to a single SGE::RenderObject,
		// The ordering of drawing is only decided by the depth of SGE::RenderObject object.
		// Also, for this reason, we can't individually assign depth values to each text string object to define draw order in them.
		// Therefore, we need to consider all the text strings in the plane of the SGE::RenderObject.
		// And assign the last set depth (of any text string derived from the same SGE::BitmapText object) to this SGE::RenderObject's depth.

		auto it = getSubTextIterator(handle);
		SGEBitmapTextStringData& data = it->second;
		auto it2 = getRenderObjectIterator(data.textHandle);
		SGE::RenderObject object = it2->second;
		vec3_t pos = object.getPosition();
		pos.x = depth;
		object.setPosition(pos);
	}

	void SGEGfxDriver::setTextPointSize(GfxDriverObjectHandleType handle, f32 pointSize)
	{
		getText(handle).setPointSize(pointSize);
	}

	f32 SGEGfxDriver::getTextPointSize(GfxDriverObjectHandleType handle)
	{
		return getText(handle).getPointSize();
	}

	SGE::Font SGEGfxDriver::getDefaultFont() noexcept
	{
		if(!m_defaultFont)
		{
			auto font_data = m_driver.getBuiltinFileData("/Roboto-Bold.ttf");
			m_defaultFont = m_driver.createFont(font_data.first.data(), font_data.second);
			m_defaultFont.setCharSize(12);
		}
		return m_defaultFont;
	}

	SGE::Font SGEGfxDriver::getFont(GfxDriverObjectHandleType handle) noexcept
	{
		if(handle == GFX_DRIVER_OBJECT_NULL_HANDLE)
			return getDefaultFont();
		std::pair<std::string, SGE::Font>& pair = m_fontData.get(handle);
		// TODO: Wait on the another thread to finish loading this font
		if(!pair.second)
			pair.second = m_driver.loadFont(pair.first);
		return pair.second;
	}

	void SGEGfxDriver::setTextFont(GfxDriverObjectHandleType handle, GfxDriverObjectHandleType font)
	{
		SGE::Font sgeFont = getFont(font);
		getText(handle).setFont(sgeFont);
	}

	static INLINE_IF_RELEASE_MODE color_t to_color(const Color4 _color)
	{
		return color(_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f);
	}

	static INLINE_IF_RELEASE_MODE vec4_t to_vec4(const Color4 _color)
	{
		return vec4(_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f);
	}

	void SGEGfxDriver::setTextColor(GfxDriverObjectHandleType handle, const Color4 color)
	{
		getText(handle).setColor(to_color(color));
	}

	void SGEGfxDriver::setTextColorRanges(GfxDriverObjectHandleType handle, const ColorRange* ranges, u32 rangeCount)
	{
		SGE::BitmapTextString textString = getText(handle);
		m_colorRangeBuffer.clear();
		m_colorRangeBuffer.reserve(textString.getLength());
		for(u32 i = 0; i < rangeCount; ++i)
		{
			char_attr_color_range_t range { };
			range.begin = ranges[i].begin;
			range.end = ranges[i].end;
			range.color = to_color(ranges[i].color);
			m_colorRangeBuffer.push_back(range);
		}
		textString.setCharAttrColor(m_colorRangeBuffer.data(), rangeCount);
	}

	void SGEGfxDriver::setTextData(GfxDriverObjectHandleType handle, const std::string& data)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString& textString = it->second.textString;
		SGEBitmapTextData& bitmapTextData = getBitmapTextIterator(it->second.textGroup, it->second.textHandle)->second;
		_assert(bitmapTextData.charCount >= textString.getLength());
		bitmapTextData.charCount -= textString.getLength();
		textString.set(data);
		bitmapTextData.charCount += textString.getLength();
	}

	void SGEGfxDriver::setTextActive(GfxDriverObjectHandleType handle, com::Bool isActive)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString& textString = it->second.textString;
		textString.setActive(static_cast<bool>(isActive));
	}

	LineCountType SGEGfxDriver::getTextGlyphIndexFromCoord(GfxDriverObjectHandleType handle, f32 coord)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString& textString = it->second.textString;
		// 'coord' is in centimeters, so convert it into pixels along the width of the monitor/window
		f32 zCoord = SGE::Display::ConvertInchesToPixels({ coord * INCHES_PER_CENTIMETER, 0 }).x;
		return static_cast<LineCountType>(textString.getGlyphIndexFromZCoord(zCoord));
	}

	f32 SGEGfxDriver::getTextCoordFromGlyphIndex(GfxDriverObjectHandleType handle, LineCountType col)
	{
		auto it = getSubTextIterator(handle);
		SGE::BitmapTextString& textString = it->second.textString;
		// get the offset along horizontal axis in pixel coordinates
		f32 zCoord = textString.getZCoordFromGlyphIndex(static_cast<u32>(col));
		// convert it into centimeters
		f32 xCoord = SGE::Display::ConvertPixelsToInches({ zCoord, 0 }).x * CENTIMETERS_PER_INCH;
		return xCoord;
	}

	GfxDriverObjectHandleType SGEGfxDriver::getTextObject(GfxDriverObjectHandleType handle)
	{
		auto it = getSubTextIterator(handle);
		return it->second.textHandle;
	}

	ObjectType SGEGfxDriver::getType(GfxDriverObjectHandleType handle)
	{
		u32 id = GET_ATTACHED_OBJECT_ID(handle);
		auto it = m_typeTable.find(id);
		_assert(it != m_typeTable.end());
		return it->second;
	}

	GfxDriverObjectHandleType SGEGfxDriver::getObject(GfxDriverObjectHandleType handle)
	{
		ObjectType type = getType(handle);
		switch(type)
		{
			case ObjectType::Text:
				return getTextObject(handle);
			case ObjectType::Mesh:
				return getGeometryObject(handle);
		}
		debug_log_error("Invalid ObjectType: %u", static_cast<u8>(type));
		return GFX_DRIVER_OBJECT_NULL_HANDLE;
	}

	void SGEGfxDriver::setObjectActive(GfxDriverObjectHandleType handle, com::Bool isActive)
	{
		auto it = getRenderObjectIterator(handle);
		it->second.setActive(static_cast<bool>(isActive));
	}

	void SGEGfxDriver::setObjectScissor(GfxDriverObjectHandleType handle, const Rect2Df rect)
	{
		auto it = getRenderObjectIterator(handle);
		extent2d_t rectPosition = SGE::Display::ConvertInchesToPixels({ rect.x * INCHES_PER_CENTIMETER, rect.y * INCHES_PER_CENTIMETER });
		extent2d_t rectSize = SGE::Display::ConvertInchesToPixels({ rect.width * INCHES_PER_CENTIMETER, rect.height * INCHES_PER_CENTIMETER });
		it->second.setScissor(irect2d(ioffset2d(rectPosition.x, rectPosition.y), iextent2d(rectSize.width, rectSize.height)));
	}

	void SGEGfxDriver::setObjectPositionXY(GfxDriverObjectHandleType handle, const Vec2Df position)
	{
		auto it = getRenderObjectIterator(handle);
		vec3_t pos = SUTKToSGECoordTransform(position);
		pos.x = it->second.getPosition().x;
		it->second.setPosition(pos);
	}

	void SGEGfxDriver::setObjectDepth(GfxDriverObjectHandleType handle, f32 depth)
	{
		auto it = getRenderObjectIterator(handle);
		vec3_t pos = it->second.getPosition();
		pos.x = depth;
		it->second.setPosition(pos);
	}

	std::unordered_map<id_generator_id_type_t, SGEMeshData>::iterator
	SGEGfxDriver::getMeshIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_meshMappings.find(GET_ATTACHED_OBJECT_ID(handle));
		_assert(it != m_meshMappings.end());
		return it;
	}

	static std::string getParametersString(const Geometry& geometry)
	{
		std::string str;
		const std::vector<SDF::ParameterData>& params = geometry.getSDF().getParams();
		for(const SDF::ParameterData& param : params)
		{
			switch(param.type)
			{
				case SDF::ParameterType::Float32:
				{
					str.append("float");
					break;
				}
				case SDF::ParameterType::Vector2Float32:
				{
					str.append("vec2");
					break;
				}
				case SDF::ParameterType::Unsigned32:
				{
					str.append("uint");
					break;
				}
				case SDF::ParameterType::Vector2Unsigned32:
				{
					str.append("uvec2");
					break;
				}
				case SDF::ParameterType::Signed32:
				{
					str.append("int");
					break;
				}
				case SDF::ParameterType::Vector2Signed32:
				{
					str.append("ivec2");
					break;
				}
				case SDF::ParameterType::InvariantWidth:
				case SDF::ParameterType::InvariantHeight:
				{
					str.append("float");
					break;
				}
				case SDF::ParameterType::InvariantSize:
				{
					str.append("vec2");
					break;
				}
			};

			str.append(" ");
			str.append(param.name);
			str.append(";\n");
		}
		return str;
	}


	static void getParametersUnpackString(const Geometry& geometry, std::string& str)
	{
		const std::vector<SDF::ParameterData>& params = geometry.getSDF().getParams();
		for(const SDF::ParameterData& param : params)
		{
			switch(param.type)
			{
				case SDF::ParameterType::Float32:
				{
					str.append("float");
					break;
				}
				case SDF::ParameterType::Vector2Float32:
				{
					str.append("vec2");
					break;
				}
				case SDF::ParameterType::Unsigned32:
				{
					str.append("uint");
					break;
				}
				case SDF::ParameterType::Vector2Unsigned32:
				{
					str.append("uvec2");
					break;
				}
				case SDF::ParameterType::Signed32:
				{
					str.append("int");
					break;
				}
				case SDF::ParameterType::Vector2Signed32:
				{
					str.append("ivec2");
					break;
				}
				case SDF::ParameterType::InvariantWidth:
				case SDF::ParameterType::InvariantHeight:
				{
					str.append("float");
					break;
				}
				case SDF::ParameterType::InvariantSize:
				{
					str.append("vec2");
					break;
				}
			};

			str.append(" ");
			str.append(param.name);
			if(param.type == SDF::ParameterType::InvariantWidth)
			{
				str.append(" = realizeWidth(");
				str.append("parameters.");
				str.append(param.name);
				str.append(")");
			}
			else if(param.type == SDF::ParameterType::InvariantHeight)
			{
				str.append(" = realizeHeight(");
				str.append("parameters.");
				str.append(param.name);
				str.append(")");
			}
			else
			{
				str.append(" = parameters.");
				str.append(param.name);
			}
			str.append(";\n");
		}
	}

	u64 SGEGfxDriver::getGeometryShaderHash(const Geometry& geometry) const noexcept
	{
		u64 hash = 0x754e30c7e3d13;
		if(geometry.isArray())
			hash ^= (static_cast<u64>(0x53) << 32);
		if(geometry.isFillImage())
			hash ^= (static_cast<u64>(0x71) << 32);
		if(geometry.isSDF())
		{
			hash |= static_cast<u32>(geometry.getSDF().getCode().size());
			std::size_t strHash = std::hash<std::string> { } (geometry.getSDF().getCode());
			const std::vector<SDF::ParameterData>& params = geometry.getSDF().getParams();
			std::size_t nmHash = 0;
			u64 pmHash = 0;
			std::size_t totalParamLength = 0;
			for(const SDF::ParameterData& param : params)
			{
				nmHash ^= std::hash<std::string> { } (param.name);
				nmHash += totalParamLength;
				pmHash = (static_cast<u64>(com::EnumClassToInt(param.type)) << 8) * pmHash;
				totalParamLength += param.name.size();
			}
			hash ^= static_cast<u64>(strHash);
			hash ^= static_cast<u64>(nmHash);
		}
		return hash;
	}

	void SGEGfxDriver::u64ToString(u64 hash, std::array<char, 17>& buffer) const noexcept
	{
		u8* raw = reinterpret_cast<u8*>(&hash);
		for(u32 i = 0; i < 8; ++i)
		{
			auto rem = raw[i] % std::numeric_limits<char>::max();
			buffer[2 * i + 0] = (static_cast<s32>(rem) + 'A') % std::numeric_limits<char>::max() + 'A';
			_com_assert(raw[i] >= rem);
			buffer[2 * i + 1] = ((static_cast<s32>(raw[i] - rem) + 'A') % std::numeric_limits<char>::max()) + 'A';

			_com_assert(buffer[2 * i + 0] != 0);
			_com_assert(buffer[2 * i + 1] != 0);
		}
		buffer[16] = 0;
	}

	SGE::Shader SGEGfxDriver::compileShader(const Geometry& geometry)
	{
		u64 hash = getGeometryShaderHash(geometry);
		std::array<char, 17> buffer;
		u64ToString(hash, buffer);
		std::string_view hashStr { buffer.data(), buffer.size() - 1 };

		// check if already compiled shader exists
		SGE::Shader shader = m_driver.getShaderLibrary().getShader(hashStr);
		if(shader)
			return shader;

		shader = m_driver.getShaderLibrary().loadCachedShader(hash, hashStr);
		if(shader)
			return shader;

		// otherwise derive .v3dshader shader source and compile it
		if(geometry.isSDF())
		{
			std::string str(m_driver.getBuiltinFileData("/sdf_template.template").first);

			// generate the parameter list
			std::string parameters = getParametersString(geometry);

			// replace every occurence of "PARAMETER_HERE" with the generated parameter list
			std::string::size_type pos = str.find("PARAMETERS_HERE");
			while(pos != std::string::npos)
			{
				str.erase(pos, strlen("PARAMETERS_HERE"));
				str.insert(pos, parameters);
				pos += parameters.size();
				pos = str.find("PARAMETERS_HERE", pos);
			}

			// replace every occurence of "PARAMETER_UNPACK_HERE" with unpacked parameter list
			pos = str.find("PARAMETERS_UNPACK_HERE");
			_com_assert(pos != std::string::npos);
			str.erase(pos, strlen("PARAMETERS_UNPACK_HERE"));
			parameters.clear();
			getParametersUnpackString(geometry, parameters);
			str.insert(pos, parameters);
			pos += parameters.size();

			// replace an occurence of "SDF_FN_BODY_HERE" with the Signed Distance Function code
			pos = str.find("SDF_FN_BODY_HERE", pos);
			_com_assert(pos != std::string::npos);
			str.erase(pos, strlen("SDF_FN_BODY_HERE"));
			_com_assert(geometry.getSDF().getCode().size() > 0);
			str.insert(pos, geometry.getSDF().getCode());

			return m_driver.getShaderLibrary().compileAndLoadShader(str, hashStr, SHADER_CACHE_WRITE_BIT, hash);
		}
		else if(geometry.isArray())
			return m_driver.getShaderLibrary().compileAndLoadShader(m_driver.getBuiltinFileData("/solid_color_rect_array.v3dshader").first, hashStr, SHADER_CACHE_WRITE_BIT, hash);
		else if(geometry.isFillImage())
			return m_driver.getShaderLibrary().compileAndLoadShader(m_driver.getBuiltinFileData("/image_rect.v3dshader").first, hashStr, SHADER_CACHE_WRITE_BIT, hash);
		else
			return m_driver.getShaderLibrary().compileAndLoadShader(m_driver.getBuiltinFileData("/solid_color.v3dshader").first, hashStr, SHADER_CACHE_WRITE_BIT, hash);
	}

	static INLINE_IF_RELEASE_MODE vec2_t to_vec2(const Vec2D<f32> value)
	{
		return vec2(value.x, value.y);
	}

	void SGEGfxDriver::updateSDFParameterValues(const Geometry& geometry, SGE::Material material) noexcept
	{
		_com_assert(geometry.isSDF());

		if(!geometry.getSDF().isParamsModified())
			return;

		const std::vector<SDF::ParameterData>& params = geometry.getSDF().getParams();
		std::string str;
		for(const SDF::ParameterData& param : params)
		{
			if(!param.isModified)
				continue;
				
			str.clear();
			str.append("parameters.");
			str.append(param.name);
			switch(param.type)
			{
				case SDF::ParameterType::Float32:
				{
					material.set<float>(str, std::get<f32>(param.value));
					break;
				}
				case SDF::ParameterType::Vector2Float32:
				{
					material.set<vec2_t>(str, to_vec2(std::get<Vec2D<f32>>(param.value)));
					break;
				}
				case SDF::ParameterType::Unsigned32:
				{
					material.set<u32>(str, std::get<u32>(param.value));
					break;
				}
				case SDF::ParameterType::Vector2Unsigned32:
				{
					// TODO: material.set<uvec2>(str, std::get<Vec2D<u32>>(param.value));
					// Currently Material::set doesn't have any template specialization for uvec2
					_com_assert(false);
					break;
				}
				case SDF::ParameterType::Signed32:
				{
					material.set<s32>(str, std::get<s32>(param.value));
					break;
				}
				case SDF::ParameterType::Vector2Signed32:
				{
					// TODO: material.set<ivec2>(str, std::get<Vec2D<s32>>(param.value));
					// Currently Material::set doesn't have any template specialization for ivec2
					_com_assert(false);
					break;
				}
				case SDF::ParameterType::InvariantWidth:
				{
					f32 value = std::get<InvariantWidth>(param.value);
					value = SUTKToSGECoordTransform({ value, 0 }).z;
					material.set<f32>(str, value);
					break;
				}
				case SDF::ParameterType::InvariantHeight:
				{
					f32 value = std::get<InvariantHeight>(param.value);
					value = SUTKToSGECoordTransform({ 0, value }).y;
					material.set<f32>(str, value);
					break;
				}
				case SDF::ParameterType::InvariantSize:
				{
					InvariantSize size = std::get<InvariantSize>(param.value);
					vec3_t v = SUTKToSGECoordTransform({ size.width, size.height });
					material.set<vec2_t>(str, vec2(v.z, v.y));
					break;
				}
			}
		}
	}

	std::pair<SGEMeshData, GfxDriverObjectHandleType> SGEGfxDriver::createMesh(const Geometry& geometry)
	{
		debug_log_info("[SGE] Creating new SGE::Mesh object");
		SGE::Mesh mesh = m_driver.createMesh();
		SGE::RenderQueue::Type queueType = getRenderQueueTypeFromRenderMode(geometry.getRenderMode());
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Mesh, queueType);
		double elapsedTime;
		SGE::Shader shader;
		{
			com::TimeTaken<double> _(elapsedTime);
			shader = compileShader(geometry);
		}
		std::cout << "Time taken to compile Shader: " << elapsedTime << " milliseconds" << std::endl;
		SGE::Material material = m_driver.getMaterialLibrary().createMaterial(shader, "Untittled");
		material.set<vec4_t>("parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
		if(geometry.isArray())
		{
			f32 z = SUTKToSGECoordTransform({ 1.0f, 0 }).z;
			z += getSizeInPixels().x * 0.5f;
			material.set<float>("parameters.numPixelsPerCM", z);
		}
		object.setMaterial(material);
		object.attach(mesh);
		
		// add mesh into the mesh mappings table
		id_generator_id_type_t meshID = id_generator_get(&m_id_generator);
		m_meshMappings.insert({ meshID, { mesh, shader } });

		m_typeTable.insert({ meshID, ObjectType::Mesh });

		// add corresponding render object into the render object mappings table
		id_generator_id_type_t renderObjectID = id_generator_get(&m_id_generator);
		m_renderObjectMappings.insert({ renderObjectID, object });

		static_assert(sizeof(GfxDriverObjectHandleType) == sizeof(u64));

		return { { mesh, shader }, static_cast<GfxDriverObjectHandleType>(BIT64_PACK32(meshID, renderObjectID)) };
	}

	void SGEGfxDriver::createOrUpdateVertexBuffer(SGE::Mesh mesh, const SGE::Mesh::VertexBufferCreateInfo& createInfo) noexcept
	{
		// if there is no vertex buffer then create one with binding equal to zero
		SGE::Buffer vertexBuffer = mesh.getVertexBuffer(createInfo.binding);
		if(!vertexBuffer)
			mesh.createAndAddVertexBuffer(com::cast_away_const(createInfo));
		else
		{
			_assert(vertexBuffer.getTraits()->elementSize == createInfo.stride);
			// if the existing vertex buffer's capacity is not equal to that of new number of posijtions
			// TODO: create a resizable vulkan buffer because it is inefficient to destroy and recreate upon each size change
			// 	We should only recreate the buffer when the capacity is no longer able to hold that much elements
			if(vertexBuffer.getTraits()->elementCount != createInfo.count)
			{
				// then destroy the vertex buffer at binding = 0
				mesh.destroyVertexBuffer(createInfo.binding);
				// and create a new vertex buffer for the binding = 0
				mesh.createAndAddVertexBuffer(com::cast_away_const(createInfo));
			}
			// otherwise, just copy the new data
			else
				vertexBuffer.copyData(0, reinterpret_cast<void*>(createInfo.data), createInfo.stride * createInfo.count);
		}
	}

	GfxDriverObjectHandleType SGEGfxDriver::loadTexture(std::string_view str)
	{
		// TODO: Load the texture from another thread
		return m_textureData.add({ std::string(str), { } });
	}

	void SGEGfxDriver::getTextureAttributes(GfxDriverObjectHandleType texture, TextureAttributes& out)
	{
		SGE::Texture sgeTexture = getTexture(texture);
		SGE::TextureAttributes attr;
		sgeTexture.getAttributes(attr);
		out.width = attr.width;
		out.height = attr.height;
		out.depth = attr.depth;
		out.channelCount = attr.channel_count;
	}

	void SGEGfxDriver::unloadTexture(GfxDriverObjectHandleType handle)
	{
		m_textureData.remove(handle);
		// TODO : unload the texture from SGE Driver's side
	}

	GfxDriverObjectHandleType SGEGfxDriver::loadFont(std::string_view str)
	{
		// TODO: Load the font from another thread
		return m_fontData.add({ std::string(str), { } });
	}

	void SGEGfxDriver::unloadFont(GfxDriverObjectHandleType handle)
	{
		m_fontData.remove(handle);
		// TODO : unload the font from SGE Driver's side
	}

	SGE::Texture SGEGfxDriver::getTexture(GfxDriverObjectHandleType handle) noexcept
	{
		if(handle == GFX_DRIVER_OBJECT_NULL_HANDLE)
			return getDefaultTexture();

		std::pair<std::string, SGE::Texture>& pair = m_textureData.get(handle);
		// TODO: Wait on the another thread to finish loading this texture
		if(!pair.second)
			pair.second = m_driver.loadTexture(TEXTURE_TYPE_ALBEDO, pair.first);
		return pair.second;
	}

	SGE::Texture SGEGfxDriver::getDefaultTexture() noexcept
	{
		if(!m_defaultTexture)
		{
			auto texture_data = m_driver.getBuiltinFileData("/white_image.bmp");
			m_defaultTexture = m_driver.loadTexture(TEXTURE_TYPE_ALBEDO, { reinterpret_cast<const u8*>(texture_data.first.data()), texture_data.second });
		}
		return m_defaultTexture;
	}

	GfxDriverObjectHandleType SGEGfxDriver::compileGeometry(const Geometry& geometry, GfxDriverObjectHandleType previous)
	{
		SGE::Mesh mesh;
		SGE::Material material;
		GfxDriverObjectHandleType newHandle;

		// if handle to the old compiled geometry has been provided, then we should use that one
		// and no need to create new set of SGE objects
		if(previous != GFX_DRIVER_OBJECT_NULL_HANDLE)
		{
			 mesh = getMeshIterator(previous)->second.mesh;
			 material = getRenderObjectIterator(previous)->second.getMaterial();
			 newHandle = previous;
		}
		// otherwise, create new compiled geometry objects.
		else
		{
			std::pair<SGEMeshData, GfxDriverObjectHandleType> result = createMesh(geometry);
			newHandle = result.second;
			mesh = result.first.mesh;
			material = getRenderObjectIterator(result.second)->second.getMaterial();
		}

		// if fill color is modified then update it on the GPU side
		if(geometry.isFillColorModified())
		{
			Color4 color = geometry.getFillColor();
			material.set<vec4_t>("parameters.color", to_vec4(color));
		}
		if(geometry.isFillImageModified())
		{
			UIDriver::ImageReference image = geometry.getFillImage();
			SGE::Texture texture = getTexture(image);
			material.set<SGE::Texture>("albedo", texture);
		}

		// if this is a SDF geometry and its parameters are modified, then we need to update the GPU side buffer
		if(geometry.isSDF() && geometry.getSDF().isParamsModified())
			updateSDFParameterValues(geometry, material);

		if(geometry.isVertexIndexArrayModified())
		{
			// prepare the index buffer create info struct
			SGE::Mesh::IndexBufferCreateInfo createInfo = { };
			const Geometry::VertexIndexArray& indexArray = geometry.getVertexIndexArray();
			void* data = reinterpret_cast<void*>(const_cast<Geometry::VertexIndex*>(indexArray.data()));
			createInfo.data = data;
			createInfo.index_type = INDEX_TYPE_U32;
			createInfo.count = indexArray.size();

			// if there is no index buffer then create one
			if(!mesh.hasIndexBuffer())
				mesh.createAndAddIndexBuffer(createInfo);
			else
			{
				SGE::Buffer indexBuffer = mesh.getIndexBuffer();
				_assert(indexBuffer.getTraits()->elementSize == sizeof(u32));
				// if previous index buffer's size doesn't match the new number of indices
				if(indexBuffer.getTraits()->elementCount != indexArray.size())
				{
					// then destroh the old index buffer
					mesh.destroyIndexBuffer();
					// and create a new index buffer
					mesh.createAndAddIndexBuffer(createInfo);
				}
				// otherwise, just copy the new data to the existing buffer
				else
					indexBuffer.copyData(0, data, sizeof(u32) * indexArray.size());
			}
		}

		// NOTE: if a geometry need to be replicated (i.e. it is an array),
		// it should be obvious that the original geometry information need to be updated whenever instance buffer data is modified
		// that is to avoid vertices still using the previous window size while calculating SGE's coords from SUTK's coords.
		if(geometry.isVertexPositionArrayModified() || (geometry.isArray() && geometry.isInstanceTransformArrayModified()))
		{
			// convert vertex position array into SGE's coordinates and strides.
			auto& positionArray = geometry.getVertexPositionArray();
			vec4_t data[positionArray.size()];
			for(u32 i = 0; i < positionArray.size(); ++i)
			{
				auto pos = SUTKToSGECoordTransform(positionArray[i]);
				data[i] = { pos.x, pos.y, pos.z, 0.0f };
			}

			// prepare the vertex buffer create info struct
			SGE::Mesh::VertexBufferCreateInfo createInfo = { };
			createInfo.data = data,
			createInfo.stride = sizeof(vec4_t),
			createInfo.count = positionArray.size(),
			createInfo.binding = 0;

			createOrUpdateVertexBuffer(mesh, createInfo);
		}

		if(geometry.isVertexTexCoordArrayModified())
		{
			auto& texcoordArray = geometry.getVertexTexCoordArray();

			// prepare the vertex buffer create info struct
			SGE::Mesh::VertexBufferCreateInfo createInfo = { };
			createInfo.data = reinterpret_cast<void*>(com::cast_away_const(texcoordArray.data()));
			createInfo.stride = sizeof(vec2_t);
			createInfo.count = texcoordArray.size();
			createInfo.binding = 3;

			createOrUpdateVertexBuffer(mesh, createInfo);
		}

		if(geometry.isInstanceTransformArrayModified())
		{
			// convert vertex position array into SGE's coordinates and strides.
			auto& transformArray = geometry.getInstanceTransformArray();
			vec4_t data[transformArray.size()];
			for(u32 i = 0; i < transformArray.size(); ++i)
			{
				Rect2Df rect = transformArray[i];
				vec3_t pos = SUTKToSGECoordTransform(rect.getPosition());
				vec3_t size = SUTKToSGECoordTransform(rect.getSize());
				data[i] = { pos.z, pos.y, size.z, size.y };
			}
			
			mesh.setInstanceCount(static_cast<u32>(transformArray.size()));

			// prepare the vertex buffer create info struct
			SGE::Mesh::VertexBufferCreateInfo createInfo = { };
			createInfo.data = (transformArray.size() == 0) ? NULL : data;
			createInfo.stride = sizeof(vec4_t);
			// SGE requires non-zero count to actually allocate non-zero device memory for VkBuffer
			createInfo.count = transformArray.size();
			createInfo.binding = 5;

			SGE::Buffer vertexBuffer = mesh.getVertexBuffer(5);
			// If the size of the buffer is going to be zero and there is no previous buffer created then it is better
			// to skip this a altogether.
			// NOTE: If we were to still proceed then we would get assertion failure in SGE, as there is no point of allocating
			// a Vulkan Buffer of size zero.
			if(vertexBuffer || createInfo.count)
			{
				// if there is no vertex buffer then create one with binding equal to 5
				if(!vertexBuffer)
					mesh.createAndAddVertexBuffer(createInfo);
				else
				{
					_assert(vertexBuffer.getTraits()->elementSize == createInfo.stride);
					// if the existing vertex buffer's capacity is less than that of new number of instance transforms
					if(vertexBuffer.getTraits()->elementCount < transformArray.size())
					{
						// then destroy the vertex buffer at binding = 0
						mesh.destroyVertexBuffer(5);
						// and create a new vertex buffer for the binding = 0
						mesh.createAndAddVertexBuffer(createInfo);
					}
					// otherwise, just copy the new data
					else
						vertexBuffer.copyData(0, reinterpret_cast<void*>(data), sizeof(vec4_t) * transformArray.size());
				}
			}
		}

		return newHandle;
	}

	void SGEGfxDriver::destroyGeometry(GfxDriverObjectHandleType geometry)
	{
		_com_assert(geometry != GFX_DRIVER_OBJECT_NULL_HANDLE);
		// erase the mesh object
		auto it = getMeshIterator(geometry);
		it->second.mesh.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_ATTACHED_OBJECT_ID(geometry)));
		m_meshMappings.erase(it);
		removeIDFromTypeTable(GET_ATTACHED_OBJECT_ID(geometry));

		// erase the corresponding render object also 
		auto it2 = getRenderObjectIterator(geometry);
		SGE::RenderObject object = it2->second;
		object.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_RENDER_OBJECT_ID(geometry)));
		m_renderObjectMappings.erase(it2);
	}

	GfxDriverObjectHandleType SGEGfxDriver::getGeometryObject(GfxDriverObjectHandleType geometry)
	{
		return geometry;
	}

	u32 SGEGfxDriver::getFontBaselineHeightInPixels(SGE::Font font, f32 pointSize) noexcept
	{
		f32 save = font.getCharSize();
		font.setCharSize(pointSize);
		u32 pixels = font.getFontUnitsToPixels(font.getBaselineSpace(), SGE::Display::GetDPI().height);
		font.setCharSize(save);
		return pixels;
	}

	f32 SGEGfxDriver::getFontBaselineHeightInCentimeters(SGE::Font font, f32 pointSize) noexcept
	{
		return SGE::Display::ConvertPixelsToInches({ 0, static_cast<f32>(getFontBaselineHeightInPixels(font, pointSize)) }).height * CENTIMETERS_PER_INCH;
	}

	u32 SGEGfxDriver::getTextBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize)
	{
		SGE::Font font = getText(handle).getFont();
		return getFontBaselineHeightInPixels(font, pointSize);
	}

	f32 SGEGfxDriver::getTextBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize)
	{
		SGE::Font font = getText(handle).getFont();
		return getFontBaselineHeightInCentimeters(font, pointSize);
	}

	u32 SGEGfxDriver::getTextGroupBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize) 
	{
		SGE::Font font = getText(handle).getFont();
		return getFontBaselineHeightInPixels(font, pointSize);
	}

	f32 SGEGfxDriver::getTextGroupBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize) 
	{
		SGE::Font font = getTextGroup(handle).font;
		return getFontBaselineHeightInCentimeters(font, pointSize);
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
				// window size in inches
				extent2d_t size_inches = SGE::Display::ConvertPixelsToInches(extent2d(size.width, size.height));
				auto& _handlerData = *reinterpret_cast<CallbackHandlerData<OnResizeCallbackHandler>*>(handlerData);
				_handlerData.handler(Vec2Df{ size_inches.width * CENTIMETERS_PER_INCH, size_inches.height * CENTIMETERS_PER_INCH });
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

	u32 SGEGfxDriver::addOnCloseHandler(OnCloseCallbackHandler handler)
	{
		u32 id = id_generator_get(&m_id_generator);
		m_onCloseHandlers.insert({ id, { handler, EVENT_SUBSCRIPTION_HANDLE_INVALID } });
		auto& onCloseHandlerData = m_onCloseHandlers[id];
		SGE::Event::SubscriptionHandle handle = m_driver.getRenderWindow().getOnCloseEvent().subscribe(
			[](void* publisher, void* handlerData)
			{
				auto& _handlerData = *reinterpret_cast<CallbackHandlerData<OnCloseCallbackHandler>*>(handlerData);
				_handlerData.handler();
			}, &onCloseHandlerData);
		onCloseHandlerData.handle = handle;
		return id;
	}

	void SGEGfxDriver::removeOnCloseHandler(u32 id)
	{
		auto it = m_onCloseHandlers.find(id);
		if(it == m_onCloseHandlers.end())
			return;
		m_driver.getRenderWindow().getOnCloseEvent().unsubscribe(it->second.handle);
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(id));
		m_onCloseHandlers.erase(it);
	}

	void SGEGfxDriver::setClose(bool isClose)
	{
		m_driver.getRenderWindow().setClose(isClose);
	}

}
