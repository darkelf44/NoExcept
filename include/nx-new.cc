// Header include
#include "nx-new.hh"

// Standard includes
#include <stdlib.h>

void * operator new (size_t size) noexcept
{
	return ::malloc(size);
}

void * operator new [] (size_t size) noexcept
{
	return ::malloc(size);
}

void operator delete (void * obj) noexcept
{
	::free(obj);
}

void operator delete [] (void * obj) noexcept
{
	::free(obj);
}
