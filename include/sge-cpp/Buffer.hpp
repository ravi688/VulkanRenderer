#pragma once

#include <sge/sge_buffer.h> // for mesh_t

#include <sge-cpp/PtrReference.hpp> // for SGE::PtrReference

namespace SGE
{
	class SGE_API Buffer : public PtrReference<sge_buffer_t>
	{
		PTR_REFERENCE_DERIVED_CLASS(Buffer)

	public:
		struct Traits
		{
			u32 elementCount;
			u32 elementSize;
		};

		friend class Mesh;

		const Traits* getTraits() const noexcept 
		{ 
			return reinterpret_cast<Traits*>(sge_buffer_get_traits(m_handle)); 
		}
		void copyData(u32 offset, const void* data, u32 size)
		{
			sge_buffer_copy_data(m_handle, offset, const_cast<void*>(data), size);
		}
	};
}
