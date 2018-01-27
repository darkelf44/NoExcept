// Include guard
#pragma once

// Standard includes
#include <stdio.h>

// Identity function
template<typename T> struct Id { using Result = T; };

class Testing
{
};

template<typename T> void expectTrue(bool expr)
{
	if (expr)
		printf("[+] Passed!\n");
	else
		printf("[ ] Failed!\n");
}

template<typename T> void expectFalse(bool expr)
{
	if (!expr)
		printf("[+] Passed!\n");
	else
		printf("[ ] Failed!\n");
}

template<typename T> void expectEqual(const T & value, typename Id<const T>::Result & expr)
{	
	if (value == expr)
		printf("[+] Passed!\n");
	else
		printf("[ ] Failed!\n");
}