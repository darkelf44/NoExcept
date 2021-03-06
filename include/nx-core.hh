// Include guard
#pragma once

// Standard includes
#include <stddef.h>
#include <stdint.h>

// GCC (for some stupid reason) gives an error instead of a warning, when try/catch/throw is used, but exceptions are disabled.
#if !defined(__GNUC__) || defined(__EXCEPTIONS)
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

// Removed keywords - by remapping these keywords, it is possible to use them as identifiers
#define typeid __nx_typeid

// Introducing std::terminate (needed for any code that works with exceptions)
namespace std { [[noreturn]] void terminate() noexcept; }

// Namespace "nx"
namespace nx {

// Aliases for very frequently used types
using byte = unsigned char;
using uint = unsigned int;

// Nothing - Type of the nihil
enum class Nothing { nothing }; constexpr auto nothing = Nothing::nothing;

// Object class - Base of object like types (as opposed value like types)
struct Object
{
	// Constructor & destructor
	inline Object() noexcept;
	inline virtual ~Object() noexcept;
	
	// Copy operators deleted
	Object(const Object &) = delete;
	Object & operator = (const Object &) = delete;
};

Object::Object() noexcept = default;
Object::~Object() noexcept = default;

// [FUNCTION] skip - A very useful function. It takes any number of arguments, and does absolutely nothing
template<typename... TS> constexpr void skip(TS && ...) noexcept
	{ /* Do nothing */ }
	
// [FUNCTION] null - Typed null pointer. Shorthand for static_cast-ing nullptr
template<typename T> constexpr T * null() noexcept
	{ return nullptr; }
	
// [FUNCTION] nullobj - Returns a null object.
template<typename T> constexpr T & nullobj() noexcept
	{ return * null<T>(); }
	
// [FUNCTION] declval - Intended for use in typeof/sizeof/noexcept expressions, creates a expression with the given type
template<typename T> constexpr T && declval() noexcept;
	
// Close namespace "nx"
}

