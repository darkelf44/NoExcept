// Include guard
#pragma once

// Local includes
#include "nx-type.hh"

// Namespace "nx"
namespace nx {
	
// Open variation of unique_ptr, the inner pointer is public and freely accessable
template<typename T> struct UniquePtr
{
	// Referenced type
	using Type = T;
	
	// Constructors & destructors
	UniquePtr() noexcept
		: pointer(nullptr) {}
	explicit UniquePtr(T * ptr) noexcept
		: pointer(ptr) {}
	~UniquePtr() noexcept
		{delete pointer;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T> &) = delete;
	UniquePtr<T> & operator = (const UniquePtr<T> &) = delete;
	
	// Move operators
	UniquePtr(UniquePtr<T> && ptr) noexcept
		: pointer(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T> && ptr) noexcept
		{return reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr) noexcept
		: pointer(ptr.release()) {}
	template<typename U> UniquePtr<T> & operator = (UniquePtr<U> && ptr) noexcept
		{return reset(ptr.release());}
	
	// Operators & methods
	T * release() noexcept
		{T * ptr = pointer; pointer = nullptr; return ptr;}
	UniquePtr<T> & reset(T * ptr = nullptr)
		{delete pointer; pointer = ptr; return * this;}
	T * operator -> () const noexcept
		{return pointer;}
	T & operator * () const noexcept
		{return * pointer;}
	T * get() const noexcept
		{return pointer;}
	T & access() const noexcept
		{return * pointer;}
	
	// Support postfix operators of underlying type [], (), (except if the underlying type is a pointer)
	template<typename I> auto index(I && i) const noexcept(noexcept(param<T>()[param<I &&>()])) -> EnableIf<!nx::type::isPointer<T>(), decltype(param<T>()[param<I &&>()])>
		{return (*pointer)[static_cast<I &&>(i)];}
	template<typename... TS> auto call(TS && ... args) const noexcept(noexcept(param<T>()(param<TS &&>()...))) -> EnableIf<!nx::type::isPointer<T>(), decltype(param<T>()(param<TS &&>()...))>
		{return (*pointer)(args...);}
	template<typename I> auto operator [] (I i) const noexcept(noexcept(param<T>()[param<I &&>()])) -> EnableIf<!nx::type::isPointer<T>(), decltype(param<T>()[param<I &&>()])>
		{return (*pointer)[static_cast<I &&>(i)];}
	template<typename... TS> auto operator () (TS && ... args) const noexcept(noexcept(param<T>()(param<TS &&>()...))) -> EnableIf<!nx::type::isPointer<T>(), decltype(param<T>()(param<TS &&>()...))>
		{return (*pointer)(static_cast<TS &&>(args)...);}
	
	// Raw pointer
	T * pointer;
};

// Open variation of unique_ptr for arrays, the inner pointer is public and freely accessable
template<typename T> struct UniquePtr<T[]>
{
	// Referenced type
	using Type = T;
	
	// Constructors & Destructors
	UniquePtr() noexcept
		: pointer(nullptr) {}
	explicit UniquePtr(T * ptr) noexcept
		: pointer(ptr) {}
	~UniquePtr() noexcept
		{delete [] pointer;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T[]> &) = delete;
	UniquePtr<T[]> & operator = (const UniquePtr<T[]> &) = delete;

	// Move operators
	UniquePtr(UniquePtr<T[]> && ptr) noexcept
		: pointer(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T[]> && ptr) noexcept
		{return this->reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr) noexcept
		: pointer(ptr.release()) {}
	template<typename U> UniquePtr<T[]> & operator = (UniquePtr<U> && ptr) noexcept
		{return this->reset(ptr.release());}

	// Operators & methods
	T * release() noexcept
		{T * ptr = pointer; pointer = nullptr; return ptr;}
	UniquePtr<T[]> & reset(T * ptr = nullptr) noexcept
		{delete [] pointer; pointer = ptr; return * this;}
	T & operator [] (size_t i) const noexcept
		{return pointer[i];}
	
	// Raw pointer
	T * pointer;
};

// Creates a new object, wrapped in a UniquePtr
template<typename T, typename ... TS> inline UniquePtr<T> make(TS && ... args)
{
	return UniquePtr<T>(new T(static_cast<TS &&>(args)...));
}

// Creates a new array, wrapped in a UniquePtr
template<typename T> inline UniquePtr<Array<T>> makeArray(size_t n)
{
	return UniquePtr<Array<T>>(Array<T>::create(n));
}

// Creates a new array from the given elements wrapped in a UniquePtr
template<typename T, typename ... TS> inline UniquePtr<Array<T>> makeArrayFrom(TS && ... list)
{
	return UniquePtr<Array<T>>(Array<T>::createFrom(list...));
}

// Close namespace "nx"
}
