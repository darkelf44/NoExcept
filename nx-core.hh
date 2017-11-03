// Include guard
#pragma once

// Standard includes
#include <stddef.h>
#include <stdint.h>

// Don't warn about C++14, when I'm *NOT* using C++14 !!! (Stupid compiler)
#pragma GCC diagnostic ignored "-Wc++14-compat"

// Support compiling with and without exceptions
#ifdef __EXCEPTIONS
	#define __nx_try	try
	#define __nx_catch(x)	catch(x)
	#define __nx_throw(x)	throw x
	// Exceptions are supported, use exception aware code, when needed
	static constexpr bool exceptions = true;
#else 
	#define __nx_try	if (true)
	#define __nx_catch(x)	if (false)
	#define __nx_throw(x)	{}
	// Exceptions are not supported, never use exception aware code
	static constexpr bool exceptions = false;
#endif

// Introducing std::terminate (needed for any code that works with exceptions)
namespace std { [[noreturn]] void terminate() noexcept; }

// Namespace "nx"
namespace nx {

// Aliases for very frequently used types
using byte = unsigned char;
using uint = unsigned int;

// Object class - Base of object like types (as opposed value like types)
struct Object
{
	// Constructor & destructor
	inline Object();
	inline virtual ~Object();
	
	// Copy operators deleted
	Object(const Object &) = delete;
	Object & operator = (const Object &) = delete;
};

Object::Object() = default;
Object::~Object() = default;

// Close namespace "nx"
}

