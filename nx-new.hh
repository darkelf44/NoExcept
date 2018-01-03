// Include guard
#pragma once

// Local includes
#include "nx-core.hh"

// Disable false warnings in header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++14-compat"

// Normal `new` operator
void * operator new (size_t size) noexcept;
// Array `new` operator
void * operator new [] (size_t size) noexcept;
// Placement `new` operator
inline void * operator new (size_t, void * ptr) noexcept {return ptr;}
// Override size `new` operator
inline void * operator new (size_t, size_t size) noexcept {return operator new (size);}

// Normal `delete` operator
void operator delete (void * obj) noexcept;
// Array `delete` operator
void operator delete [] (void * obj) noexcept;
// Placement `delete` operator - Only called when placement `new` fails
inline void operator delete (void * obj, void *) noexcept {}
// Override size `delete` operator - Only called when override size `new` fails		<- Yes, this is "ill-formed" in C++14. Don't care, it'll work anyway
inline void operator delete (void * obj, size_t) noexcept {operator delete (obj);}

// Namespace "nx::type"
namespace nx { namespace type {
	
// Allocates memory for a type, without constructing them
template<typename T> inline T * alloc() noexcept
{
	return static_cast<T*>(operator new (sizeof(T)));
}

// Allocates memory for a type, without constructing them
template<typename T> inline T * alloc(size_t size) noexcept
{
	return static_cast<T*>(operator new (size));
}

// Deallocates the memory of a type, without deconstructing them
template<typename T> inline T * free(T * obj) noexcept
{
	operator delete (static_cast<void *>(obj)); return nullptr;
}

// Creates a single object (wraps a new expression)
template<typename T, typename... TS> inline T * create(TS && ... args) noexcept(noexcept(new T(static_cast<TS &&>(args)...)))
{
	return new T(static_cast<TS &&>(args)...);
}

// Creates a single opject in place (wraps a placement new expression)
template<typename T, typename... TS> inline T * createAt(T * ptr, TS && ... args) noexcept(noexcept(new (nullptr) T(static_cast<TS &&>(args)...)))
{
	return new (static_cast<void *>(ptr)) T(static_cast<TS &&>(args)...);
}

// Creates an array of objects (wraps a new array expression)
template<typename T> inline T * createArray(size_t n) noexcept(noexcept(new T[n]))
{
	return new T[n];
}

// Creates an array of objects in place (with exception handling)
template<typename T> inline T * createArrayAt(T * ptr, size_t n) noexcept(noexcept(new (nullptr) T()))
{
	if (noexcept(new (nullptr) T()) || !exceptions)
	{
		for (size_t i = 0; i < n; ++ i)
			new (ptr + i) T;
		return ptr;
	}
	else
	{
		size_t i = 0;
		__nx_try
		{
			for (; i < n; ++ i)
				new (ptr + i) T;
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

// Creates an array of objects in place, and uses a list to initialize them
template<typename T, typename... TS> inline T * createArrayAtFrom(T * ptr, TS && ... list) noexcept(noexcept(new (nullptr) T(param<T &&>())))
{
	constexpr size_t n = sizeof...(list);
	// Unpack list elements into a temporary array (TODO: find a way to do this without the temporary array)
	T init[] = {static_cast<TS &&>(list) ...};
	// Construct elements
	size_t i = 0;
	__nx_try
	{
		for (; i < n; ++ i)
			new (ptr + i) T(static_cast<T &&>(init[i]));
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

// Destroys a single object (wraps a delete expression)
template<typename T> inline void destroy(T * ptr) noexcept(noexcept(delete ptr))
{
	delete ptr;
}

// Destroys a single object in place (wraps a destructor call)
template<typename T> inline void destroyAt(T * ptr) noexcept(noexcept(ptr->~T()))
{
	ptr->~T();
}

// Destroys an array of objects (wraps a delete array expression)
template<typename T> inline void destroyArray(T * ptr, size_t n) noexcept(noexcept(delete [] ptr))
{
	delete [] ptr;
}

// Destroys an array of objects in place
template<typename T> inline void destroyArrayAt(T * ptr, size_t n) noexcept(noexcept(ptr->~T()))
{
	if (noexcept(ptr->~T()) || !exceptions)
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

// Reset warnings
#pragma GCC diagnostic pop
