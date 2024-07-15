#pragma once

#include <sge/shader.h> /* for shader_t* */

namespace SGE
{
	class SGE_API Shader
	{
	private:
		shader_t* m_handle;
		Shader(shader_t* handle) noexcept : m_handle(handle) { }

		friend class ShaderLibrary;

	public:
		Shader() noexcept : m_handle(NULL) { }
		shader_t* getHandle() const noexcept { return m_handle; }
	};
}
