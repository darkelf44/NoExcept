// Include guard
#pragma once

// Local includes
#include "nx-new.hh"

// Disable false warnings in header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Namespace "nx"
namespace nx {
	
// Multi class - Multiple consecutive elements of the same type, or a fixed size array type
template<typename T, size_t N> struct Multi
{
	// Element type
	using Type = T;

	// Static fields
	static constexpr size_t length = N;
	
	// Fields
	T data[N];
	
	// Operators & methods
	inline T * begin()
		{return data;}
	inline const T * begin() const
		{return data;}
	inline T * end()
		{return data + N;}
	inline const  T * end() const
		{return data + N;}
	inline T & operator [] (size_t i)
		{return data[i];}
	inline const T & operator [] (size_t i) const
		{return data[i];}		
};

// Array class - Java like array type (dynamically allocated on the heap, cannot be resized)
template<typename T> class Array: public Object
{
public:
	// Element type
	using Type = T;
	
	// Fields
	const size_t length;
	T data[0];	// Yes, I know, not valid in C++. Whatever! It should be.
	
	// Destructor
	~Array() override
		{nx::type::destroyArrayAt<T>(data, length);}
		
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
		
	// Allocators
	static Array<T> * create(size_t n);	
	template<typename... TS> Array<T> * createFrom(TS && ... list);
		
private:
	// Constructors
	Array(size_t n)
		: length(n) {}
};

// Array allocator - creates a new array
template<typename T> Array<T> * Array<T>::create(size_t n)
{
	// Allocate array with custom size
	auto * array = new (sizeof(Array<T>) + n * sizeof(T)) Array<T>(n);
	// Create array elements
	nx::type::createArrayAt<T>(array->data, n);
	// Return array
	return array;
}

template<typename T> template<typename... TS> Array<T> * Array<T>::createFrom(TS && ... list)
{
	constexpr size_t n = sizeof...(list);
	// Allocate array with custom size
	auto * array = new (sizeof(Array<T>) + n * sizeof(T)) Array<T>(n);
	// Create array elements
	nx::type::createArrayAtFrom<T>(array->data, list...);
	// Return array
	return array;
}

// Close namespace "nx"
}

// Reset warnings
#pragma GCC diagnostic pop
