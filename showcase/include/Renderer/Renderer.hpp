/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Renderer.hpp is a part of VulkanRenderer

	Copyright (C) 2023  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

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