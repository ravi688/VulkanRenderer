
#pragma once

#include <Showcase/Defines.hpp>	// SHOWCASE_API
#include <Renderer/IRenderObject.hpp>
#include <renderer/shader.h>

#include <string>

namespace V3D
{
	class Renderer;
	class SHOWCASE_API Shader : IRenderObject
	{
	private:
		shader_t* handle;

		friend Renderer;
		friend class Material;

		Shader(Renderer& renderer, const std::string& filePath);

	public:

		inline Shader(shader_t* shader) : handle(shader) { }

		// Implementation of IRenderObject interface
		void destroy() const;
		void releaseResources() const;
		void drop() const;

	};
}
