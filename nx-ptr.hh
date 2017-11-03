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
	UniquePtr()
		: pointer(nullptr) {}
	explicit UniquePtr(T * ptr)
		: pointer(ptr) {}
	~UniquePtr()
		{delete pointer;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T> &) = delete;
	UniquePtr<T> & operator = (const UniquePtr<T> &) = delete;
	
	// Move operators
	UniquePtr(UniquePtr<T> && ptr)
		: pointer(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T> && ptr)
		{return reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr)
		: pointer(ptr.release()) {}
	template<typename U> UniquePtr<T> & operator = (UniquePtr<U> && ptr)
		{return reset(ptr.release());}
	
	// Operators & methods
	T * release()
		{T * ptr = pointer; pointer = nullptr; return ptr;}
	UniquePtr<T> & reset(T * ptr = nullptr)
		{delete pointer; pointer = ptr; return * this;}
	T * operator -> () const
		{return pointer;}
	T & operator * () const
		{return * pointer;}
	T & access() const
		{return * pointer;}
	
	// Raw pointer
	T * pointer;
};

// Open variation of unique_ptr for arrays, the inner pointer is public and freely accessable
template<typename T> struct UniquePtr<T[]>
{
	// Referenced type
	using Type = T;
	
	// Constructors & Destructors
	UniquePtr()
		: pointer(nullptr) {}
	explicit UniquePtr(T * ptr)
		: pointer(ptr) {}
	~UniquePtr()
		{delete [] pointer;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T[]> &) = delete;
	UniquePtr<T[]> & operator = (const UniquePtr<T[]> &) = delete;

	// Move operators
	UniquePtr(UniquePtr<T[]> && ptr)
		: pointer(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T[]> && ptr)
		{return this->reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr)
		: pointer(ptr.release()) {}
	template<typename U> UniquePtr<T[]> & operator = (UniquePtr<U> && ptr)
		{return this->reset(ptr.release());}

	// Operators & methods
	T * release()
		{T * ptr = pointer; pointer = nullptr; return ptr;}
	UniquePtr<T[]> & reset(T * ptr = nullptr)
		{delete [] pointer; pointer = ptr; return * this;}
	T & operator [] (size_t i) const
		{return pointer[i];}
	
	// Raw pointer
	T * pointer;
};

// creates a new object, wrapped in a UniquePtr
template<typename T, typename ... TS> inline UniquePtr<T> make(TS ... args)
{
	return UniquePtr<T>(new T(static_cast<TS>(args)...));
}

// creates a new array, wrapped in a UniquePtr
template<typename T> inline UniquePtr<Array<T>> makeArray(size_t n)
{
	return UniquePtr<Array<T>>(Array<T>::create(n));
}
	
// Close namespace "nx"
}
