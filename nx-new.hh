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

// Check for noexcept constructor with the given types
template<typename T, typename ... TS> constexpr bool isCreateNoexcept()
	{return noexcept(new (nullptr, nothing) T(param<TS>() ...));}
// Check for noexcept destructor
template<typename T> constexpr bool isDestroyNoexcept()
	{return noexcept(param<T &>().~T());}
// Check for noexcept copy
template<typename T> constexpr bool isCopyNoexcept()
	{return noexcept(param<T &>() = param<T &>());}
// Check for noexcept move
template<typename T> constexpr bool isMoveNoexcept()
	{return noexcept(param<T &>() = param<T &&>());}

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
template<typename T, typename... TS> inline T * createAt(T * ptr, TS && ... args) noexcept(isCreateNoexcept<T, TS ...>())
{
	return new (static_cast<void *>(ptr), nothing) T(static_cast<TS &&>(args)...);
}

// Create a single object with a custom size
template<typename T, typename... TS> inline T * createWithSize(size_t size, TS && ... args) noexcept(isCreateNoexcept<T, TS ...>())
{
	return new (alloc<void*>(size), nothing) T(static_cast<TS &&>(args)...);
}

// Create an array of objects (wraps a new array expression)
template<typename T> inline T * createArray(size_t n) noexcept(noexcept(new T[n]))
{
	return new T[n];
}

// Create an array of objects in place (with exception handling)
template<typename T> inline T * createArrayAt(T * ptr, size_t n) noexcept(isCreateNoexcept<T>() || !exceptions)
{
	if (n == 0)
		return ptr;
	
	if (isCreateNoexcept<T>() || !exceptions)
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
template<typename T, typename... TS> inline T * createArrayAtByCopy(T * ptr, const T * array, size_t n) noexcept(isCreateNoexcept<T, T &>() || !exceptions)
{
	if (n == 0)
		return ptr;
	
	if (isCreateNoexcept<T, T &>() || !exceptions)
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

// Create an array of objects in place, by moving from an other array (if it can be safely moved, otherwise copy)
template<typename T, typename... TS> inline T * createArrayAtByMove(T * ptr, T * array, size_t n) noexcept(isCreateNoexcept<T, T>() || isCreateNoexcept<T, T &>() || !exceptions)
{
	if (n == 0)
		return ptr;
	
	if (isCreateNoexcept<T, T &&>() || !exceptions)
	{
		for (size_t i = 0; i < n; ++ i)
			new (ptr + i, nothing) T(static_cast<T &&>(array[i]));		
		return ptr;	
	}
	else
	{
		return createArrayAtByCopy(ptr, array, n);
	}
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
template<typename T> inline void destroyAt(T * ptr) noexcept(isDestroyNoexcept<T>())
{
	ptr->~T();
}

// Destroy an array of objects (wraps a delete array expression)
template<typename T> inline void destroyArray(T * ptr, size_t n) noexcept(noexcept(delete [] ptr))
{
	delete [] ptr;
}

// Destroy an array of objects in place
template<typename T> inline void destroyArrayAt(T * ptr, size_t n) noexcept(isDestroyNoexcept<T>())
{
	if (n == 0)
		return;
	
	if (isDestroyNoexcept<T>() || !exceptions)
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
