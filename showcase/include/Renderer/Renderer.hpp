#pragma once

#include <Showcase/Interfaces/ISystem.hpp>
#include <Showcase/Defines.hpp>			// SHOWCASE_API

#include <Renderer/TextMesh.hpp>
#include <Renderer/GlyphMeshPool.hpp>
#include <Renderer/Font.hpp>

#include <renderer/defines.h>			// C header, u32
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

		friend class TextMesh;
		friend class GlyphMeshPool;

	public:
		Renderer(RendererGPUType gpuType = RendererGPUType::AUTO, 
				u32 width = 800, u32 height = 800, 
				const std::string& windowTitle = "V3D Renderer", 
				bool isFullScreen = false);
		~Renderer() = default;

		// Implementation of Showcase::ISystem
		void init() override;
		void terminate() override;
		bool isRunning() const override;

		void beginFrame(float r, float g, float b, float a) const;
		void endFrame() const;
		void update() const;


		TextMesh createTextMesh(GlyphMeshPool pool);
		GlyphMeshPool createGlyphMeshPool(Font font);

		renderer_t* getHandle() const;
	};
}