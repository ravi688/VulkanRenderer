#pragma once

#include <sutk/defines.hpp> /* for Vec2D, etc. */
#include <sutk/IGfxDriver.hpp> /* for IGfxDriver interface class */

#include <sge-cpp/sge.hpp>

#include <unordered_map> /* for std::unordered_map */

#include <common/id_generator.h> /* for id_generator */
#include <common/IDMap.hpp> // for com::IDMap

namespace SUTK
{	
	struct SGEBitmapTextData
	{
		SGE::BitmapText text;
		SGE::RenderObject object;
		// holds total aggregate number of characters in 'text' (BitmapText), i.e. sum over all text strings's sizes created out of this 'text'
		u32 charCount;
	};
	struct SGEBitmapTextStringData
	{
		SGE::BitmapTextString textString;
		// handle to SGE::BitmapText out of which this text string has been created
		// this is used to hash into the 'm_bitmapTextMappings' unordered map and update the 'charCount'
		GfxDriverObjectHandleType textHandle;
		GfxDriverObjectHandleType textGroup;
	};
	struct SGEMeshData
	{
		SGE::Mesh mesh;
		SGE::Shader shader;
	};

	enum class ObjectType : u8
	{
		Text,
		Mesh
	};

	class Geometry;
	class SGEGfxDriver : public IGfxDriver
	{
	private:
		SGE::Driver& m_driver;
		bool m_autoCmdRecordAndExecute;
		SGE::BitmapGlyphAtlasTexture m_bgaTexture;
		SGE::Font m_defaultFont;
		SGE::RenderScene m_scene;
		SGE::Shader m_shader;
		SGE::Texture m_defaultTexture;
		id_generator_t m_id_generator;
		GfxDriverObjectHandleType m_currentBitmapTextHandle;
		typedef std::unordered_map<id_generator_id_type_t, SGEBitmapTextData> SGEBitmapTextTable;
		struct SGEBitmapTextGroup
		{
			SGE::Font font;
			SGEBitmapTextTable bitmapTextTable;
			GfxDriverObjectHandleType currentBitmapTextHandle;
			RenderMode renderMode;
			f32 depth;
		};
		std::unordered_map<id_generator_id_type_t, SGEBitmapTextGroup> m_bitmapTextGroups;
		std::unordered_map<id_generator_id_type_t, SGEMeshData> m_meshMappings;
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData> m_bitmapTextStringMappings;
		std::unordered_map<id_generator_id_type_t, SGE::RenderObject> m_renderObjectMappings;
		com::IDMap<std::pair<std::string, SGE::Texture>> m_textureData;
		com::IDMap<std::pair<std::string, SGE::Font>> m_fontData;
		std::unordered_map<id_generator_id_type_t, ObjectType> m_typeTable;
		template<typename T>
		struct CallbackHandlerData
		{
			T handler;
			SGE::Event::SubscriptionHandle handle;
		};
		std::unordered_map<id_generator_id_type_t, CallbackHandlerData<OnResizeCallbackHandler>> m_onResizeHandlers;
		std::unordered_map<id_generator_id_type_t, CallbackHandlerData<OnCloseCallbackHandler>> m_onCloseHandlers;

		std::vector<char_attr_color_range_t> m_colorRangeBuffer;

		// We're caching current window size into this member variable to avoid calling internal function calls everytime getSize() is called
		// The value of this variable will be updated whenever window is resized.
		Vec2D<DisplaySizeType> m_size;
		Vec2Df m_sizeCentimeters;

	private:
		std::unordered_map<id_generator_id_type_t, SGEBitmapTextGroup>::iterator
		getBitmapTextGroupIterator(GfxDriverObjectHandleType groupHandle);
		std::unordered_map<id_generator_id_type_t, SGEBitmapTextData>::iterator
		getBitmapTextIterator(GfxDriverObjectHandleType groupHandle, GfxDriverObjectHandleType handle);
		std::unordered_map<id_generator_id_type_t, SGE::RenderObject>::iterator
		getRenderObjectIterator(GfxDriverObjectHandleType handle);
		std::unordered_map<GfxDriverObjectHandleType, SGEBitmapTextStringData>::iterator
		getSubTextIterator(GfxDriverObjectHandleType handle);
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> getOrCreateBitmapText(GfxDriverObjectHandleType textGroup);
		std::pair<SGE::BitmapText, GfxDriverObjectHandleType> createBitmapText(SGEBitmapTextGroup& group);
		void destroyBitmapText(GfxDriverObjectHandleType groupHandle, GfxDriverObjectHandleType handle);
		SGE::BitmapTextString getText(GfxDriverObjectHandleType handle);
		std::unordered_map<id_generator_id_type_t, SGEMeshData>::iterator
		getMeshIterator(GfxDriverObjectHandleType handle);
		std::pair<SGEMeshData, GfxDriverObjectHandleType> createMesh(const Geometry& geometry);
		void createOrUpdateVertexBuffer(SGE::Mesh mesh, const SGE::Mesh::VertexBufferCreateInfo& createInfo) noexcept;
		SGE::Texture getTexture(GfxDriverObjectHandleType handle) noexcept;
		// If called for the first time, then it loads a white image,
		// Subsequent calls return the cached reference to earlier loaded white image.
		SGE::Texture getDefaultTexture() noexcept;
		SGE::Font getFont(GfxDriverObjectHandleType handle) noexcept;
		SGE::Font getDefaultFont() noexcept;
		SGE::Shader compileShader(const Geometry& geometry);
		// Transforms SUTK coordinates (origin at top-left, and y downwards) to SGE coordinates (origin at center, and y upwards)
		vec3_t SUTKToSGECoordTransform(const Vec2Df position);
		Vec2Df SGEToSUTKCoordTransform(const vec3_t position);
		ObjectType getType(GfxDriverObjectHandleType handle);
		void removeIDFromTypeTable(u32 id);
		void destroyTextGroup(std::unordered_map<id_generator_id_type_t, SGEBitmapTextGroup>::iterator it, bool isErase = true);

		void updateSDFParameterValues(const Geometry& geometry, SGE::Material material) noexcept;

		u64 getGeometryShaderHash(const Geometry& geometry) const noexcept;
		void u64ToString(u64 hash, std::array<char, 17>& buffer) const noexcept;

		u32 getFontBaselineHeightInPixels(SGE::Font font, f32 pointSize) noexcept;
		f32 getFontBaselineHeightInCentimeters(SGE::Font font, f32 pointSize) noexcept;
		SGEBitmapTextGroup& getTextGroup(GfxDriverObjectHandleType handle) noexcept;

	public:

		// Constructors
		SGEGfxDriver(SGE::Driver& driver, bool autoCmdRecordAndExecute = false);
		// Destructor
		~SGEGfxDriver();


		// IGfxDriver INTERFACE IMPLEMENTATION

		virtual u32 getNativeWindowHandleSize() override;
		virtual void getNativeWindowHandle(void* const out) override;

		virtual Vec2D<DisplaySizeType> getSizeInPixels() override { return m_size; }
		virtual Vec2Df getSizeInCentimeters() override { return m_sizeCentimeters; }
		virtual void render(UIDriver& driver) override;

		virtual GfxDriverObjectHandleType createTextGroup(RenderMode renderMode = RenderMode::Transparent) override;
		// NOTE: It force updates all bitmap text objects and text strings created from them.
		// Thus, it overrides every font inside the text group.
		virtual void setTextGroupFont(GfxDriverObjectHandleType textGroup, GfxDriverObjectHandleType font) override;
		virtual void setTextGroupDepth(GfxDriverObjectHandleType textGroup, f32 depth) override;
		virtual void destroyTextGroup(GfxDriverObjectHandleType textGroup) override;
		virtual GfxDriverObjectHandleType createText(GfxDriverObjectHandleType textGroup) override;
		virtual void destroyText(GfxDriverObjectHandleType handle) override;

		virtual void setTextPosition(GfxDriverObjectHandleType handle, Vec3Df position) override;
		virtual void setTextDepth(GfxDriverObjectHandleType handle, f32 depth) override;
		virtual void setTextPointSize(GfxDriverObjectHandleType handle, f32 pointSize) override;
		virtual f32 getTextPointSize(GfxDriverObjectHandleType handle) override;
		virtual void setTextFont(GfxDriverObjectHandleType handle, GfxDriverObjectHandleType font) override;
		virtual void setTextColor(GfxDriverObjectHandleType handle, const Color4 color) override;
		virtual void setTextColorRanges(GfxDriverObjectHandleType handle, const ColorRange* ranges, u32 rangeCount) override;
		virtual void setTextData(GfxDriverObjectHandleType handle, const std::string& data) override;
		virtual void setTextActive(GfxDriverObjectHandleType handle, com::Bool isActive) override;

		virtual LineCountType getTextGlyphIndexFromCoord(GfxDriverObjectHandleType handle, f32 coord) override;
		virtual f32 getTextCoordFromGlyphIndex(GfxDriverObjectHandleType handle, LineCountType col) override;

		virtual GfxDriverObjectHandleType getTextObject(GfxDriverObjectHandleType handle) override;
		virtual GfxDriverObjectHandleType getObject(GfxDriverObjectHandleType handle) override;
		virtual void setObjectActive(GfxDriverObjectHandleType handle, com::Bool isActive) override;
		virtual void setObjectScissor(GfxDriverObjectHandleType handle, const Rect2Df rect) override;
		virtual void setObjectPositionXY(GfxDriverObjectHandleType handle, const Vec2Df position) override;
		virtual void setObjectDepth(GfxDriverObjectHandleType handle, f32 depth) override;

		virtual GfxDriverObjectHandleType loadTexture(std::string_view str) override;
		virtual GfxDriverObjectHandleType loadTexture(const std::span<const u8> data) override;
		virtual void getTextureAttributes(GfxDriverObjectHandleType texture, TextureAttributes& out) override;
		virtual void unloadTexture(GfxDriverObjectHandleType handle) override;

		virtual GfxDriverObjectHandleType loadFont(std::string_view str) override;
		virtual void unloadFont(GfxDriverObjectHandleType handle) override;

		// compiles SUTK::Geometry description into SGE objects which can eventually be renderered in SGE
		virtual GfxDriverObjectHandleType compileGeometry(const Geometry& geometryDsc, GfxDriverObjectHandleType previous = GFX_DRIVER_OBJECT_NULL_HANDLE) override;
		virtual void destroyGeometry(GfxDriverObjectHandleType geometry) override;
		virtual GfxDriverObjectHandleType getGeometryObject(GfxDriverObjectHandleType geometry) override;

		virtual u32 getTextBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize) override;
		virtual f32 getTextBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize) override;
		virtual u32 getTextGroupBaselineHeightInPixels(GfxDriverObjectHandleType handle, f32 pointSize) override;
		virtual f32 getTextGroupBaselineHeightInCentimeters(GfxDriverObjectHandleType handle, f32 pointSize) override;
		virtual u32 addOnResizeHandler(OnResizeCallbackHandler handler) override;
		virtual void removeOnResizeHandler(u32 id) override;
		virtual u32 addOnCloseHandler(OnCloseCallbackHandler handler) override;
		virtual void removeOnCloseHandler(u32 id) override;
		virtual void setClose(bool isClose) override;
	};
}
