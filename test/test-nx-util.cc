// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-util.hh>

template<typename T> void TestList(nx::Testing & test)
{
	test.runCase( "Sanity" , [] (bool)	// List sanity test - Make sure nothing is broken out of the 
		{
			nx::List<T> list;
			expectEqual(0, list.size());
			expectEqual(0, list.capacity());
			expectEqual(0, list.end() - list.begin());
		}
	);
	
	test.runCase( "Sanity" , [] (bool)	// List sanity test - Make sure nothing is broken out of the 
		{
			nx::List<T> list;
			
			list.append(T());
		}
	);
	
}

int main()
{
	return nx::Testing::get().runGroup("List", TestList<int>);
}