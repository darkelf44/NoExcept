// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-util.hh>

template<typename T> void TestList(nx::Testing & test)
{
	test.runCase( "Sanity" , [] (bool)	// List sanity test - Make sure nothing is broken from the start
		{
			nx::List<T> list;
			expectEqual(0, list.size());
			expectEqual(0, list.capacity());
			expectEqual(0, list.end() - list.begin());
		}
	);
	
	test.runCase( "Append" , [] (bool)	// List append test - Make sure, that we can add elements to the list
		{
			nx::List<T> list;
			
			list.append(T());
			expectEqual(1, list.size());
			list.append(T());
			expectEqual(2, list.size());
			list.append(T());
			expectEqual(3, list.size());
			list.append(T());
			expectEqual(4, list.size());
		}
	);
	
}

int main()
{
	return nx::Testing::get().runGroup("List", TestList<int>);
}