#pragma once

#include <sge/defines.hpp>
#include <sge/sge.h>

#include <sge-cpp/CameraSystem.hpp> /* for SGE::CameraSystem */
#include <sge-cpp/MaterialLibrary.hpp> /* for SGE::MaterialLibrary */
#include <sge-cpp/ShaderLibrary.hpp> /* for SGE::ShaderLibrary */
#include <sge-cpp/RenderScene.hpp> /* for SGE::RenderScene */
#include <sge-cpp/BitmapGlyphAtlasTexture.hpp> /* for SGE::BitmapGlyphAtlasTexture */
#include <sge-cpp/Font.hpp> /* for SGE::Font */

#include <string> /* for std::string */

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
		u32 maxPointLights;
		u32 maxSpotLights;
		u32 maxFarLights;

		Requirements() noexcept : maxPointLights(0), maxSpotLights(0), maxFarLights(0) { }
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
				com::OptionalReference<Requirements> requirements = { });
		~Driver();

		renderer_t* getHandle() const noexcept { return m_driver; }

		void update() { renderer_update(m_driver); }
		bool isRunning() { return renderer_is_running(m_driver); }
		void beginFrame() { renderer_begin_frame(m_driver); }
		void endFrame() { renderer_end_frame(m_driver); }
		void waitIdle() { renderer_wait_idle(m_driver); }

		Font loadFont(const std::string& filePath) noexcept { return Font(*this, filePath); }

		MaterialLibrary getMaterialLibrary() const noexcept { return MaterialLibrary(renderer_get_material_library(m_driver)); }
		ShaderLibrary getShaderLibrary() const noexcept { return ShaderLibrary(renderer_get_shader_library(m_driver)); }
		CameraSystem getCameraSystem() const noexcept { return CameraSystem(renderer_get_camera_system(m_driver)); }

		RenderScene createRenderScene(u32 queueMask) noexcept { return RenderScene(*this, queueMask); }
		BitmapGlyphAtlasTexture createBitmapGlyphAtlasTexture(const BitmapGlyphAtlasTexture::CreateInfo& createInfo) noexcept { return BitmapGlyphAtlasTexture(*this, createInfo); }
		BitmapText createBitmapText(BitmapGlyphAtlasTexture texture) noexcept { return BitmapText(*this, texture); }
	};
}
