#pragma once

#include <sge/sge_buffer.h> // for mesh_t

namespace SGE
{
	class SGE_API Buffer
	{
	public:
		struct Traits
		{
			u32 elementCount;
			u32 elementSize;
		};

	private:
		sge_buffer_t* m_handle;
		Buffer(sge_buffer_t* handle) noexcept : m_handle(handle) { }

		friend class Mesh;

	public:
		Buffer() noexcept : m_handle(NULL) { }
		sge_buffer_t* getHandle() const noexcept { return m_handle; }

		operator bool() const noexcept { return m_handle != NULL; }
		operator sge_buffer_t*() noexcept { return m_handle; }


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
