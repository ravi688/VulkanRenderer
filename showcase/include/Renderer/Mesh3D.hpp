/*
	***This is computer generated notice - Do not modify it***

	VulkanRenderer (inclusive of its dependencies and subprojects 
	such as toolchains written by the same author) is a software to render 
	2D & 3D geometries by writing C/C++ code and shaders.

	File: Mesh3D.hpp is a part of VulkanRenderer

	Copyright (C) 2021 - 2024  Author: Ravi Prakash Singh

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>. 
*/

#pragma once

#include <Showcase/Defines.hpp>
#include <Renderer/IResourceObject.hpp>
#include <Math/Vec3.hpp>
#include <Math/Vec2.hpp>
#include <Math/Mat4.hpp>
#include <Renderer/Color.hpp>
#include <renderer/mesh3d.h>
#include <string>


namespace V3D
{
	typedef index_t INDEX;

	class SHOWCASE_API Mesh3D : IResourceObject
	{
		struct Triangle
		{
			INDEX i0;
			INDEX i1;
			INDEX i2;

			inline Triangle(INDEX x, INDEX y, INDEX z) : i0(x), i1(y), i2(z) { }
			inline Triangle() : i0(0), i1(1), i2(2) { }
			inline Triangle(INDEX offset) : i0(offset), i1(offset + 1), i2(offset + 2) { }

			inline INDEX& operator [](int index) { return reinterpret_cast<INDEX*>(this)[index]; }
		};

	private:
		mesh3d_t* handle;

		friend class Mesh;

	public:
		Mesh3D();
		inline Mesh3D(mesh3d_t* mesh3d) : handle(mesh3d) { }

		// Implementation of IResourceObject interface
		void unload() const override;
		void drop() const override { unload(); };


		void createPositions(INDEX count) const;
		void createNormals(INDEX count) const;
		void createTangents(INDEX count) const;
		void createUVs(INDEX count) const;
		void createColors(INDEX count) const;

		void freePositions() const;
		void freeNormals() const;
		void freeTangents() const;
		void freeColors() const;
		void freeTriangles() const;
		void freeUVs() const;

		bool hasUVs() const;
		bool hasPositions() const;
		bool hasNormals() const;
		bool hasTangents() const;
		bool hasTriangles() const;
		bool hasColors() const;

		INDEX positionCount() const;
		INDEX normalCount() const;
		INDEX tangentCount() const;
		INDEX uvCount() const;
		INDEX colorCount() const;
		INDEX triangleCount() const;


		void flipTriangles() const;
		void calculateNormals() const;
		void calculateTangents() const;
		void optimizeBuffer() const;
		void makeCentroidOrigin() const;

		void setTransform(const Math::Mat4& transform) const;
		Math::Mat4 getTransform() const;

		void foreachPositions(void (*visitor)(Math::Vec3* position, void* user_data), void* user_data) const;
		void foreachNormals(void (*visitor)(Math::Vec3* normal, void* user_data), void* user_data) const;
		void foreachTangents(void (*visitor)(Math::Vec3* tangent, void* user_data), void* user_data) const;
		void foreachColors(void (*visitor)(V3D::Color* color, void* user_data), void* user_data) const;
		void foreachUVs(void (*visitor)(Math::Vec2* color, void* user_data), void* user_data) const;
		void foreachTriangles(void (*visitor)(Triangle* triangle, void* user_data), void* user_data) const;


		// Vertex attribute | Position
		void addPosition(const Math::Vec3& position) const;
		void addPosition(float x, float y, float z) const;

		void setPosition(INDEX index, const Math::Vec3& position) const;
		void setPosition(INDEX index, float x, float y, float z) const;

		Math::Vec3& getPosition(INDEX index) const;

		Math::Vec3* getPositionPtr(INDEX index) const;

		void setPositionX(INDEX index, float x) const;
		void setPositionY(INDEX index, float y) const;
		void setPositionZ(INDEX index, float z) const;
		float getPositionX(INDEX index) const;
		float getPositionY(INDEX index) const;
		float getPositionZ(INDEX index) const;

		// Vertex attribute | Normal
		void addNormal(const Math::Vec3& normal) const;
		void addNormal(float x, float y, float z) const;

		void setNormal(INDEX index, const Math::Vec3& normal) const;
		void setNormal(INDEX index, float x, float y, float z) const;

		Math::Vec3& getNormal(INDEX index) const;

		Math::Vec3* getNormalPtr(INDEX index) const;

		void setNormalX(INDEX index, float x) const;
		void setNormalY(INDEX index, float y) const;
		void setNormalZ(INDEX index, float z) const;
		float getNormalX(INDEX index) const;
		float getNormalY(INDEX index) const;
		float getNormalZ(INDEX index) const;

		// Vertex attribute | Tangent
		void addTangent(const Math::Vec3& tangent) const;
		void addTangent(float x, float y, float z) const;

		void setTangent(INDEX index, const Math::Vec3& tangent) const;
		void setTangent(INDEX index, float x, float y, float z) const;

		Math::Vec3& getTangent(INDEX index) const;

		Math::Vec3* getTangentPtr(INDEX index) const;

		void setTangentX(INDEX index, float x) const;
		void setTangentY(INDEX index, float y) const;
		void setTangentZ(INDEX index, float z) const;
		float getTangentX(INDEX index) const;
		float getTangentY(INDEX index) const;
		float getTangentZ(INDEX index) const;

		// Vertex attribute | V3D::Color (RGB only for now)
		void addColor(const V3D::Color& color) const;
		void addColor(float r, float g, float b) const;

		void setColor(INDEX index, const V3D::Color& color) const;
		void setColor(INDEX index, float r, float g, float b) const;

		V3D::Color& getColor(INDEX index) const;

		V3D::Color* getColorPtr(INDEX index) const;

		void setColorR(INDEX index, float r) const;
		void setColorG(INDEX index, float g) const;
		void setColorB(INDEX index, float b) const;
		// throws undefined exception for now
		void setColorA(INDEX index, float a) const;
		float getColorR(INDEX index) const;
		float getColorG(INDEX index) const;
		float getColorB(INDEX index) const;
		// throws undefined exception for now
		float getColorA(INDEX index) const;

		// Vertex attribute | UV
		void addUV(const Math::Vec2& tangent) const;
		void addUV(float x, float y) const;

		void setUV(const Math::Vec2& tangent) const;
		void setUV(float x, float y) const;

		Math::Vec2& getUV(INDEX index) const;

		Math::Vec2* getUVPtr(INDEX index) const;

		void setUVX(INDEX index, float x) const;
		void setUVY(INDEX index, float y) const;
		float getUVX(INDEX index) const;
		float getUVY(INDEX index) const;

		// Triangles
		void addTriangle(const Triangle& triangle) const;
		void addTriangle(INDEX i0, INDEX i1, INDEX i2) const;

		void setTriangle(INDEX index, const Triangle& tangent) const;
		void setTriangle(INDEX index, INDEX i0, INDEX i1, INDEX i2) const;

		Triangle& getTriangle(INDEX index) const;

		Triangle* getTrianglePtr(INDEX index) const;

		void setTriangle0(INDEX index, INDEX i0) const;
		void setTriangle1(INDEX index, INDEX i1) const;
		void setTriangle2(INDEX index, INDEX i2) const;
		INDEX getTriangle0(INDEX index) const;
		INDEX getTriangle1(INDEX index) const;
		INDEX getTriangle2(INDEX index) const;


		// procedural geometries
		static inline Mesh3D cube(float length) { return cuboid(length, length, length); }
		static inline Mesh3D plane(float length) { return plane(length, length); }
		static Mesh3D cuboid(float length, float width, float height);
		static Mesh3D plane(float length, float width);
		static Mesh3D ellipsoid(float semiAxisX, float semiAxisY, float semiAxisZ);
		static Mesh3D cylinder(float radius, float height);
		static Mesh3D cone(float radius, float height);
		static Mesh3D torus(float majorRadius, float minorRadius);
		static Mesh3D capsule(float radius, float height);


		static u32 sizeOfPosition();
		static u32 sizeofNormal();
		static u32 sizeofTangent();
		static u32 sizeOfColor();
		static u32 sizeofOfUV();
		static u32 sizeofOfIndex(); 	// sizeof (Triangle) / 3


		static Mesh3D load(const char* filePath);
	};
}