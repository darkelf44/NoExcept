// Include guard
#pragma once

// Local includes
#include "nx-type.hh"

// Namespace "nx"
namespace nx {
	
// List class - Dynamically resizable arrays -- TODO: concurrent, lock free, virtual interface
template<typename T> class List: public Object
{
public:
	// Element type
	using Type = T;

	// Constructors & destructors
	List() noexcept
		: n(0), m(0), items(nullptr) {}
	List(List && list) noexcept
		: n(list.n), m(list.m), items(list.items) {list.n = 0; list.m = 0; list.items = nullptr;}
	List(const List & list);
	~List() override
		{nx::type::destroyArrayAt(items, n); nx::type::free(items);}
		
	// Size & capacity
	inline size_t size() const
		{return n;}
	inline size_t capacity() const
		{return m;}
		
	void resize(size_t n);
	void reserve(size_t n);
	void compact();
	
	// Data
	inline T * data()
		{return items;}
	inline const T * data() const
		{return items;}
	
	// Getters
	inline const T & get(size_t i, const T & def = T())
		{return (0 <= i) && (i < n) ? items[i] : def;}
		
	// Methods
	void append(T && item);
	void append(const T & item);
	
	void extend(List && list);
	void extend(const List & list);
	
	// Itertator methods
	inline T * begin() noexcept
		{return items;}
	inline const T * begin() const noexcept
		{return items;}
	inline T * end() noexcept
		{return items + n;}
	inline const T * end() const noexcept
		{return items + n;}
	
	// Copy & move
	List & operator = (List && list) noexcept;
	List & operator = (const List & list);
	
	// Operators
	inline T & operator [] (size_t i) noexcept
		{return items[i];}
	inline const T & operator [] (size_t i) const noexcept
		{return items[i];}
		
	inline List & operator += (List && list)
		{extend(static_cast<List &&>(list)); return * this;}
	inline List & operator += (const List & list)
		{extend(list); return * this;}
		
private:
	size_t n;
	size_t m;
	T *    items;
};

// Set class - TODO: concurrent, lock free, virtual interface
template<typename T> class Set: public Object
{
public:
private:
};


// Dictionary class - Associative array, map, or dictionary -- TODO: concurrect, lock free, virtual interface
template<typename K, typename V> class Dictionary: public Object
{
public:


private:
	Object * indexArray;
	
};

// ------------------------------------------------------------ //
//		List Implementation
// ------------------------------------------------------------ //

template<typename T> List<T>::List(const List<T> & list)
	: nx::Object(), n(0), m(0), items(nullptr)
{
	if (list.n > 0)
	{
		reserve(list.n);
		nx::type::createArrayAtByCopy(items, list.items, list.n);
		n = list.n;
	}
}

template<typename T> void List<T>::resize(size_t size)
{
	reserve(size);
	if (size > n)
	{
		nx::type::createArrayAt(items + n, size - n);
		n = size;
	}
	if (size < n)
	{
		nx::type::destroyArrayAt(items + size, n - size);
		n = size;
	}
}

template<typename T> void List<T>::reserve(size_t size)
{
	if (size > m)
	{
		List list;
		list.items = nx::type::alloc<T>(sizeof(T) * size);
		nx::type::confirm(list.items);
		
		if (items)
		{
			nx::type::createArrayAtByMove(list.items, items, n);
			nx::type::destroyArrayAt(items, n);
			nx::type::free(items);
		}
		
		m = size;
		items = list.items;
		list.items = nullptr;
	}
}

template<typename T> void List<T>::compact()
{
	if (items)
	{
		if (n > 0)
		{
			List list;
			list.items = nx::type::alloc<T>(sizeof(T) * n);
			nx::type::confirm(list.items);
			
			nx::type::createArrayAtByMove(list.items, items, n);
			nx::type::destroyArrayAt(items, n);
			nx::type::free(items);
			
			m = n;
			items = list.items;
			list.items = nullptr;
		}
		else
		{
			m = 0;
			items = nullptr;
		}
	}
}

template<typename T> void List<T>::append(T && item)
{
	if (n + 1 > m)
	{
		// Exponential growth with a 1.5 base (starting from 16)
		m = m > 16 ? m : 16; 
		reserve(m + (m >> 1));
	}
	
	nx::type::createAt(items + n, static_cast<T &&>(item));
	n ++;
}

template<typename T> void List<T>::append(const T & item)
{
	if (n + 1 > m)
	{
		// Exponential growth with a 1.5 base (starting from 16)
		m = m > 16 ? m : 16;
		reserve(m + (m >> 1));
	}
	
	nx::type::createAt(items + n, item);
	n ++;
}

template<typename T> void List<T>::extend(List<T> && list)
{
	if (list.n > 0)
	{
		size_t x = m > 16 ? m : 16;
		while (n + list.n > x)
		{
			x = x + (x >> 1);
		}
		reserve(x);

		nx::type::createArrayAtByMove(items + n, list.items, list.n);
		n += list.n;
	}
}

template<typename T> void List<T>::extend(const List<T> & list)
{
	if (list.n > 0)
	{
		size_t x = m > 16 ? m : 16;
		while (n + list.n > x)
		{
			x = x + (x >> 1);
		}
		reserve(x);

		nx::type::createArrayAtByCopy(items + n, list.items, list.n);
		n += list.n;
	}
}

template<typename T> List<T> & List<T>::operator = (List<T> && list) noexcept
{
	n = list.n; list.n = 0;
	m = list.m; list.m = 0;
	items = list.items; list.items = nullptr;
	return * this;
}

template<typename T> List<T> & List<T>::operator = (const List<T> & list)
{
	resize(0);
	extend(list);
	return * this;
}

// ------------------------------------------------------------ //
//		Set Implementation
// ------------------------------------------------------------ //

// ------------------------------------------------------------ //
//		Dictionary Implementation
// ------------------------------------------------------------ //

// Close namespace "nx"
}
