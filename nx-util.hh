// Include guard
#pragma once

// Local includes
#include "nx-type.hh"
//#include "nx-meta.hh"

// Namespace "nx"
namespace nx {

// ------------------------------------------------------------ //
//		Forward declarations 
// ------------------------------------------------------------ //

// Abstract classes
template<typename T> class Iterator;

// Numeric range
template<typename T> class Range;

// Containers
template<typename T> class List;
template<typename T> class Set;
template<typename K, typename V> class Dictionary;

// Optional parameters
namespace opt {
	template<typename T> class In;
	template<typename T> class Out;
	template<typename T> class InOut;
}

// Iterators
namespace iter {
	template<typename T> class Legacy;
	template<typename T> class Virtual;
}

// ------------------------------------------------------------ //
//		Abstract classes
// ------------------------------------------------------------ //

// [CLASS] Iterator - Base class of virtual iterators
template<typename T> class Iterator : public Object
{
public:
	virtual ~Iterator() {}
	
	virtual T last() const = 0;
	
	virtual T next() = 0;
	virtual bool hasNext() const = 0;
	
	virtual T prev() = 0;
	virtual bool hasPrev() const = 0;
};


// ------------------------------------------------------------ //
//		Numeric range
// ------------------------------------------------------------ //

/**
	[CLASS] Range - Stores a linear sequence as a single convinient object
	
	For now, it can be used with the C++ foreach loop, to iterate over a range of numbers. It creates an STL compatible
	Singular iterator, so it can be used in many places, where STL iterators can be.
 */
template<typename T> class Range
{
public:
	// Legacy Iterator
	class Iter
	{
	public:
		// Direction
		const bool forward = true;
		
		// Contructors
		Iter()
			{}
		Iter(T value, T end, T step)
			: forward(step > 0), _value(rvalue(value)), _end(rvalue(end)), _step(rvalue(step)) {}
		
		// NX iterator interface
		T last() const
			{return _value;}
		T next()
			{_value += _step; return _value;}
		bool hasNext() const
			{return forward ? _value < _end : _value > _end;}
		
	private:
		// Remaining range
		T _value;
		T _end;
		T _step;
	};

	// Constructor
	Range(T start, T end, T step)
		: _start(rvalue(start)), _end(rvalue(end)), _step(rvalue(step)) {}
		
	// NX iterator methods
	inline Iter<T> iter() const
		{return Iter<T>(_start, _end, _step);}

	// STL iterator methods
	inline iter::Legacy<Iter<T>> begin() const
		{return iter::Legacy<Iter<T>>(_start, _end, _step);}
	inline iter::Legacy<Iter<T>> end() const
		{return iter::Legacy<Iter<T>>();}
	
private:
	T _start;
	T _end;
	T _step;
};

inline Range<int32_t> range(int32_t end)
	{return Range<int32_t>(0, end, 1);}
inline Range<int32_t> range(int32_t start, int32_t end, int32_t step = 1)
	{return Range<int32_t>(start, end, step);}
inline Range<int64_t> range(int64_t end)
	{return Range<int64_t>(0, end, 1);}
inline Range<int64_t> range(int64_t start, int64_t end, int64_t step = 1)
	{return Range<int64_t>(start, end, step);}

inline Range<uint32_t> range(uint32_t end)
	{return Range<uint32_t>(0, end, 1);}
inline Range<uint32_t> range(uint32_t start, uint32_t end, uint32_t step = 1)
	{return Range<uint32_t>(start, end, step);}
inline Range<uint64_t> range(uint64_t end)
	{return Range<uint64_t>(0, end, 1);}
inline Range<uint64_t> range(uint64_t start, uint64_t end, uint64_t step = 1)
	{return Range<uint64_t>(start, end, step);}

inline Range<float> range(float end)
	{return Range<float>(0.f, end, 1.f);}
inline Range<float> range(float start, float end, float step = 1.f)
	{return Range<float>(start, end, step);}
inline Range<double> range(double end)
	{return Range<double>(0.0, end, 1.0);}
inline Range<double> range(double start, double end, double step = 1.0)
	{return Range<double>(start, end, step);}

	
// ------------------------------------------------------------ //
//		Containers
// ------------------------------------------------------------ //
	
/**
	[CLASS] List - Dynamically resizable arrays
	
	Implementation:
	
	List is currently implemented with 3 fields, for storing the size, capacity, and a pointer to the array.
	
	An other option would be to store the size and the capacity in the allocated space, like in array. This, just like
	in array, would reduce the size of the structure to a single pointer.
	
	But there is a very nice property of the current implementation, that we desperately want to keep: Creating an
	empty list does not need allocate any additional memory. This means creating an empty list is VERY fast, and VERY
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
	
	// STL itertator methods
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
	Dictionary(const Dictionary & dict);
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


// ------------------------------------------------------------ //
//		Optional parameters
// ------------------------------------------------------------ //

// Namespace "nx::opt"
namespace opt {

// [CLASS] In - Optional input only parameter. Can be constructed from "nothing". (For mandatory input, use "const T &") 
template<typename T> class In
{
public:
	// Constructors
	In(const T & obj)
		: ptr(& obj) {}
	In(Nothing)
		: ptr(nullptr) {}

	// Operator
	operator bool()
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


// [CLASS] Out - Optional output only parameter. Can be constructed from "nothing". (For mandatory output, use "T &") 
template<typename T> class Out
{
	// Constructors
	Out(T & obj)
		: ptr(& obj) {}
	Out(Nothing)
		: ptr(nullptr) {}

	// Operator
	operator bool()
		{return ptr;}

	// Use parameter
	bool provided() const
		{return ptr;}
	void & set(T && val) const
		{if (ptr) (* ptr) = rvalue(val);}
	void & set(const T & val) const
		{if (ptr) (* ptr) = val;}

private:
	// Pointer to object
	const T * ptr;
};


// [CLASS] InOut - Optional input-output parameter. Can be constructed from "nothing". (For mandatory input-output, use "T &")
template<typename T> class InOut
{
	// Constructors
	InOut(T & obj)
		: ptr(& obj) {}
	InOut(Nothing)
		: ptr(nullptr) {}

	// Operator
	operator bool()
		{return ptr;}
		
	// Use parameter
	bool provided() const
		{return ptr;}
	void & set(T && val) const
		{if (ptr) (* ptr) = rvalue(val);}
	void & set(const T & val) const
		{if (ptr) (* ptr) = val;}
	const T & get(const T & def) const
		{return ptr ? * ptr : def;}

private:
	// Pointer to object
	const T * ptr;
};


// Close namespace "nx::opt"
}


// ------------------------------------------------------------ //
//		Iterators 
// ------------------------------------------------------------ //

// Namespace "nx::iter"
namespace iter {
	
// [CLASS] Legacy - Coverts singular iterators to legacy C++ iterators, that will work with foreach
template<typename T> class Legacy
{
public:
	// Element type
	using Type = decltype(param<T>().last());

	// Constructor
	template<typename... TS> Legacy(TS && ... args)
		: iterator(forward<TS>(args) ...) {}
		
	// Legacy C++ iterator interface
	Type operator * () const
		{return iterator.last();}
	Legacy & operator ++ ()
		{iterator.next(); return * this;}
	bool operator != (const Legacy &)
		{return iterator.hasNext();}
	
private:
	T iterator;
};

// [CLASS] Virtual - Wraps a simple iterator, and creates one with a virtual interface
template<typename T> class Virtual : public Iterator<decltype(param<T>().last())>
{
public:
	// Element type
	using Type = decltype(param<T>().last());
	
	// Constructor
	template<typename... TS> Virtual(TS && ... args)
		: iterator(forward<TS>(args)...) {}
	
	// Iterator methods
	Type last() const override
		{return iterator.last();}

	Type next() override
		{return iterator.next();}
	bool hasNext() override
		{return iterator.hasNext();}
		
	Type prev() override
		{return iterator.prev();}
	bool hasPrev() override
		{return iterator.hasPrev();}

private:
	T iterator;
}
	
// Close namespace "nx::iter"
}


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
