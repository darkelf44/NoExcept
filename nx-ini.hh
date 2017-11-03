// Include guard
#pragma once

// Local includes
#include "nx.hh"

// Standard includes
#include <stdio.h>

// Namespace "nx"
namespace nx {

class Ini : public Object
{
public:
	Ini() = default

	static UniquePtr<Ini> parse(FILE * file);
	static UniquePtr<Ini> parse(const char * filename);
private:
};

// Close namespace "nx"
}
