// Include guard
#pragma once

// Local includes
#include "nx-core.hh"

// Disable -Wterminate : gcc over warns on templates
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wterminate"

// Placement `new` operator for nx (takes an additional nothing)
inline void * operator new (size_t, void * ptr, nx::Nothing) noexcept {return ptr;}

// Placement `delete` operator for nx (takes an additional nothing) - Only called when placement `new` fails
inline void operator delete (void *, void *, nx::Nothing) noexcept {}


// Namespace "nx::type"
namespace nx { namespace type {

// Namespace "nx::type::proto" - Naked C++ meta functions, without wrappers
namespace proto {

// [META FUNCTION] Id - Identity function, returns the given type. Useful for preventing type deduction
template<typename T> struct Id { using Result = T; };

// [META FUNCTION] Seq - Sequence function, returns the last type. Useful for SFINAE along with decltype
template<typename ... TS> struct Seq;
template<typename T> struct Seq<T> { using Result = T; };
template<typename T, typename ... TS> struct Seq<T, TS ...> { using Result = typename Seq<TS ...>::Result; };

// [META FUNCTION] EnableIf - Enables template function, if the condition is true
template<bool b, typename T> struct EnableIf {};
template<typename T> struct EnableIf<true, T> { using Result = T; };

// [META FUNCTION] DisableIf - Enables template function, if the condition is false
template<bool b, typename T> struct DisableIf {};
template<typename T> struct DisableIf<false, T> { using Result = T; };

// Close namespace "nx::type::proto"
}

template<typename T> using Id = typename proto::Id<T>::Result;
template<typename ... TS> using Seq = typename proto::Seq<TS ...>::Result;

template<bool b, typename T = void> using EnableIf = typename proto::EnableIf<b, T>::Result;
template<bool b, typename T = void> using DisableIf = typename proto::DisableIf<b, T>::Result;
	
// Namespace "nx::type::impl"
namespace impl {

// Tests if the type has a specific constructor
template<typename T, typename ... TS>
	constexpr bool hasCreate(...) noexcept {return false; }
template<typename T, typename ... TS, typename = Seq<decltype(new (nullptr, nothing) T(declval<TS>() ...)), void>>
	constexpr bool hasCreate(int) noexcept {return true;}

// Tests if the type has a destructor
template<typename T>
	constexpr bool hasDestroy(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>().~T()), void>>
	constexpr bool hasDestroy(int) noexcept {return true;}

// Tests if the type has a copy assignment
template<typename T>
	constexpr bool hasCopy(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>() = declval<T &>()), void>>
	constexpr bool hasCopy(int) noexcept {return true;}

// Tests if the type has a move assignment
template<typename T>
	constexpr bool hasMove(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>() = declval<T &&>()), void>>
	constexpr bool hasMove(int) noexcept {return true;}

// Tests if the type has specific noexcept constructor
template<typename T, typename ... TS>
	constexpr bool hasNoexceptCreate(...) noexcept {return false;}
template<typename T, typename ... TS, typename = Seq<decltype(new (nullptr, nothing) T(declval<TS>() ...)), void>>
	constexpr bool hasNoexceptCreate(int) noexcept {return noexcept(new (nullptr, nothing) T(declval<TS>() ...));}

// Tests if the type has a noexcept destructor
template<typename T>
	constexpr bool hasNoexceptDestroy(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>().~T()), void>>
	constexpr bool hasNoexceptDestroy(int) noexcept {return noexcept(declval<T &>().~T());}

// Tests if the type has a noexcept copy assignment
template<typename T>
	constexpr bool hasNoexceptCopy(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>() = declval<T &>()), void>>
	constexpr bool hasNoexceptCopy(int) noexcept {return noexcept(declval<T &>() = declval<T &>());}

// Tests if the type has a noexcept move assignment
template<typename T>
	constexpr bool hasNoexceptMove(...) noexcept {return false;}
template<typename T, typename = Seq<decltype(declval<T &>() = declval<T &&>()), void>>
	constexpr bool hasNoexceptMove(int) noexcept {return noexcept(declval<T &>() = declval<T &&>());}
	
// Close namespace "nx::type::impl"
}

// Check for default functions
template<typename T, typename ... TS> constexpr bool hasCreate()
	{return impl::hasCreate<T, TS ...>(0);}
template<typename T> constexpr bool hasDestroy()
	{return impl::hasDestroy<T>(0);}
template<typename T> constexpr bool hasCopy()
	{return impl::hasCopy<T>(0);}
template<typename T> constexpr bool hasMove()
	{return impl::hasMove<T>(0);}

// Check for noexcept default functions
template<typename T, typename ... TS> constexpr bool hasNoexceptCreate()
	{return impl::hasNoexceptCreate<T, TS ...>(0);}
template<typename T> constexpr bool hasNoexceptDestroy()
	{return impl::hasNoexceptDestroy<T>(0);}
template<typename T> constexpr bool hasNoexceptCopy()
	{return impl::hasNoexceptCopy<T>(0);}
template<typename T> constexpr bool hasNoexceptMove()
	{return impl::hasNoexceptMove<T>(0);}

// Allocate memory for a type, without constructing them
template<typename T> inline T * alloc(size_t size = sizeof(T)) noexcept
{
	return static_cast<T*>(operator new (size));
}

// Deallocate the memory of a type, without deconstructing them
template<typename T> inline T * free(T * obj) noexcept
{
	operator delete (static_cast<void *>(obj)); return nullptr;
}

// Confirm that allocations were successful. TODO: Throws <?> exception if any of the pointers are null
template<typename... TS> inline void confirm(const TS * ... ptrs)
{
	// Extract parameter pack (I prefer this to recursion)
	bool list[] = {ptrs ...};
	// Check for null pointer
	for (bool value : list)
	{
		if (!value)
		{
			if (exceptions)
			{
				//__nx_throw();
			}
			else
			{
				std::terminate();
			}
		}
	}
}

// Create a single object (wraps a new expression)
template<typename T, typename... TS> inline T * create(TS && ... args) noexcept(noexcept(new T(static_cast<TS &&>(args)...)))
{
	return new T(static_cast<TS &&>(args)...);
}

// Create a single opject in place (wraps a placement new expression)
template<typename T, typename... TS> inline T * createAt(T * ptr, TS && ... args) noexcept(hasNoexceptCreate<T, TS ...>())
{
	return new (static_cast<void *>(ptr), nothing) T(static_cast<TS &&>(args)...);
}

// Create a single object with a custom size
template<typename T, typename... TS> inline T * createWithSize(size_t size, TS && ... args) noexcept(hasNoexceptCreate<T, TS ...>())
{
	return new (alloc<void*>(size), nothing) T(static_cast<TS &&>(args)...);
}

// Create an array of objects (wraps a new array expression)
template<typename T> inline T * createArray(size_t n) noexcept(noexcept(new T[n]))
{
	return new T[n];
}

// Create an array of objects in place (with exception handling)
template<typename T> inline T * createArrayAt(T * ptr, size_t n) noexcept(hasNoexceptCreate<T>() || !exceptions)
{
	if (n == 0)
		return ptr;
	
	if (hasNoexceptCreate<T>() || !exceptions)
	{
		for (size_t i = 0; i < n; ++ i)
			new (ptr + i, nothing) T;
		return ptr;
	}
	else
	{
		size_t i = 0;
		__nx_try
		{
			for (; i < n; ++ i)
				new (ptr + i, nothing) T;
		}
		__nx_catch (...)
		{
			__nx_try
			{
				for (; i < n; -- i)
					(ptr + i)->~T();
			}
			__nx_catch (...)
			{
				std::terminate();
			}
			__nx_throw();
		}
		return ptr;
	}
}

// Create an array of objects in place, by copying from an other array
template<typename T> inline T * createArrayAtByCopy(T * ptr, const T * array, size_t n) noexcept(hasNoexceptCreate<T, T &>() || !exceptions)
{
	if (n == 0)
		return ptr;
	
	if (hasNoexceptCreate<T, T &>() || !exceptions)
	{
		for (size_t i = 0; i < n; ++ i)
			new (ptr + i, nothing) T(array[i]);
		return ptr;	
	}
	else
	{
		size_t i = 0;
		__nx_try
		{
			for (; i < n; ++ i)
				new (ptr + i, nothing) T(array[i]);
		}
		__nx_catch (...)
		{
			__nx_try
			{
				for (; i < n; -- i)
					(ptr + i)->~T();
			}
			__nx_catch (...)
			{
				std::terminate();
			}
			__nx_throw();
		}
		return ptr;	
	}
}

// Create an array of objects in place, by moving from an other array (if it can be safely moved)
template<typename T> inline EnableIf<hasNoexceptCreate<T, T>(), T *> createArrayAtByMove(T * ptr, T * array, size_t n) noexcept
{
	if (n == 0)
		return ptr;
	
	for (size_t i = 0; i < n; ++ i)
		new (ptr + i, nothing) T(static_cast<T &&>(array[i]));		
	return ptr;
}

// Create an array of objects in place, by copying from an other array (if it can't be safely moved)
template<typename T> inline EnableIf<!hasNoexceptCreate<T, T>(), T *> createArrayAtByMove(T * ptr, T * array, size_t n) noexcept(hasNoexceptCreate<T, T &>() || !exceptions)
{
	return createArrayAtByCopy(ptr, array, n);
}

// Create an array of objects in place, and initialize them from the list
template<typename T, typename... TS> inline T * createArrayAtFromList(T * ptr, TS && ... list)
{
	constexpr size_t n = sizeof...(list);
	// Unpack list elements into a temporary array (TODO: find a way to do this without the temporary array)
	T init[] = {static_cast<TS &&>(list) ...};
	// Construct elements from temporary array
	return createArrayAtByMove(ptr, init, n);
}

// Destroy a single object (wraps a delete expression)
template<typename T> inline void destroy(T * ptr) noexcept(noexcept(delete ptr))
{
	delete ptr;
}

// Destroy a single object in place (wraps a destructor call)
template<typename T> inline void destroyAt(T * ptr) noexcept(hasNoexceptDestroy<T>())
{
	ptr->~T();
}

// Destroy an array of objects (wraps a delete array expression)
template<typename T> inline void destroyArray(T * ptr, size_t n) noexcept(noexcept(delete [] ptr))
{
	delete [] ptr;
}

// Destroy an array of objects in place
template<typename T> inline void destroyArrayAt(T * ptr, size_t n) noexcept(hasNoexceptDestroy<T>())
{
	if (n == 0)
		return;
	
	if (hasNoexceptDestroy<T>() || !exceptions)
	{
		for (size_t i = n-1; i < n; -- i)
			(ptr + i)->~T();
	}
	else
	{
		size_t i = n-1;
		__nx_try
		{
			for (; i < n; -- i)
				(ptr + i)->~T();
		}
		__nx_catch (...)
		{
			__nx_try
			{
				for (-- i; i < n; -- i)
					(ptr + i)->~T();
			}
			__nx_catch (...)
			{
				std::terminate();
			}
			__nx_throw();
		}
	}
}

// Close namespace "nx::type"
}}

// Restore GCC diagnostic options
#pragma GCC diagnostic pop
