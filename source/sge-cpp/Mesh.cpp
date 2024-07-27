#include <sge-cpp/Mesh.hpp>
#include <sge-cpp/sge.hpp> // for SGE::Driver::getHandle()

namespace SGE
{
	Mesh::Mesh(Driver& driver) noexcept
	{
		m_handle = mesh_create(driver.getHandle(), NULL);
	}

	void Mesh::destroy() noexcept
	{
		mesh_destroy(m_handle);
		mesh_release_resources(m_handle);
		m_handle = NULL;
	}
}