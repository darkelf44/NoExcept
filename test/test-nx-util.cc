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
	
	test.runCase( "Append & Extend" , [] (bool)	// List append test - Make sure, that we can add elements to the list
		{
			T t;
			
			// Append T &&
			nx::List<T> list1;			
			for (size_t i = 1; i <= 16; ++ i)
			{
				list1.append(T());
				expectEqual(i, list1.size());
			}
			
			// Append const T &
			nx::List<T> list2;			
			for (size_t i = 1; i <= 16; ++ i)
			{
				list2.append(t);
				expectEqual(i, list2.size());
			}
			
			// Extend T &&
			nx::List<T> list3;
			for (size_t i = 1; i <= 16; ++ i)
			{
				list3.extend(nx::List<T>(list1));
				expectEqual(i * list1.size(), list3.size());
			}
			
			// Extend const T &
			nx::List<T> list4;
			for (size_t i = 1; i <= 16; ++ i)
			{
				list4.extend(list1);
				expectEqual(i * list1.size(), list4.size());
			}
		}
	);
	
}

int main()
{
	return nx::Testing::get().runGroup("List", TestList<int>);
}