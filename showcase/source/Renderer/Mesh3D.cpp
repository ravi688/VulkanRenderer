
#include <Renderer/Mesh3D.hpp>

namespace V3D
{
	SHOWCASE_API Mesh3D::Mesh3D()
	{
		handle = mesh3d_new();
	}

	SHOWCASE_API void Mesh3D::unload() const
	{
		mesh3d_destroy(handle);
	}

	SHOWCASE_API void Mesh3D::createPositions(INDEX count) const
	{
		mesh3d_positions_new(handle, count);
	}

	SHOWCASE_API void Mesh3D::createNormals(INDEX count) const
	{
		mesh3d_normals_new(handle, count);
	}

	SHOWCASE_API void Mesh3D::createTangents(INDEX count) const
	{
		mesh3d_tangents_new(handle, count);
	}

	SHOWCASE_API void Mesh3D::createUVs(INDEX count) const
	{
		mesh3d_uvs_new(handle, count);
	}

	SHOWCASE_API void Mesh3D::createColors(INDEX count) const
	{
		mesh3d_colors_new(handle, count);
	}

	SHOWCASE_API void Mesh3D::freePositions() const
	{
		mesh3d_positions_free(handle);
	}

	SHOWCASE_API void Mesh3D::freeNormals() const
	{
		mesh3d_normals_free(handle);
	}

	SHOWCASE_API void Mesh3D::freeTangents() const
	{
		mesh3d_tangents_free(handle);
	}

	SHOWCASE_API void Mesh3D::freeColors() const
	{
		mesh3d_colors_free(handle);
	}

	SHOWCASE_API void Mesh3D::freeTriangles() const
	{
		mesh3d_triangles_free(handle);
	}

	SHOWCASE_API void Mesh3D::freeUVs() const
	{
		mesh3d_uvs_free(handle);
	}

	SHOWCASE_API bool Mesh3D::hasPositions() const
	{
		return mesh3d_has_positions(handle);
	}

	SHOWCASE_API bool Mesh3D::hasNormals() const
	{
		return mesh3d_has_normals(handle);
	}

	SHOWCASE_API bool Mesh3D::hasTangents() const
	{
		return mesh3d_has_tangents(handle);
	}

	SHOWCASE_API bool Mesh3D::hasColors() const
	{
		return mesh3d_has_colors(handle);
	}

	SHOWCASE_API bool Mesh3D::hasTriangles() const
	{
		return mesh3d_has_triangles(handle);
	}

	SHOWCASE_API bool Mesh3D::hasUVs() const
	{
		return mesh3d_has_uvs(handle);
	}


	SHOWCASE_API INDEX Mesh3D::positionCount() const
	{
		return mesh3d_positions_count(handle);
	}

	SHOWCASE_API INDEX Mesh3D::normalCount() const
	{
		return mesh3d_normals_count(handle);
	}

	SHOWCASE_API INDEX Mesh3D::tangentCount() const
	{
		return mesh3d_tangents_count(handle);
	}

	SHOWCASE_API INDEX Mesh3D::colorCount() const
	{
		return mesh3d_colors_count(handle);
	}

	SHOWCASE_API INDEX Mesh3D::triangleCount() const
	{
		return mesh3d_triangles_count(handle);
	}

	SHOWCASE_API INDEX Mesh3D::uvCount() const
	{
		return mesh3d_uvs_count(handle);
	}

	SHOWCASE_API void Mesh3D::flipTriangles() const
	{
		mesh3d_flip_triangles(handle);
	}

	SHOWCASE_API void Mesh3D::calculateNormals() const
	{
		mesh3d_calculate_normals(handle);
	}

	SHOWCASE_API void Mesh3D::calculateTangents() const
	{
		mesh3d_calculate_tangents(handle);
	}

	SHOWCASE_API void Mesh3D::optimizeBuffer() const
	{
		mesh3d_optimize_buffer(handle);
	}

	SHOWCASE_API void Mesh3D::makeCentroidOrigin() const
	{
		mesh3d_make_centroid_origin(handle);
	}

	SHOWCASE_API void Mesh3D::setTransform(const Math::Mat4& transform) const
	{
		mesh3d_transform_set(handle, const_cast<Math::Mat4&>(transform));
	}

	SHOWCASE_API Math::Mat4 Mesh3D::getTransform() const
	{
		return mesh3d_transform_get(handle);
	}

	SHOWCASE_API void Mesh3D::foreachPositions(void (*visitor)(Math::Vec3* position, void* user_data), void* user_data) const
	{
		mesh3d_positions_foreach(handle, reinterpret_cast<void (*)(vec3_t(float)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::foreachNormals(void (*visitor)(Math::Vec3* normal, void* user_data), void* user_data) const
	{
		mesh3d_normals_foreach(handle, reinterpret_cast<void (*)(vec3_t(float)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::foreachTangents(void (*visitor)(Math::Vec3* tangent, void* user_data), void* user_data) const
	{
		mesh3d_tangents_foreach(handle, reinterpret_cast<void (*)(vec3_t(float)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::foreachUVs(void (*visitor)(Math::Vec2* uv, void* user_data), void* user_data) const
	{
		mesh3d_uvs_foreach(handle, reinterpret_cast<void (*)(vec2_t(float)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::foreachColors(void (*visitor)(Color* color, void* user_data), void* user_data) const
	{
		mesh3d_colors_foreach(handle, reinterpret_cast<void (*)(vec3_t(float)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::foreachTriangles(void (*visitor)(Triangle* triangle, void* user_data), void* user_data) const
	{
		mesh3d_triangles_foreach(handle, reinterpret_cast<void (*)(vec3_t(index_t)*, void*)>(visitor), user_data);
	}

	SHOWCASE_API void Mesh3D::addPosition(const Math::Vec3& position) const
	{
		mesh3d_position_add_vec3(handle, const_cast<Math::Vec3&>(position));
	}

	SHOWCASE_API void Mesh3D::addPosition(float x, float y, float z) const
	{
		mesh3d_position_add(handle, x, y, z);
	}


	SHOWCASE_API void Mesh3D::setPosition(INDEX index, const Math::Vec3& position) const
	{
		mesh3d_position_set_vec3(handle, index, const_cast<Math::Vec3&>(position));
	}

	SHOWCASE_API void Mesh3D::setPosition(INDEX index, float x, float y, float z) const
	{
		mesh3d_position_set(handle, index, x, y, z);
	}

	SHOWCASE_API Math::Vec3& Mesh3D::getPosition(INDEX index) const
	{
		return *reinterpret_cast<Math::Vec3*>(mesh3d_position_get_ptr(handle, index));
	}

	SHOWCASE_API Math::Vec3* Mesh3D::getPositionPtr(INDEX index) const
	{
		return reinterpret_cast<Math::Vec3*>(mesh3d_position_get_ptr(handle, index));
	}


	SHOWCASE_API void Mesh3D::setPositionX(INDEX index, float x) const
	{
		mesh3d_position_set_x(handle, index, x);
	}

	SHOWCASE_API void Mesh3D::setPositionY(INDEX index, float y) const
	{
		mesh3d_position_set_y(handle, index, y);
	}

	SHOWCASE_API void Mesh3D::setPositionZ(INDEX index, float z) const
	{
		mesh3d_position_set_z(handle, index, z);
	}

	SHOWCASE_API float Mesh3D::getPositionX(INDEX index) const
	{
		return mesh3d_position_get_x(handle, index);
	}

	SHOWCASE_API float Mesh3D::getPositionY(INDEX index) const
	{
		return mesh3d_position_get_y(handle, index);
	}

	SHOWCASE_API float Mesh3D::getPositionZ(INDEX index) const
	{
		return mesh3d_position_get_z(handle, index);
	}


	SHOWCASE_API void Mesh3D::addColor(const Color& color) const
	{
		mesh3d_color_add_vec3(handle, const_cast<Color&>(color));
	}


	SHOWCASE_API Mesh3D Mesh3D::cuboid(float length, float width, float height)
	{
		return mesh3d_cube(length);
	}

	SHOWCASE_API Mesh3D Mesh3D::plane(float length, float width)
	{
		return mesh3d_plane(length);
	}

	SHOWCASE_API Mesh3D Mesh3D::load(const char* filePath)
	{
		return mesh3d_load(filePath);
	}
}