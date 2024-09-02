#pragma once

#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference
#include <sge/shader.h> // for shader_t

namespace SGE
{
	class Shader : public PtrReference<shader_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Shader)
	};
}
