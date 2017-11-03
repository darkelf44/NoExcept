// Include guard
#pragma once

// Local includes
#include "nx-new.hh"

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
	T data[0];	// Yes, I know not vaild in C++. Whatever! It should be.
	
	// Allocator
	static Array<T> * create(size_t n);
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
		
private:
	// Constructor
	Array(size_t n)
		: length(n) {}
};

// Array allocator - Rather that splitting the allocation logic, and creating a private constructor, I simply use 
template<typename T> Array<T> * Array<T>::create(size_t n)
{
	// Allocate array with custom size
	auto * array = new (sizeof(Array<T>) + n * sizeof(T)) Array<T>(n);
	// Create array elements
	nx::type::createArrayAt<T>(array->data, n);
	// Return array
	return array;
}

// List class - Dynamically resizable arrays
template<typename T> class List: public Object
{
public:
private:
	Array<T> * array;
};

// Dictionary class - Associative array, map, or dictionary
template<typename K, typename V> class Dictionary: public Object
{
public:
	// Entry type
	class Entry
	{
		K key;
		V value;
	};
	
	

private:
	K * keys;
	V * values;
	size_t * hashes;
	size_t * indices;
};

// Close namespace "nx"
}
