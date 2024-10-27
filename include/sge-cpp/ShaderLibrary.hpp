#pragma once

#include <sge/shader_library.h>
#include <sge-cpp/Shader.hpp> /* for Shader */
#include <common/PtrReference.hpp> // for com::PtrReference
#include <string> /* for std::string */

namespace SGE
{
	class SGE_API ShaderLibrary : public com::PtrReference<shader_library_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(ShaderLibrary);
		
		friend class Driver;
		
	public:

		Shader loadShader(const std::string& filePath) const noexcept
		{
			auto handle = shader_library_load_shader(m_handle, filePath.c_str());
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		// name: should be null-terminated
		Shader compileAndLoadShader(const std::string_view source, const std::string_view name) const noexcept
		{
			auto handle = shader_library_compile_and_load_shader(m_handle, source.data(), name.data());
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		Shader loadCachedShader(u64 key, const std::string_view name) const noexcept
		{
			auto handle = shader_library_load_cached_shader(m_handle, key, name.data());
			if(handle == SHADER_HANDLE_INVALID)
				return Shader { };
			else
			{
				shader_t* shader = shader_library_getH(m_handle, handle);
				return Shader(shader);
			}
		}

		Shader compileAndLoadShader(const std::string_view source, const std::string_view name, shader_cache_flags_t flags, u64 key) const noexcept
		{
			auto handle = shader_library_compile_and_load_shader_sc(m_handle, source.data(), name.data(), flags,
				[](const char*, const char*, void* key_ptr) noexcept
				{
					return DREF_TO(u64, key_ptr);
				}, &key);
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		Shader createShaderFromPreset(shader_library_shader_preset_t preset) const noexcept
		{
			auto handle = shader_library_create_shader_from_preset(m_handle, preset);
			shader_t* shader = shader_library_getH(m_handle, handle);
			return Shader(shader);
		}

		Shader getShader(const std::string& name) noexcept 
		{
			return getShader(static_cast<const std::string_view>(name));
		}

		// name: should be null-terminated
		Shader getShader(const std::string_view name) noexcept
		{
			shader_handle_t handle = shader_library_get_handle(m_handle, name.data());
			if(handle == SHADER_HANDLE_INVALID)
				return Shader { };
			else
			{
				shader_t* shader = shader_library_getH(m_handle, handle);
				return Shader(shader);
			}
		}
	};
}
