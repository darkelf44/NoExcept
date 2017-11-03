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

// Object class - Base of role like types (as opposed value like types)
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

// Array class - Java like array type (dynamically allocated on the heap, cannot be resized)
template<typename T> struct Array
{
	// Element type
	using Type = T;
	
	// Fields
	const size_t length;
	T data[0];	// Yes, I know not vaild in C++. Whatever! It should be.
	
	// Default constructor deleted
	Array() = delete;
	
	// Allocator
	static Array<T> * create(size_t n);
	// Destructor
	~Array()
		{nx::type::destroyArrayAt<T>(data, length);}
	
	// Copy operators deleted
	Array(const Array &) = delete;
	Array<T> & operator = (const Array &) = delete;
	
	// Operators & methods
	inline T * begin()
		{return data;}
	inline const T * begin() const
		{return data;}
	inline T * end()
		{return data + length;}
	inline const  T * end() const
		{return data + length;}
	inline T & operator [] (size_t i)
		{return data[i];}
	inline const T & operator [] (size_t i) const
		{return data[i];}		
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
