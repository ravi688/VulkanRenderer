#pragma once

#include <Showcase/Interfaces/ISystem.hpp>
#include <Showcase/Defines.hpp>			// SHOWCASE_API

#include <Renderer/TextMesh.hpp>
#include <Renderer/GlyphMeshPool.hpp>
#include <Renderer/Font.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Texture.hpp>
#include <Renderer/Mesh.hpp>
#include <Renderer/Material.hpp>
#include <Renderer/RenderWindow.hpp>

#include <renderer/defines.h>			// C header, u32
#include <renderer/renderer.h>			// C header
#include <string>

namespace V3D
{
	enum class RendererGPUType
	{
		AUTO = 0,
		INTEGRATED,
		DISCRETE
	};

	class SHOWCASE_API Renderer : Showcase::ISystem
	{
	private:
		renderer_t* handle;

		friend TextMesh;
		friend GlyphMeshPool;
		friend Mesh;
		friend Shader;
		friend Texture;
		friend Material;

	public:
		Renderer(RendererGPUType gpuType = RendererGPUType::AUTO, 
				u32 width = 800, u32 height = 800, 
				const std::string& windowTitle = "V3D Renderer", 
				bool isFullScreen = false);
		inline Renderer(renderer_t* renderer) : handle(renderer) { }

		// Implementation of Showcase::ISystem
		void init() override;
		void terminate() override;
		bool isRunning() const override;

		inline void beginFrame() const { beginFrame(0, 0, 0, 0); }
		void beginFrame(float r, float g, float b, float a) const;
		void endFrame() const;
		void update() const;

		inline RenderWindow getRenderWindow() const { return renderer_get_window(handle); }

		TextMesh createTextMesh(GlyphMeshPool pool) const;
		GlyphMeshPool createGlyphMeshPool(Font font) const;
		Mesh createMesh(Mesh3D meshData) const;
		Material createMaterial(Shader shader) const;
		Shader loadShader(const std::string& filePath) const;
		Texture loadTexture(TextureType type, ...) const;
		Mesh loadMesh(const std::string& filePath) const;
	};
}