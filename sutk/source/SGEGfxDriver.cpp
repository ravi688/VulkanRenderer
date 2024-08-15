#include <sutk/SGEGfxDriver.hpp>
#include <sutk/Geometry.hpp> // for SGE::Geometry
#include <sge-cpp/Display.hpp>

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
		m_scene = driver.createRenderScene(BIT64(RENDER_QUEUE_TYPE_GEOMETRY));
		// add the camera to the scene 
		m_scene.addCamera(camera);

		m_shader = shaderLibrary.compileAndLoadShader(driver.getBuiltinFileData("/bitmap_text_shader.v3dshader").first);
		auto font_data = driver.getBuiltinFileData("/Roboto-Bold.ttf");
		m_font = driver.createFont(font_data.first.data(), font_data.second);

		m_font.setCharSize(12);

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
		for(auto it = m_bitmapTextGroups.begin(); it != m_bitmapTextGroups.end(); it++)
			destroyTextGroup(it, false);
		m_bitmapTextGroups.clear();
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

	std::pair<SGE::BitmapText, GfxDriverObjectHandleType> SGEGfxDriver::createBitmapText(SGEBitmapTextGroup& group)
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
		group.bitmapTextTable.insert({ bitmapTextID, { text, 0u } });

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

	GfxDriverObjectHandleType SGEGfxDriver::createTextGroup()
	{
		id_generator_id_type_t id = id_generator_get(&m_id_generator);
		SGEBitmapTextGroup& group = m_bitmapTextGroups[id];
		group.currentBitmapTextHandle = GFX_DRIVER_OBJECT_NULL_HANDLE;
		return static_cast<GfxDriverObjectHandleType>(id);
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

	void SGEGfxDriver::setTextPosition(GfxDriverObjectHandleType handle, Vec2Df position)
	{
		getText(handle).setPosition(SUTKToSGECoordTransform(position));
	}

	void SGEGfxDriver::setTextPointSize(GfxDriverObjectHandleType handle, f32 pointSize)
	{
		getText(handle).setPointSize(pointSize);
	}

	f32 SGEGfxDriver::getTextPointSize(GfxDriverObjectHandleType handle)
	{
		return getText(handle).getPointSize();
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

	void SGEGfxDriver::setObjectScissor(GfxDriverObjectHandleType handle, const Rect2Df rect)
	{
		auto it = getRenderObjectIterator(handle);
		extent2d_t rectPosition = SGE::Display::ConvertInchesToPixels({ rect.x * INCHES_PER_CENTIMETER, rect.y * INCHES_PER_CENTIMETER });
		extent2d_t rectSize = SGE::Display::ConvertInchesToPixels({ rect.width * INCHES_PER_CENTIMETER, rect.height * INCHES_PER_CENTIMETER });
		it->second.setScissor(irect2d(ioffset2d(rectPosition.x, rectPosition.y), iextent2d(rectSize.width, rectSize.height)));
	}

	void SGEGfxDriver::setObjectPosition(GfxDriverObjectHandleType handle, const Vec2Df position)
	{
		auto it = getRenderObjectIterator(handle);
		it->second.setPosition(SUTKToSGECoordTransform(position));
	}

	std::unordered_map<id_generator_id_type_t, SGEMeshData>::iterator
	SGEGfxDriver::getMeshIterator(GfxDriverObjectHandleType handle)
	{	
		auto it = m_meshMappings.find(GET_ATTACHED_OBJECT_ID(handle));
		_assert(it != m_meshMappings.end());
		return it;
	}

	SGE::Shader SGEGfxDriver::compileShader(const Geometry& geometry)
	{
		return m_driver.getShaderLibrary().compileAndLoadShader(m_driver.getBuiltinFileData("/solid_color.v3dshader").first);
	}

	std::pair<SGEMeshData, GfxDriverObjectHandleType> SGEGfxDriver::createMesh(const Geometry& geometry)
	{
		debug_log_info("[SGE] Creating new SGE::Mesh object");
		SGE::Mesh mesh = m_driver.createMesh();
		SGE::RenderObject object = m_scene.createObject(SGE::RenderObject::Type::Mesh, SGE::RenderQueue::Type::Geometry);
		SGE::Shader shader = compileShader(geometry);
		SGE::Material material = m_driver.getMaterialLibrary().createMaterial(shader, "Untittled");
		material.set<vec4_t>("parameters.color", vec4(1.0f, 1.0f, 1.0f, 1.0f));
		object.setMaterial(material);
		object.attach(mesh);
		// rebuild render pass graph as new objects have been added into the render scene 
		m_scene.buildQueues();
		
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

		if(geometry.isVertexPositionArrayModified())
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

			// if there is no vertex buffer then create one with binding equal to zero
			if(mesh.getVertexBufferCount() == 0)
				mesh.createAndAddVertexBuffer(createInfo);
			else
			{
				SGE::Buffer vertexBuffer = mesh.getVertexBuffer(0);
				_assert(vertexBuffer.getTraits()->elementSize == createInfo.stride);
				// if the existing vertex buffer's capacity is not equal to that of new number of posijtions
				if(vertexBuffer.getTraits()->elementCount != positionArray.size())
				{
					// then destroy the vertex buffer at binding = 0
					mesh.destroyVertexBuffer(0);
					// and create a new vertex buffer for the binding = 0
					mesh.createAndAddVertexBuffer(createInfo);
				}
				// otherwise, just copy the new data
				else
					vertexBuffer.copyData(0, reinterpret_cast<void*>(data), sizeof(vec4_t) * positionArray.size());
			}
		}

		return newHandle;
	}

	void SGEGfxDriver::destroyGeometry(GfxDriverObjectHandleType geometry)
	{
		// erase the mesh object
		auto it = getMeshIterator(geometry);
		it->second.mesh.destroy();
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_ATTACHED_OBJECT_ID(geometry)));
		m_meshMappings.erase(it);
		removeIDFromTypeTable(GET_ATTACHED_OBJECT_ID(geometry));

		// erase the corresponding render object also 
		auto it2 = getRenderObjectIterator(geometry);
		// SGE::RenderObject object = *it2;
		// TODO: object.destroy()
		id_generator_return(&m_id_generator, static_cast<id_generator_id_type_t>(GET_RENDER_OBJECT_ID(geometry)));
		m_renderObjectMappings.erase(it2);
	}

	GfxDriverObjectHandleType SGEGfxDriver::getGeometryObject(GfxDriverObjectHandleType geometry)
	{
		return geometry;
	}

	u32 SGEGfxDriver::getTextBaselineHeightInPixels(GfxDriverObjectHandleType handle)
	{
		f32 pointSize = getText(handle).getPointSize();
		f32 save = m_font.getCharSize();
		m_font.setCharSize(pointSize);
		u32 pixels = m_font.getFontUnitsToPixels(m_font.getBaselineSpace(), SGE::Display::GetDPI().height);
		m_font.setCharSize(save);
		return pixels;
	}

	f32 SGEGfxDriver::getTextBaselineHeightInCentimeters(GfxDriverObjectHandleType handle)
	{
		return SGE::Display::ConvertPixelsToInches({ 0, static_cast<f32>(getTextBaselineHeightInPixels(handle)) }).height * CENTIMETERS_PER_INCH;
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
				auto& _handlerData = *reinterpret_cast<OnResizeCallbackHandlerData*>(handlerData);
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
}
