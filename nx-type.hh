// Include guard
#pragma once

// Local includes
#include "nx-new.hh"

// Disable false warnings in header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Namespace "nx"
namespace nx {
	
// Namespace "nx::meta" - Meta programming resources
namespace meta {
	
// Namespace "nx::meta::proto" - Naked C++ meta functions, without wrappers
namespace proto {
	
// [META FUNCTION] If - Conditional expression
template<bool b, typename T, typename F> struct If;
template<typename T, typename F> struct If<true, T, F> { using Result = T; };
template<typename T, typename F> struct If<false, T, F> { using Result = F; };

// [META FUNCTION] Select - Select from a parameter list
template<size_t i, typename ... TS> struct Select;
template<typename T, typename ... TS> struct Select<0, T, TS ...> { using Result = T; };
template<size_t i, typename T, typename ... TS> struct Select<i, T, TS ...> { using Result = typename Select<i - 1, TS ...>::Result; };

// close namespace "nx::meta::proto"
}

template<bool b, typename T, typename F> using If = typename proto::If<b, T, F>::Result;
template<size_t i, typename ... TS> using Select = typename proto::Select<i, TS ...>::Result;


// close namespace "nx::meta"
}

// Namespace "nx::type" - Type system related functionality
namespace type {

// Namespace "nx::type::proto" - Naked C++ meta functions, without wrappers
namespace proto {

// [META FUNCTION] Id - Identity function, returns the given type. Useful for preventing type deduction
template<typename T> struct Id { using Result = T; };

// [META FUNCTION] EnableIf - Enables template function, if the condition is true
template<bool b, typename T> struct EnableIf {};
template<typename T> struct EnableIf<true, T> { using Result = T; };


// [META FUNCTION] IsConstant - True, if the type is const qualified
template<typename T> struct IsConstant { static constexpr bool result = false; };
template<typename T> struct IsConstant<const T> { static constexpr bool result = true; };

// [META FUNCTION] IsVolatile - True, if the type is volatile qualified
template<typename T> struct IsVolatile { static constexpr bool result = false; };
template<typename T> struct IsVolatile<volatile T> { static constexpr bool result = true; };

// [META FUNCTION] IsVolatile - True, if the type is const or volatile qualified
template<typename T> struct IsQualified { static constexpr bool result = IsConstant<T>::result || IsVolatile<T>::result; };


// [META FUNCTION] IsPointer - True, if the type is a pointer
template<typename T> struct IsPointer { static constexpr bool result = false; };
template<typename T> struct IsPointer<T *> { static constexpr bool result = true; };

// [META FUNCTION] IsReference - True, if the type is a normal, lvalue reference
template<typename T> struct IsReference { static constexpr bool result = false; };
template<typename T> struct IsReference<T &> { static constexpr bool result = true; };

// [META FUNCTION] IsLValueReference - True, if the type is a normal, lvalue reference
template<typename T> struct IsLValueReference { static constexpr bool result = IsReference<T>::result; };

// [META FUNCTION] IsRValueReference - True, if the type is an rvalue reference
template<typename T> struct IsRValueReference { static constexpr bool result = false; };
template<typename T> struct IsRValueReference<T &&> { static constexpr bool result = true; };

// [META FUNCTION] IsAnyReference - True if the type if either type of reference
template<typename T> struct IsAnyReference { static constexpr bool result = IsReference<T>::result || IsRValueReference<T>::result; };


// [META FUNCTION] RemoveConstant
template<typename T> struct RemoveConstant { using Result = T; };
template<typename T> struct RemoveConstant<const T> { using Result = T; };

// [META FUNCTION] RemoveVolatile
template<typename T> struct RemoveVolatile { using Result = T; };
template<typename T> struct RemoveVolatile<volatile T> { using Result = T; };

// [META FUNCTION] RemoveQualifiers
template<typename T> struct RemoveQualifiers { using Result = T; };
template<typename T> struct RemoveQualifiers<const T> { using Result = T; };
template<typename T> struct RemoveQualifiers<volatile T> { using Result = T; };
template<typename T> struct RemoveQualifiers<const volatile T> { using Result = T; };


// [META FUNCTION] RemovePointer
template<typename T> struct RemovePointer { using Result = T; };
template<typename T> struct RemovePointer<T *> { using Result = T; };

// [META FUNCTION] RemoveReference
template<typename T> struct RemoveReference { using Result = T; };
template<typename T> struct RemoveReference<T &> { using Result = T; };

// [META FUNCTION] RemoveLValueReference
template<typename T> struct RemoveLValueReference { using Result = typename RemoveReference<T>::Result; };

// [META FUNCTION] RemoveReference
template<typename T> struct RemoveRValueReference { using Result = T; };
template<typename T> struct RemoveRValueReference<T &&> { using Result = T; };

// [META FUNCTION] RemoveAnyReference
template<typename T> struct RemoveAnyReference { using Result = T; };
template<typename T> struct RemoveAnyReference<T &> { using Result = T; };
template<typename T> struct RemoveAnyReference<T &&> { using Result = T; };

// [META FUNCTION] Strip - Remove all qualifiers and references from the type (eg. "const volatile int &&" -> "int")
template<typename T> struct Strip { using Result = typename RemoveQualifiers<typename RemoveAnyReference<T>::Result>::Result; };

// Close namespace "nx::type::proto"
}

// [FUNCTION] typeid - returns a unique integer id for a type
template<typename T> inline uintptr_t typeid()
	{ static void * id; return reinterpret_cast<uintptr_t>(&id); }

template<typename T> using Id = typename proto::Id<T>::Result;
template<bool b, typename T> using EnableIf = typename proto::EnableIf<b, T>::Result;

template<typename T> inline constexpr bool isConstant()
	{ return proto::IsConstant<T>::result; }
template<typename T> inline constexpr bool isVolatile()
	{ return proto::IsVolatile<T>::result; }
template<typename T> inline constexpr bool isQualified()
	{ return proto::IsConstant<T>::result || proto::IsVolatile<T>::result; }
	
template<typename T> inline constexpr bool isPointer()
	{ return proto::IsPointer<T>::result; }
template<typename T> inline constexpr bool isReference()
	{ return proto::IsReference<T>::result; }
template<typename T> inline constexpr bool isLValueReference()
	{ return proto::IsReference<T>::result; }
template<typename T> inline constexpr bool isRValueReference()
	{ return proto::IsRValueReference<T>::result; }
template<typename T> inline constexpr bool isAnyReference()
	{ return proto::IsReference<T>::result || proto::IsRValueReference<T>::result; }

template<typename T> using RemoveConstant = typename proto::RemoveConstant<T>::Result;
template<typename T> using RemoveVolatile = typename proto::RemoveVolatile<T>::Result;
template<typename T> using RemoveQualifiers = typename proto::RemoveQualifiers<T>::Result;

template<typename T> using RemovePointer = typename proto::RemovePointer<T>::Result;
template<typename T> using RemoveReference = typename proto::RemoveReference<T>::Result;
template<typename T> using RemoveLValueReference = typename proto::RemoveReference<T>::Result;
template<typename T> using RemoveRValueReference = typename proto::RemoveRValueReference<T>::Result;
template<typename T> using RemoveAnyReference = typename proto::RemoveAnyReference<T>::Result;

template<typename T> using Strip = typename RemoveQualifiers<typename RemoveAnyReference<T>::Result>::Result;


// Close namespace "nx::type"	
}

// Add "EnableIf" to the "nx" namespace
using nx::type::EnableIf;

// Add "typeid" to the "nx" namespace
using nx::type::typeid;

// [FUNCTION] lvalue - Turns anything into an rvalue (Hey, it can be useful)
template<typename T> constexpr nx::type::RemoveAnyReference<T> & lvalue(T && value)
	{ return static_cast<T &>(value); }
// [FUNCTION] rvalue - Same as std::move, turns anything into an lvalue
template<typename T> constexpr nx::type::RemoveAnyReference<T> && rvalue(T && value)
	{ return static_cast<T &&>(value); }
// [FUNCTION] forward - Same as std::forward, explicit cast to either rvalue or lvalue
template<typename T> constexpr T && forward(nx::type::RemoveAnyReference<T> & value)
	{ return static_cast<T &&>(value); }

// ------------------------------------------------------------ //
//		Bits auxiliary
// ------------------------------------------------------------ //

// [FUNCTION] Rotate left - Fundamental bitwise operation, that is missing from C++
inline constexpr uint32_t rotateBitsLeft(uint32_t x, uint32_t r)
	{return (x << r) | (x >> (32 - r));}
inline constexpr uint64_t rotateBitsLeft(uint64_t x, uint64_t r)
	{return (x << r) | (x >> (64 - r));}

// [FUNCTION] Rotate rigth - Fundamental bitwise operation, that is missing from C++
inline constexpr uint32_t rotateBitsRight(uint32_t x, uint32_t r)
	{return (x >> r) | (x << (32 - r));}
inline constexpr uint64_t rotateBitsRight(uint64_t x, uint64_t r)
	{return (x >> r) | (x << (64 - r));}

// ------------------------------------------------------------ //
//		Fundamental classes
// ------------------------------------------------------------ //

// Multi class - Multiple consecutive elements of the same type, or a fixed size array type
template<typename T, size_t N> struct Multi
{
	// Element type
	using Type = T;

	// Static fields
	static constexpr size_t length = N;
	
	// Fields
	T data[N];
	
	// Iterators - mostly for foreach
	inline T * begin()
		{return data;}
	inline const T * begin() const
		{return data;}
	inline T * end()
		{return data + N;}
	inline const  T * end() const
		{return data + N;}
		
	// Operators & methods
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
		
	// Iterators - mostly for foreach
	inline T * begin()
		{return data;}
	inline const T * begin() const
		{return data;}
	inline T * end()
		{return data + length;}
	inline const  T * end() const
		{return data + length;}
	
	// Operators & methods
	inline T & operator [] (size_t i)
		{return data[i];}
	inline const T & operator [] (size_t i) const
		{return data[i];}
	
	// Clone
	Array * clone() const
		{return create(*this);}

	// Allocators
	static Array * create(size_t n);
	template<typename U> static Array * create(const Array<U> * array);
	template<typename... TS> static Array * createFrom(TS && ... list);
	
	// Fill arrays
	static void fill(Array & array, const T & item);
	static void fill(Array & array, size_t off, size_t len, const T & item);
	
	// Copy arrays
	static void copy(const Array & src, Array & dest, size_t len);
	static void copy(const Array & src, size_t src_idx, Array & dest, size_t dest_idx, size_t len);
	
private:
	// Constructors
	Array(size_t n)
		: length(n) {}
};

// Tuple class - only 2, 3 and 4 element tuples are defined here. For generic tuples include <nx-util.hh>
template<typename ... TS> struct Tuple;

// Special tuples
template<typename X, typename Y> using Pair = Tuple<X, Y>;
template<typename X, typename Y, typename Z> using Trio = Tuple<X, Y, Z>;
template<typename X, typename Y, typename Z, typename W> using Quad = Tuple<X, Y, Z, W>;

// Pair class - Generic two element structure
template<typename X, typename Y> struct Tuple<X, Y>
{
	// Fields
	X first;
	Y second;
	
	// Constructors
	Tuple() = default;
	Tuple(Tuple &&) = default;
	Tuple(const Tuple &) = default;
	
	template<typename T1, typename T2> Tuple(Tuple<T1, T2> && tuple)
		: first(rvalue(tuple.first)), second(rvalue(tuple.second)) {}
	template<typename T1, typename T2> Tuple(const Tuple<T1, T2> & tuple)
		: first(tuple.first), second(tuple.second) {}
	
	template<typename T1, typename T2> Tuple(T1 && t1, T2 && t2)
		: first(forward<T1 &&>(t1)), second(forward<T2 &&>(t2)) {}
		
	// Methods: TODO: somehow implement this
	template<size_t I> meta::Select<I, X, Y> & at();
	template<size_t I> const meta::Select<I, X, Y> & at() const;
	
	// Copy and move
	Tuple & operator = (Tuple &&) = default;
	Tuple & operator = (const Tuple &) = default;
	
	template<typename T1, typename T2> Tuple & operator = (Tuple<T1, T2> && tuple)
		{first = rvalue(tuple.first); second = rvalue(tuple.second); return * this;}
	template<typename T1, typename T2> Tuple & operator = (const Tuple<T1, T2> & tuple)
		{first = tuple.first; second = tuple.second; return * this;}
};

// Trio class - Generic three element structure
template<typename X, typename Y, typename Z> struct Tuple<X, Y, Z>
{
	// Fields
	X first;
	Y second;
	Z third;
	
	// Constructors
	Tuple() = default;
	Tuple(Tuple &&) = default;
	Tuple(const Tuple &) = default;
	
	template<typename T1, typename T2, typename T3> Tuple(Tuple<T1, T2, T3> && tuple)
		: first(rvalue(tuple.first)), second(rvalue(tuple.second)), third(rvalue(tuple.third)) {}
	template<typename T1, typename T2, typename T3> Tuple(const Tuple<T1, T2, T3> & tuple)
		: first(tuple.first), second(tuple.second), third(tuple.third) {}
	
	template<typename T1, typename T2, typename T3> Tuple(T1 && t1, T2 && t2, T3 && t3)
		: first(forward<T1 &&>(t1)), second(forward<T2 &&>(t2)), third(forward<T3 &&>(t3)) {}
		
	// Methods: TODO: somehow implement this
	template<size_t I> meta::Select<I, X, Y, Z> & at();
	template<size_t I> const meta::Select<I, X, Y, Z> & at() const;
	
	// Copy and move
	Tuple & operator = (Tuple &&) = default;
	Tuple & operator = (const Tuple &) = default;
	
	template<typename T1, typename T2, typename T3> Tuple & operator = (Tuple<T1, T2, T3> && tuple)
		{first = rvalue(tuple.first); second = rvalue(tuple.second); third = rvalue(tuple.third); return * this;}
	template<typename T1, typename T2, typename T3> Tuple & operator = (const Tuple<T1, T2, T3> & tuple)
		{first = tuple.first; second = tuple.second; third = tuple.third; return * this;}
};

// Quad class - Generic four element structure
template<typename X, typename Y, typename Z, typename W> struct Tuple<X, Y, Z, W>
{
	// Fields
	X first;
	Y second;
	Z third;
	W fourth;
	
	// Constructors
	Tuple() = default;
	Tuple(Tuple &&) = default;
	Tuple(const Tuple &) = default;
	
	template<typename T1, typename T2, typename T3, typename T4> Tuple(Tuple<T1, T2, T3, T4> && tuple)
		: first(rvalue(tuple.first)), second(rvalue(tuple.second)), third(rvalue(tuple.third)) {}
	template<typename T1, typename T2, typename T3, typename T4> Tuple(const Tuple<T1, T2, T3, T4> & tuple)
		: first(tuple.first), second(tuple.second), third(tuple.third) {}
	
	template<typename T1, typename T2, typename T3, typename T4> Tuple(T1 && t1, T2 && t2, T3 && t3)
		: first(forward<T1 &&>(t1)), second(forward<T2 &&>(t2)), third(forward<T3 &&>(t3)) {}
		
	// Methods: TODO: somehow implement this
	template<size_t I> meta::Select<I, X, Y, Z, W> & at();
	template<size_t I> const meta::Select<I, X, Y, Z, W> & at() const;
	
	// Copy and move
	Tuple & operator = (Tuple &&) = default;
	Tuple & operator = (const Tuple &) = default;
	
	template<typename T1, typename T2, typename T3, typename T4> Tuple & operator = (Tuple<T1, T2, T3, T4> && tuple)
		{first = rvalue(tuple.first); second = rvalue(tuple.second); third = rvalue(tuple.third); fourth = rvalue(tuple.fourth); return * this;}
	template<typename T1, typename T2, typename T3, typename T4> Tuple & operator = (const Tuple<T1, T2, T3, T4> & tuple)
		{first = tuple.first; second = tuple.second; third = tuple.third; fourth = tuple.fourth; return * this;}
};

// ------------------------------------------------------------ //
//		Array Implementation
// ------------------------------------------------------------ //

// Array allocator - creates a new array
template<typename T> Array<T> * Array<T>::create(size_t n)
{
	// Allocate array with custom size
	auto * result = nx::type::alloc<Array>(sizeof(Array) + n * sizeof(T));
	// Return null, if the allocation failed
	if (!result) return nullptr;
	// Create array (needs to be done in this function, because constructor is private)
	new (static_cast<void *>(result)) Array(n);
	// Create elements
	nx::type::createArrayAt<T>(result->data, n);
	// Return result
	return result;
}

template<typename T> template<typename U> Array<T> * Array<T>::create(const Array<U> * array)
{
	// Allocate array with custom size
	auto * result = nx::type::alloc<Array>(sizeof(Array) + array->length * sizeof(T));
	// Return null, if the allocation failed
	if (!result) return nullptr;
	// Create array (needs to be done in this function, because constructor is private)
	new (static_cast<void *>(result)) Array(array->length);
	// Create elements
	nx::type::createArrayAtByCopy<T>(result->data, array->data, array->length);
	// Return result
	return result;
}

// Array allocator - creates a new array from a list of elements
template<typename T> template<typename... TS> Array<T> * Array<T>::createFrom(TS && ... list)
{
	constexpr size_t n = sizeof...(list);
	// Allocate array with custom size
	auto * result = nx::type::alloc<Array>(sizeof(Array) + n * sizeof(T));
	// Return null, if the allocation failed
	if (!result) return nullptr;
	// Create array (needs to be done in this function, because constructor is private)
	new (static_cast<void *>(result)) Array(n);
	// Create elements
	nx::type::createArrayAtFromList<T>(result->data, list...);
	// Return result
	return result;
}

// Fill arrays
template<typename T> void Array<T>::fill(Array<T> & array, const T & item)
{
	fill(array, 0, array.length, item);
}
template<typename T> void Array<T>::fill(Array<T> & array, size_t off, size_t len, const T & item)
{
	for (size_t i = 0; i < len; ++ i)
		array[off + i] = item;
}

// Copy arrays
template<typename T> void Array<T>::copy(const Array<T> & src, Array<T> & dest, size_t len)
{
	copy(src, 0, dest, 0, len);
}
template<typename T> void Array<T>::copy(const Array<T> & src, size_t src_idx, Array<T> & dest, size_t dest_idx, size_t len)
{
	// copy supports copying inside the same array, and tries to copy forward
	if (& src != & dest || src_idx >= dest_idx || src_idx + len <= dest_idx)
	{
		for (size_t i = 0; i < len; ++ i)
			dest[src_idx + i] = dest[dest_idx + i];
	}
	else
	{
		for (size_t i = len - 1; i < len; -- i)
			dest[src_idx + i] = dest[dest_idx + i];
	}
}

// Close namespace "nx"
}

// Reset warnings
#pragma GCC diagnostic pop
