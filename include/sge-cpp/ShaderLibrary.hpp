#pragma once

#include <sge/shader_library.h>
#include <sge-cpp/Shader.hpp> /* for Shader */
#include <string> /* for std::string */

namespace SGE
{
	class SGE_API ShaderLibrary
	{
	private:
		shader_library_t* m_handle;
		ShaderLibrary(shader_library_t* handle) noexcept : m_handle(handle) { }

		friend class Driver;
		
	public:
		shader_library_t* getHandle() const noexcept { return m_handle; }

		Shader loadShader(const std::string& filePath) const noexcept
		{
			auto handle = shader_library_load_shader(m_handle, filePath.c_str());
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		Shader compileAndLoadShader(const std::string_view source) const noexcept
		{
			auto handle = shader_library_compile_and_load_shader(m_handle, source.data());
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		Shader createShaderFromPreset(shader_library_shader_preset_t preset) const noexcept
		{
			auto handle = shader_library_create_shader_from_preset(m_handle, preset);
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}
	};
}
