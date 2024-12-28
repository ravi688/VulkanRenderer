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
#include <sge-cpp/Texture.hpp> // for SGE::Texture
#include <common/PtrReference.hpp> // for com::PtrReference

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

	class SGE_API Driver final : public com::PtrReference<renderer_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Driver);

	private:
		memory_allocator_t* m_allocator;

	public:
		Driver(GPUType type = GPUType::Discrete, 
				u32 width = 800, 
				u32 height = 800, 
				const std::string_view title = "SGE",
				bool fullScreen = false,
				bool resizable = true,
				com::OptionalReference<const Requirements> requirements = { });
		~Driver();

		void update() noexcept { renderer_update(m_handle); }
		bool isRunning() noexcept { return renderer_is_running(m_handle); }
		void beginFrame() noexcept { renderer_begin_frame(m_handle); }
		void endFrame() noexcept { renderer_end_frame(m_handle); }
		void dispatchFrame() noexcept { renderer_dispatch_frame(m_handle); }
		void waitIdle() noexcept { renderer_wait_idle(m_handle); }
		void pollEvents() noexcept { renderer_poll_events(m_handle); }

		// shaderCachePath: must be null-terminated
		void setShaderCachePath(const std::string_view shaderCachePath) noexcept
		{
			renderer_set_shader_cache_path(m_handle, shaderCachePath.data());
		}

		const std::pair<std::string_view, u64> getBuiltinFileData(const std::string_view virtualFilePath) const noexcept
		{
			u64 data_size = 0;
			const char* data = renderer_get_builtin_file_data(m_handle, virtualFilePath.data(), &data_size);
			return { std::string_view(data), data_size };
		}

		Font loadFont(const std::string& filePath) noexcept { return Font(*this, filePath); }
		Font createFont(const void* bytes, u64 length) noexcept { return Font(*this, bytes, length); }
		Font createFont(std::span<const std::byte> bytes) noexcept { return createFont(bytes.data(), bytes.size()); }

		MaterialLibrary getMaterialLibrary() const noexcept { return MaterialLibrary(renderer_get_material_library(m_handle)); }
		ShaderLibrary getShaderLibrary() const noexcept { return ShaderLibrary(renderer_get_shader_library(m_handle)); }
		CameraSystem getCameraSystem() const noexcept { return CameraSystem(renderer_get_camera_system(m_handle)); }

		RenderScene createRenderScene(u32 queueMask) noexcept { return RenderScene(*this, queueMask); }
		BitmapGlyphAtlasTexture createBitmapGlyphAtlasTexture(const BitmapGlyphAtlasTexture::CreateInfo& createInfo) noexcept { return BitmapGlyphAtlasTexture(*this, createInfo); }
		BitmapText createBitmapText(BitmapGlyphAtlasTexture texture, Font font = { }) noexcept { return BitmapText(*this, texture, font); }
		Mesh createMesh() noexcept { return Mesh(*this); }
		Texture loadTexture(texture_type_t type, std::string_view str) noexcept { return Texture(*this, type, str); }
		Texture loadTexture(texture_type_t type, const std::span<const u8> data) noexcept { return Texture(*this, type, data); }
		Texture loadTexture(texture_type_t type, const u8* pixelData, u32 width, u32 height, u32 channels) noexcept { return Texture(*this, type, pixelData, width, height, channels); }

		RenderWindow getRenderWindow() const noexcept { return RenderWindow(renderer_get_window(m_handle)); }
	};
}
