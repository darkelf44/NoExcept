// Include guard
#pragma once

// Standard includes
#include <stdint.h>

// Local includes
#include "nx-new.hh"

// Namespace "nx"
namespace nx {
	
using byte = unsigned char;
using uint = unsigned int;

// Base class for reference type classes (as oppposed to value type ones)
class Object
{
public:
	inline Object();
	inline virtual ~Object();
	
	Object(const Object &) = delete;
	Object & operator = (const Object &) = delete;
};

Object::Object() = default;
Object::~Object() = default;

// Open variation of unique_ptr, the inner pointer is public and freely accessable
template<typename T> struct UniquePtr
{
	// Referenced Type
	using Type = T;
	
	// Constructors & destructors
	UniquePtr()
		: data(nullptr) {}
	explicit UniquePtr(T * ptr)
		: data(ptr) {}
	~UniquePtr()
		{delete data;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T> &) = delete;
	UniquePtr<T> & operator = (const UniquePtr<T> &) = delete;
	
	// Move operators
	UniquePtr(UniquePtr<T> && ptr)
		: data(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T> && ptr)
		{return this->reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr)
		: data(ptr.release()) {}
	template<typename U> UniquePtr<T> & operator = (UniquePtr<U> && ptr)
		{return this->reset(ptr.release());}
	
	// Operators & methods
	T * release()
		{T * ptr = data; data = nullptr; return ptr;}
	UniquePtr<T> & reset(T * ptr = nullptr)
		{delete data; data = ptr; return * this;}
	T * operator -> () const
		{return data;}
	T & operator * () const
		{return * data;}
	T & access() const
		{return * data;}
	
	// Referenced Data
	T * data;
};

// Open variation of unique_ptr for arrays, the inner pointer is public and freely accessable
template<typename T> struct UniquePtr<T[]>
{
	// Referenced Type
	using Type = T;
	
	// Constructors & Destructors
	UniquePtr()
		: data(nullptr) {}
	explicit UniquePtr(T * ptr)
		: data(ptr) {}
	~UniquePtr()
		{delete [] data;}
	
	// Copy operators deleted
	UniquePtr(const UniquePtr<T[]> &) = delete;
	UniquePtr<T[]> & operator = (const UniquePtr<T[]> &) = delete;

	// Move operators
	UniquePtr(UniquePtr<T[]> && ptr)
		: data(ptr.release()) {}
	UniquePtr<T> & operator = (UniquePtr<T[]> && ptr)
		{return this->reset(ptr.release());}

	// Template move operators
	template<typename U> UniquePtr(UniquePtr<U> && ptr)
		: data(ptr.release()) {}
	template<typename U> UniquePtr<T[]> & operator = (UniquePtr<U> && ptr)
		{return this->reset(ptr.release());}

	// Operators & Methods
	T * release()
		{T * ptr = data; data = nullptr; return ptr;}
	UniquePtr<T[]> & reset(T * ptr = nullptr)
		{delete [] data; data = ptr; return * this;}
	T & operator [] (size_t i) const
		{return data[i];}
	
	// Referenced Data
	T * data;
};

// creates a new object, wrapped in a UniquePtr
template<typename T, typename ... TS> inline UniquePtr<T> make(TS ... args)
{
	return UniquePtr<T>(new T(static_cast<TS>(args)...));
}

// Array class
template<typename T> struct Array
{
	// Types
	using Element = T;
	
	// Fields
	const size_t length;
	T data[0];	// Yes, I know not vaild in C++. Whatever! It should be.
	
	// Default constructor deleted
	Array() = delete;
	
	// Allocator
	static Array<T> * create(size_t n);
	// Destructor
	~Array();
	
	// Copy operators deleted
	Array(const Array &) = delete;
	Array<T> & operator = (const Array &) = delete;	
};

template<typename T> Array<T> * Array<T>::create(size_t n)
{
	// allocate array
	Array<T> * array = nx::type::alloc<Array<T>>(sizeof(Array<T>) + n * sizeof(T));
	// set length
	const_cast<size_t &>(array->length) = n;
	// create elements
	nx::type::createArrayAt<T>(array->data, n);
	// return array
	return array;
}

template<typename T> Array<T>::~Array()
{
	// destroy elements
	nx::type::destroyArrayAt<T>(this->data, this->length);
}

// creates a new array, wrapped in a UniquePtr
template<typename T> inline UniquePtr<Array<T>> makeArray(size_t n)
{
	return UniquePtr<Array<T>>(Array<T>::create(n));
}

// Close namespace "nx"
}
