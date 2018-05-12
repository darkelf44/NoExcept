// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-type.hh>
#include <nx-ptr.hh>

void TestArray(nx::Testing & test)
{
	test.runCase("Sanity", [] (bool) 	// Very basic sanity test (Ok...)
		{
			// Creating arrays with different sizes
			for (size_t n = 0; n < 1000; ++ n)
			{
				auto array = nx::makeArray<int>(n);
				
				expectEqual(n, array->length);
				expectEqual(array->length, array->end() - array->begin());
			}
		}
	);

	test.runCase("Set & get", [] (bool)		// Testing different array sizes (Why?)
		{
			auto array = nx::makeArray<int>(1000);

			// Make sure nothing gets overwritten
			for (size_t n = 0; n < 10; ++ n)
			{
				for (size_t i = 0; i < array->length; ++ i)
					array[i] = 1000 * n + i;
				for (size_t i = 0; i < array->length; ++ i)
					expectEqual(1000 * n + i, array[i]);
			}
		}
	);

	test.runCase("Syntax & utility", [] (bool)		// Testing other stuff
		{
			// Creating array from list
			auto array = nx::makeArrayFrom<int>(1, 1, 2, 3, 5, 8, 13, 21, 35, 56);

			// Iterating over arrays
			size_t i = 0;
			for (auto x : *array)
				expectEqual(array[i++], x);
			
			// Copying arrays

			
		}
	);
}

int main()
{
	return nx::Testing::get().runGroup("Test Array<>", TestArray);
}
