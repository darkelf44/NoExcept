// Include guard
#pragma once

// Standard includes
#include <stddef.h>

// Normal `new` operator
void * operator new (size_t size) noexcept;
// Array `new` operator
void * operator new [] (size_t size) noexcept;
// Placement `new` operator
inline void * operator new (size_t size, void * ptr) noexcept {return ptr;}

// Normal `delete` operator
void operator delete (void * obj) noexcept;
// Array `delete` operator
void operator delete [] (void * obj) noexcept;
// Placement `delete` operator - Only called when placement `new` fails
inline void operator delete (void * obj, void * ptr) noexcept {}

// Introducing std::terminate
namespace std { [[noreturn]] void terminate() noexcept; }

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
template<typename T, typename... TS> inline T * create(TS ... args) noexcept(noexcept(new T(args...)))
{
	return new T(args...);
}

// Creates a single opject in place (wraps a placement new expression)
template<typename T, typename... TS> inline T * createAt(T * ptr, TS ... args) noexcept(noexcept(new (nullptr) T(args...)))
{
	return new (static_cast<void *>(ptr)) T(args...);
}

// Creates an array of objects in place (with exception handling)
template<typename T> inline T * createArrayAt(T * ptr, size_t n) noexcept(noexcept(new (nullptr) T()))
{
	if (noexcept(new (nullptr) T()))
	{
		for (size_t i = 0; i < n; ++ i)
			new (ptr + i) T;
		return ptr;
	}
	else
	{
		size_t i = 0;
		try
		{
			for (; i < n; ++ i)
				new (ptr + i) T;
		}
		catch (...)
		{
			try
			{
				for (; i < n; -- i)
					(ptr + i)->~T();
			}
			catch (...)
			{
				std::terminate();
			}
			throw;
		}
		return ptr;
	}
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

// Destroys an array of objects in place
template<typename T> inline void destroyArrayAt(T * ptr, size_t n)
{
	for (size_t i = n-1; i < n; -- i)
		(ptr + i)->~T();
}


// Close namespace "nx::type"
}}
