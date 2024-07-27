#pragma once

#include <sge/defines.hpp>
#include <sge/sge.h>

#include <sge-cpp/CameraSystem.hpp> /* for SGE::CameraSystem */
#include <sge-cpp/MaterialLibrary.hpp> /* for SGE::MaterialLibrary */
#include <sge-cpp/ShaderLibrary.hpp> /* for SGE::ShaderLibrary */
#include <sge-cpp/RenderScene.hpp> /* for SGE::RenderScene */
#include <sge-cpp/BitmapGlyphAtlasTexture.hpp> /* for SGE::BitmapGlyphAtlasTexture */
#include <sge-cpp/Font.hpp> /* for SGE::Font */
#include <sge-cpp/RenderWindow.hpp> /* for SGE::RenderWindow */
#include <sge-cpp/Mesh.hpp> // for SGE::Mesh

#include <string> /* for std::string */
#include <string_view> /* for std::string_view */

namespace SGE
{
	enum class GPUType : u8
	{
		Auto = 0,
		Integrated,
		Discrete,
		CPU
	};

	struct Requirements
	{
		bool bitmapText;
		u32 maxPointLights;
		u32 maxSpotLights;
		u32 maxFarLights;

		Requirements() noexcept : bitmapText(false), maxPointLights(0), maxSpotLights(0), maxFarLights(0) { }
		Requirements(bool _bitmapText) noexcept : bitmapText(_bitmapText), maxPointLights(0), maxSpotLights(0), maxFarLights(0) { }
	};

	class SGE_API Driver final
	{
	private:
		memory_allocator_t* m_allocator;
		renderer_t* m_driver;

	public:
		Driver(GPUType type = GPUType::Discrete, 
				u32 width = 800, 
				u32 height = 800, 
				const std::string& title = "SGE",
				bool fullScreen = false,
				bool resizable = true,
				com::OptionalReference<const Requirements> requirements = { });
		~Driver();

		renderer_t* getHandle() const noexcept { return m_driver; }

		void update() { renderer_update(m_driver); }
		bool isRunning() { return renderer_is_running(m_driver); }
		void beginFrame() { renderer_begin_frame(m_driver); }
		void endFrame() { renderer_end_frame(m_driver); }
		void waitIdle() { renderer_wait_idle(m_driver); }

		const std::pair<std::string_view, u64> getBuiltinFileData(const std::string_view virtualFilePath) const noexcept
		{
			u64 data_size = 0;
			const char* data = renderer_get_builtin_file_data(m_driver, virtualFilePath.data(), &data_size);
			return { std::string_view(data), data_size };
		}

		Font loadFont(const std::string& filePath) noexcept { return Font(*this, filePath); }
		Font createFont(const void* bytes, u64 length) noexcept { return Font(*this, bytes, length); }

		MaterialLibrary getMaterialLibrary() const noexcept { return MaterialLibrary(renderer_get_material_library(m_driver)); }
		ShaderLibrary getShaderLibrary() const noexcept { return ShaderLibrary(renderer_get_shader_library(m_driver)); }
		CameraSystem getCameraSystem() const noexcept { return CameraSystem(renderer_get_camera_system(m_driver)); }

		RenderScene createRenderScene(u32 queueMask) noexcept { return RenderScene(*this, queueMask); }
		BitmapGlyphAtlasTexture createBitmapGlyphAtlasTexture(const BitmapGlyphAtlasTexture::CreateInfo& createInfo) noexcept { return BitmapGlyphAtlasTexture(*this, createInfo); }
		BitmapText createBitmapText(BitmapGlyphAtlasTexture texture) noexcept { return BitmapText(*this, texture); }
		Mesh createMesh() noexcept { return Mesh(*this); }

		RenderWindow getRenderWindow() const noexcept { return RenderWindow(renderer_get_window(m_driver)); }
	};
}
