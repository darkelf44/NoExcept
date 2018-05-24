// Include guard
#pragma once

// Local includes
#include "nx-type.hh"
//#include "nx-meta.hh"

// Namespace "nx"
namespace nx {

// [CLASS] In - Optional input only parameter. Can be constructed from "nothing". (For mandatory input, just use "const T &") 
template<typename T> class In
{
public:
	// Constructors
	In(const T & obj)
		: ptr(& obj) {}
	In(Nothing)
		: ptr(nullptr) {}
		
	// Operator
	bool operator bool()
		{return ptr;}
	
	// Use parameter
	bool provided() const
		{return ptr;}
	const T & get(const T & def) const
		{return ptr ? * ptr : def;}

private:
	// Pointer to object
	const T * ptr;
};

// [CLASS] Out - Optional output only parameter. Can be constructed from "nothing". (For mandatory output, just use "T &") 
template<typename T> class Out
{
	// Constructors
	Out(T & obj)
		: ptr(& obj) {}
	Out(Nothing)
		: ptr(nullptr) {}
		
	// Operator
	bool operator bool()
		{return ptr;}
	
	// Use parameter
	bool provided() const
		{return ptr;}
	Out & set(T && val) const
		{if (ptr) (* ptr) = rvalue(val); return * this;}
	Out & set(const T & val) const
		{if (ptr) (* ptr) = val; return * this;}
	
private:
	// Pointer to object
	const T * ptr;
};

// [CLASS] InOut - Optional input output parameter. Can be constructed from "nothing". 
template<typename T> class InOut
{
};

/**
	[CLASS] List - Dynamically resizable arrays
	
	Implementation:
	
	List is currently implemented with 3 fields, for storing the size, capacity, and a pointer to the array.
	
	An other option would be to store the size and the capacity in the allocated space, like in array. This, just like
	in array, would reduce the size of the structure to a single pointer.
	
	But there is a very nice property of the current implementation, that we desperately want to keep: Creating an
	empty list does not need allocate any additional memory. This means creating empty lists are VERY fast, and VERY
	cheap to do so.
	
	To achive the same with the single pointer implementation, it would need additional branching to determine the
	size and capacity of the list, and also introduces some extra indirection. It's a small tradeoff, but a tradeoff
	nevertheless.
 */
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
	inline size_t size() const noexcept
		{return n;}
	inline size_t capacity() const noexcept
		{return m;}
		
	void resize(size_t n);
	void reserve(size_t n);
	void compact();
	
	// Data
	inline T * data() noexcept
		{return items;}
	inline const T * data() const noexcept
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

// Operations on List
template<typename T> void swap(List<T> & left, List<T> & right) noexcept;


// Set class - TODO: concurrent, lock free, virtual interface
template<typename T> class Set: public Object
{
public:
private:
};


// Dictionary class - Associative array, map, or dictionary -- TODO: concurrect, lock free, virtual interface
/**
	[CLASS] Dictionary - Associative array (also sometimes called "map")
	
	Implementation:
	
	Dictionary is implemented using two arrays. The first contains nodes, these nodes combine the key, the hash of the
	key, and the associated value. The second is the index table, this is a hash table, that contains indices into the
	node list.
	
	The index table can contain 8, 16, 32 or 64 bit indices, depending on its size. If it's current size is less than
	128, 32768 or 2^31 bytes, it will use 8, 16 or 32 bit indices respectively, otherwise it' use 64bit indices
	(but by then just the index table will take up 8 GB of memory).
 */
template<typename K, typename V> class Dictionary: public Object
{
public:
	// Entry type
	using Entry = Pair<K, V>;

	// Constructors & destructors
	Dictionary() noexcept
		{}
	Dictionary(Dictionary && dict) noexcept
		: config(dict.config), n(dict.n), m(dict.m), nodes(dict.nodes), table(dict.table) {dict.nodes = nullptr; dict.table = nullptr;}
	Dictionary(const Dictionary & dict)
		: config(dict.config), n(dict.n), m(dict.m), nodes(dict.nodes.clone()), table(dict.table.clone()) {}
	~Dictionary() override
		{delete nodes; delete table;}
		
	// Size & capacity
	size_t size() const noexcept
		{return n;}
	size_t capacity() const noexcept
		{return m;}

private:
	// Node type
	struct Node;
	
	// Configuration
	struct
	{
		float growth_rate = 2.0;
		float shrink_rate = 0.5;
		float max_fill_rate = 0.75;
		float min_fill_rate = 0.25;
	}
	config;
	
	// size and capacity
	size_t n = 0;
	size_t m = 0;
	
	// Node list and index
	Array<Node> * nodes = nullptr;
	Array<byte> * table = nullptr;
};

// Tuple class - The real, generic version of the tuple COMING SOON!
template<typename ... TS> struct Tuple
{
};

// ------------------------------------------------------------ //
//		Utility functions
// ------------------------------------------------------------ //

template<typename T> void swap(T & left, T & right) noexcept(nx::type::isCreateNoexcept<T, T>() && nx::type::isMoveNoexcept<T>())
{
	T l = rvalue(left);
	T r = rvalue(right);
	left = rvalue(r);
	right = rvalue(l);
}

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
	swap(* this, list);
	return * this;
}

template<typename T> List<T> & List<T>::operator = (const List<T> & list)
{
	// TODO: use operator = here
	return * this;
}

template<typename T> void swap(List<T> & left, List<T> & right) noexcept
{
	swap(left.n, right.n);
	swap(left.m, right.m);
	swap(left.items, right.items);
}


// ------------------------------------------------------------ //
//		Set Implementation
// ------------------------------------------------------------ //

// ------------------------------------------------------------ //
//		Dictionary Implementation
// ------------------------------------------------------------ //

/**
	[CLASS] Dictionary Node - Stores dictionary entries with their hashes
	
	Implementation:
	
	The hash value 0 is remapped, and used to indicate empty nodes. Empty nodes are nodes, without a constructed entry
	inside them. These nodes are managed by the 
 */
template<typename K, typename V> struct Dictionary<K, V>::Node
{
	// Fields
	uintptr_t hash;
	union
	{
		Entry entry;
	};
	
	// Constructors & destructors
	Node() noexcept
		: hash(0) {}
	~Node()
		{if (hash != 0) destroy();}
	
	// Create and destroy the entry
	template<typename ... TS> void create(TS && ... args) noexcept(nx::type::isCreateNoexcept<Entry, TS ...>())
		{nx::type::createAt(& entry, forward<TS &&>(args) ...);}
	void destroy() noexcept(nx::type::isDestroyNoexcept<Entry>())
		{nx::type::destroyAt(& entry);}
		
	// Copy and move isn't needed
	Node & operator = (Node && node) = delete;
	Node & operator = (const Node & node) = delete;
};

// Close namespace "nx"
}
