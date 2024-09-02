#pragma once

#include <cstddef> // for NULL

namespace SGE
{
	#define PTR_REFERENCE_DERIVED_CLASS(DerivedClass) \
	public: \
		DerivedClass(PtrReference::HandleType* handle) noexcept : PtrReference(handle) { } \
		DerivedClass() noexcept : PtrReference() { } \
		DerivedClass(const DerivedClass& reference) : PtrReference(static_cast<const PtrReference&>(reference)) { } \
		DerivedClass operator =(DerivedClass reference) noexcept \
		{ \
			m_handle = reference.m_handle; \
			return { m_handle }; \
		} \
		bool operator ==(const DerivedClass reference) const noexcept \
		{ \
			return m_handle == reference.m_handle; \
		} \
		bool operator !=(const DerivedClass reference) const noexcept \
		{ \
			return !operator==(reference); \
		} \
		operator bool() const noexcept { return m_handle == NULL; } \
		operator PtrReference::HandleType*() noexcept { return m_handle; }

	template<typename T>
	class PtrReference
	{
	public:
		typedef T HandleType;
	protected:
		HandleType* m_handle;
	public:
		PtrReference(T* handle) noexcept : m_handle(handle) { }
		PtrReference() noexcept : m_handle(NULL) { }
		PtrReference(const PtrReference& reference) : m_handle(reference.m_handle) { }

		PtrReference operator =(PtrReference reference) noexcept
		{
			m_handle = reference.m_handle;
			return { m_handle };
		}

		bool operator ==(const PtrReference reference) const noexcept
		{
			return m_handle == reference.m_handle;
		}

		bool operator !=(const PtrReference reference) const noexcept
		{
			return !operator==(reference);
		}

		operator bool() const noexcept { return m_handle == NULL; }

		HandleType* getHandle() const noexcept { return m_handle; }
	};
}