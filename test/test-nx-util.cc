// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-util.hh>

// int suffix, for narrowing conversions
constexpr int operator "" _i (unsigned long long i)
	{return static_cast<int>(i);}

template<typename T> class TestData
{
	static T random[];
	static T select[];
};

// Random data set
template<> int TestData<int>::random[] = {
	-545300232, 1417913656, 341412532, 1533270110, -249392204, -1962300738, 193878770, -1016769243,
	-982441607, 1915417778, 167073993, 1212070828, -1316408840, 406944549, 923954193, 98145105,
	620088011, 313620423, -1964568121, 1609187590, 1637611819, -1284514332, 1070369006, -517173077,
	1507022977, -695059478, -1673547419, -802516921, 423550275, 1322904686, -1133035350, 599027874
};

// Hand picked data set
template<> int TestData<int>::select[] = {
	0, 1, 2, 3, 4, 7, 8, 15,
	16, 32, 128, 256, 512, 1024, 2048, 4096,
	0x10000_i, 0x100000_i, 0x1000000_i, 0x10000000_i, 0x01010101_i, 0x10101010_i, 0x0A0B0C0D_i, 0xA0B0C0D0_i,
	0xFFFFFFFF_i, 0xFFFFFFF0_i, 0xFFFFFF00_i, 0xFFFFF000_i, 0xFFFF0000_i, 0xFFF00000_i, 0xFF000000_i, 0xF0000000_i
};

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
	
	test.runCase( "Resize & Reserve" , [] (bool) {});
	
	test.runCase( "Append & Extend" , [] (bool)
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