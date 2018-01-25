// Include guard
#pragma once

// Local includes
#include "nx-type.hh"
#include "nx-ptr.hh"

// Namespace "nx"
namespace nx {
	
// List class - Dynamically resizable arrays -- TODO: concurrent, lock free, virtual interface
template<typename T> class List: public Object
{
public:
	// Element type
	using Type = T;

	// Constructors & destructors
	List()
		: n(0), m(0), data(nullptr) {}
	~List() override
		{ nx::type::destroyArrayAt(data, n); nx::type::free(data); }
		
	// Size and capacity	
	inline void size() const
		{return n;}
	inline void capacity() const
		{return m;}
		
	void resize(size_t n)
	void reserve(size_t n);
	
	inline T * begin()
		{return data;}
	inline const T * begin() const
		{return data;}
	inline T * end()
		{return data + n;}
	inline const T * end() const
		{return data + n;}

private:
	size_t n;
	size_t m;
	T * data;
};

template<typename T> void List<T>::resize(size_t size)
{
	reserve(size);
	if (size > n)
	{
		nx::type::createArrayAt(data + n, size - n);
		n = size;
	}
	if (size < n)
	{
		nx::type::destroyArrayAt(data + size, n - size);
		n = size;
	}
}

template<typename T> void List<T>::reserve(size_t size)
{
	if (size > m)
	{
		UniquePtr<T> list = nx::type::alloc(sizeof(T) * size);
		nx::type::confirm(list.get());
		
		if (data)
		{
			nx::type::createArrayAtByMove(list.data, data, n);
			nx::type::destroyArrayAt(data, n);
		}
		
		data = list.release();
		m = size;
	}
}


// Set class -
template<typename T> class Set: public Object
{
};


// Dictionary class - Associative array, map, or dictionary -- TODO: concurrect, lock free, virtual interface
template<typename K, typename V> class Dictionary: public Object
{
};

// Close namespace "nx"
}
