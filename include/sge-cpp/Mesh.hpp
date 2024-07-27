#pragma once

#include <sge/mesh.h> // for mesh_t

#include <sge-cpp/Buffer.hpp> // for SGE::Buffer

namespace SGE
{
	class Driver;
	class SGE_API Mesh
	{
	private:
		mesh_t* m_handle;
		Mesh(mesh_t* handle) noexcept : m_handle(handle) { }

	public:
		Mesh() noexcept : m_handle(NULL) { }
		Mesh(Driver& driver) noexcept;
		mesh_t* getHandle() const noexcept { return m_handle; }

		void destroy() noexcept;

		typedef vertex_buffer_create_info_t VertexBufferCreateInfo;

		void createAndAddVertexBuffer(VertexBufferCreateInfo& createInfo) noexcept
		{
			mesh_create_and_add_vertex_buffer(m_handle, &createInfo);
		}
		void destroyVertexBuffers() noexcept
		{
			mesh_destroy_vertex_buffers(m_handle);
		}
		void destroyVertexBuffer(u32 binding) noexcept
		{
			mesh_destroy_vertex_buffer(m_handle, binding);
		}

		typedef index_buffer_create_info_t IndexBufferCreateInfo;

		void createAndAddIndexBuffer(IndexBufferCreateInfo& createInfo) noexcept
		{
			mesh_create_and_set_index_buffer(m_handle, &createInfo);
		}
		typedef index_type_t IndexType;
		void setIndexBuffer(Buffer buffer, IndexType indexType) noexcept
		{
			mesh_set_index_buffer(m_handle, buffer.getHandle(), indexType);
		}
		void destroyIndexBuffer() noexcept
		{
			mesh_destroy_index_buffer(m_handle);
		}
		Buffer getIndexBuffer() noexcept
		{
			return Buffer(mesh_get_index_buffer(m_handle));
		}
		bool hasIndexBuffer() const noexcept
		{
			return mesh_has_index_buffer(m_handle);
		}
		u32 getVertexBufferCount() const noexcept
		{
			return mesh_get_vertex_buffer_count(m_handle);
		}
		Buffer getVertexBuffer(u32 binding)
		{
			return Buffer(mesh_get_vertex_buffer(m_handle, binding));
		}
		Buffer getVertexBufferAt(u32 index)
		{
			return Buffer(mesh_get_vertex_buffer_at(m_handle, index));
		}
	};
}
