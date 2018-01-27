#include <nx-type.hh>
#include <nx-ptr.hh>

#include "nx-test.hh"

int main()
{
	// Very basic sanity test
	{
		auto array = nx::makeArray<int>(0);
		expectEqual(0, array->length);	
		expectEqual(array->length, array->end() - array->begin());	
	}
	
	// Testing single element array - Really?
	{
		auto array = nx::makeArray<int>(1);
		expectEqual(1, array->length);
		expectEqual(array->length, array->end() - array->begin());
		array[0] = 2;
		expectEqual(2, array[0]);
		array[0] = 4;
		expectEqual(4, array[0]);
	}

	// Testing multiple element array - Why are we doing this again? 
	{
		auto array = nx::makeArray<int>(8);
		expectEqual(8, array->length);		
		expectEqual(array->length, array->end() - array->begin());
		for (size_t i = 0; i < array->length; ++ i)
			array[i] = 2 * i + 4;
		for (size_t i = 0; i < array->length; ++ i)
			expectEqual(2 * i + 4, array[i]);
	}
}
