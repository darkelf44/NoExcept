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

	test.runCase("Syntax & utility", [] (bool)
		{
			// Creating array from list
			auto array = nx::makeArrayFrom<int>(10, 20, 30, 40, 50, 60, 70, 80, 90, 100);

			// Iterating over arrays
			{
				size_t i = 0;
				for (auto x : *array)
					expectEqual(array[i ++], x);
				expectEqual(array->length, i);
			}
			
			// Copying arrays
			{
				auto other = nx::makeArray<int>(array->length);
				nx::Array<int>::copy(*array, *other, array->length);
				for (size_t i = 0; i < other->length; ++ i)
					expectEqual(array[i], other[i]);
			}
				
			// Filling arrays
			{
				auto other = nx::makeArray<int>(1000);
				nx::Array<int>::fill(*other, 42);
				for (size_t i = 0; i < other->length; ++ i)
					expectEqual(42, other[i]);
			}
			
		}
	);
}

void TestTuple(nx::Testing & test)
{
	test.runCase("Pair", [] (bool) 	// Very basic sanity test (Ok...)
		{
			
		}
	);
}

void TestSession(nx::Testing & test)
{
	test.runGroup("Test Array", TestArray);
	test.runGroup("Test Tuple", TestTuple);
}

int main()
{
	return nx::Testing::get().runSession("NX Type", TestSession);
}
