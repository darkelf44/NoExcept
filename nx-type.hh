// Include guard
#pragma once

// Local includes
#include "nx-new.hh"

// Disable false warnings in header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

// Namespace "nx"
namespace nx {

// Namespace "nx::type" - Type system related functionality
namespace type {

// Namespace "nx::type::proto" - Naked C++ meta function, without wrappers
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

template<typename T> using Id = T;
template<bool b, typename T> using EnableIf = typename nx::type::proto::EnableIf<b, T>::Result;

template<typename T> inline constexpr bool isConstant()
	{ return nx::type::proto::IsConstant<T>::result; }
template<typename T> inline constexpr bool isVolatile()
	{ return nx::type::proto::IsVolatile<T>::result; }
template<typename T> inline constexpr bool isQualified()
	{ return nx::type::proto::IsConstant<T>::result || nx::type::proto::IsVolatile<T>::result; }
	
template<typename T> inline constexpr bool isPointer()
	{ return nx::type::proto::IsPointer<T>::result; }
template<typename T> inline constexpr bool isReference()
	{ return nx::type::proto::IsReference<T>::result; }
template<typename T> inline constexpr bool isLValueReference()
	{ return nx::type::proto::IsReference<T>::result; }
template<typename T> inline constexpr bool isRValueReference()
	{ return nx::type::proto::IsRValueReference<T>::result; }
template<typename T> inline constexpr bool isAnyReference()
	{ return nx::type::proto::IsReference<T>::result || nx::type::proto::IsRValueReference<T>::result; }

template<typename T> using RemoveConstant = typename nx::type::proto::RemoveConstant<T>::Result;
template<typename T> using RemoveVolatile = typename nx::type::proto::RemoveVolatile<T>::Result;
template<typename T> using RemoveQualifiers = typename nx::type::proto::RemoveQualifiers<T>::Result;

template<typename T> using RemovePointer = typename nx::type::proto::RemovePointer<T>::Result;
template<typename T> using RemoveReference = typename nx::type::proto::RemoveReference<T>::Result;
template<typename T> using RemoveLValueReference = typename nx::type::proto::RemoveReference<T>::Result;
template<typename T> using RemoveRValueReference = typename nx::type::proto::RemoveRValueReference<T>::Result;
template<typename T> using RemoveAnyReference = typename nx::type::proto::RemoveAnyReference<T>::Result;

template<typename T> using Strip = typename RemoveQualifiers<typename RemoveAnyReference<T>::Result>::Result;


// Close namespace "nx::type"	
}

// Add "EnableIf" to the "nx" namespace
using nx::type::EnableIf;

// Add "typeid" to the "nx" namespace
using nx::type::typeid;

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
	
	// Allocators
	static Array<T> * create(size_t n);	
	template<typename... TS> static Array<T> * createFrom(TS && ... list);
	
	// Fill arrays
	static void fill(Array<T> & array, const T & item);
	static void fill(Array<T> & array, size_t off, size_t len, const T & item);
	
	// Copy arrays
	static void copy(const Array<T> & src, Array<T> & dest, size_t len);
	static void copy(const Array<T> & src, size_t src_idx, Array<T> & dest, size_t dest_idx, size_t len);
	
private:
	// Constructors
	Array(size_t n)
		: length(n) {}
};

// Array allocator - creates a new array
template<typename T> Array<T> * Array<T>::create(size_t n)
{
	// Allocate array with custom size
	auto * array = nx::type::alloc<Array<T>>(sizeof(Array<T>) + n * sizeof(T));
	// Return null, if the allocation failed
	if (!array) return nullptr;
	// Create array (needs to be done in this function, because constructor is private)
	new (static_cast<void *>(array)) Array<T>(n);
	// Create elements
	nx::type::createArrayAt<T>(array->data, n);
	// Return array
	return array;
}

// Array allocator - creates a new array from a list of elements
template<typename T> template<typename... TS> Array<T> * Array<T>::createFrom(TS && ... list)
{
	constexpr size_t n = sizeof...(list);
	// Allocate array with custom size
	auto * array = nx::type::alloc<Array<T>>(sizeof(Array<T>) + n * sizeof(T));
	// Return null, if the allocation failed
	if (!array) return nullptr;
	// Create array (needs to be done in this function, because constructor is private)
	new (static_cast<void *>(array)) Array<T>(n);
	// Create elements
	nx::type::createArrayAtFromList<T>(array->data, list...);
	// Return array
	return array;
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
