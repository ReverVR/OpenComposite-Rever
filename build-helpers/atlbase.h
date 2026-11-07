// Rever VR: the part of ATL's atlbase.h that OpenComposite uses (CComPtr / CComQIPtr), so it builds
// without Visual Studio's optional ATL component. COM reference counting only.
#pragma once
// What the real atlbase.h brings along and OpenComposite relies on without including it itself
#include <windows.h>
#include <shlwapi.h>
#include <unknwn.h>
#include <chrono>
#include <thread>
#pragma comment(lib, "shlwapi.lib")

template <class T>
class CComPtr {
public:
	T* p = nullptr;

	CComPtr() noexcept = default;
	CComPtr(decltype(nullptr)) noexcept {}
	CComPtr(T* other) noexcept : p(other) { if (p) p->AddRef(); }
	CComPtr(const CComPtr& other) noexcept : p(other.p) { if (p) p->AddRef(); }
	CComPtr(CComPtr&& other) noexcept : p(other.p) { other.p = nullptr; }
	~CComPtr() { Release(); }

	CComPtr& operator=(T* other) noexcept
	{
		if (other) other->AddRef();
		Release();
		p = other;
		return *this;
	}
	CComPtr& operator=(const CComPtr& other) noexcept { return *this = other.p; }
	CComPtr& operator=(CComPtr&& other) noexcept
	{
		if (this != &other) {
			Release();
			p = other.p;
			other.p = nullptr;
		}
		return *this;
	}
	CComPtr& operator=(decltype(nullptr)) noexcept
	{
		Release();
		return *this;
	}

	void Release() noexcept
	{
		if (p) {
			T* old = p;
			p = nullptr;
			old->Release();
		}
	}

	operator T*() const noexcept { return p; }
	T* operator->() const noexcept { return p; }
	T& operator*() const noexcept { return *p; }
	T** operator&() noexcept { return &p; } // as ATL: for out parameters, expected to be empty
	bool operator!() const noexcept { return p == nullptr; }
};

template <class T>
class CComQIPtr : public CComPtr<T> {
public:
	CComQIPtr() noexcept = default;
	CComQIPtr(decltype(nullptr)) noexcept {}
	CComQIPtr(T* other) noexcept : CComPtr<T>(other) {}
	CComQIPtr(IUnknown* other) noexcept { Query(other); }

	CComQIPtr& operator=(T* other) noexcept
	{
		CComPtr<T>::operator=(other);
		return *this;
	}
	CComQIPtr& operator=(IUnknown* other) noexcept
	{
		this->Release();
		Query(other);
		return *this;
	}
	CComQIPtr& operator=(decltype(nullptr)) noexcept
	{
		this->Release();
		return *this;
	}

private:
	void Query(IUnknown* other) noexcept
	{
		if (other) other->QueryInterface(__uuidof(T), reinterpret_cast<void**>(&this->p));
	}
};
