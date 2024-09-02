#pragma once

#include <common/PtrReference.hpp> // for com::PtrReference
#include <sge/shader.h> // for shader_t

namespace SGE
{
	class Shader : public com::PtrReference<shader_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Shader)
	};
}
