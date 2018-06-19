// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-util.hh>
#include <nx-rng.hh>

void TestRange(nx::Testing & test)
{
	test.runCase( "Range<int>" , [] (bool)
		{
			int n;
			int j;

			j = 0;
			for (int i: nx::range(10))
				expectEqual(j ++, i);
			expectEqual(10, j);

			j = 10;
			for (int i: nx::range(10, 0, -1))
				expectEqual(j --, i);
			expectEqual(0, j);
			
			n = 0;
			j = 0;
			for (int i: nx::range(0, 10, 3))
			{
				expectEqual(j, i);
				++ n;
				j += 3;
			}
			expectEqual(4, n);
			expectEqual(12, j);
			
			n = 0;
			j = 10;
			for (int i: nx::range(10, 0, -3))
			{
				expectEqual(j, i);
				++ n;
				j -= 3;
			}
			expectEqual(4, n);
			expectEqual(-2, j);
			
		}
	);
	
	test.runCase( "Range<double>" , [] (bool)
		{
			int n, j;
			double g;
			
			j = 0;
			for (double f: nx::range(10.0))
			{
				expectEqual(double(j++), f);
			}
			expectEqual(10, j);
			
			j = 10;
			for (double f: nx::range(10.0, 0.0, -1.0))
				expectEqual(double(j--), f);
			expectEqual(0, j);
			
			n = 0;
			g = 0.0;
			for (double f: nx::range(0.0, 10.0, 3.0/7.0))
			{
				expectEqual(g, f);
				++ n;
				g += 3.0/7.0;
			}
			expectEqual(24, n);
			
			n = 0;
			g = 10.0;
			for (double f: nx::range(10.0, 0.0, -3.0/7.0))
			{
				expectEqual(g, f);
				++ n;
				g -= 3.0/7.0;
			}
			expectEqual(24, n);
		}			
	);
}


void TestList(nx::Testing & test)
{
	test.runCase( "Sanity" , [] (bool)	// List sanity test - Make sure nothing is broken from the start
		{
			nx::List<uint64_t> list;
			expectEqual(0, list.size());
			expectEqual(0, list.capacity());
			expectEqual(0, list.end() - list.begin());
		}
	);
	
	test.runCase( "Resize & Reserve" , [] (bool)
		{
			
		}
	);
	
	test.runCase( "Append & Extend" , [] (bool)
		{
			nx::rng::Random r(2018);
			auto list = nx::rng::makeRandomArray(r, 16);
			
			// Append T &&
			nx::List<uint64_t> list1;
			for (size_t i : nx::range(16))
			{
				list1.append(uint64_t(list[i]));
				expectEqual(i + 1, list1.size());
			}
			
			// Check elements
			for (size_t i : nx::range(16))
				expectEqual(list[i], list1[i]);
			
			// Append const T &
			nx::List<uint64_t> list2;			
			for (size_t i : nx::range(16))
			{
				list2.append(list[i]);
				expectEqual(i + 1, list2.size());
			}
			
			// Check elements
			for (size_t i : nx::range(16))
				expectEqual(list[i], list1[i]);
			
			// Extend T &&
			nx::List<uint64_t> list3;
			for (size_t i : nx::range(16))
			{
				list3.extend(nx::List<uint64_t>(list1));
				expectEqual((i + 1) * list1.size(), list3.size());
			}
			
			// Check elements
			for (size_t i : nx::range(16))
				for (size_t j : nx::range(16))
					expectEqual(list[j], list3[i*16 + j]);
			
			// Extend const T &
			nx::List<uint64_t> list4;
			for (size_t i : nx::range(16))
			{
				list4.extend(list1);
				expectEqual((i + 1) * list1.size(), list4.size());
			}
			
			// Check elements
			for (size_t i : nx::range(16))
				for (size_t j : nx::range(16))
					expectEqual(list[j], list4[i*16 + j]);
		}
	);
	
}

void TestSession(nx::Testing & test)
{
	test.runGroup("Range", TestRange);
	test.runGroup("List", TestList);
//	test.runGroup("", Test);
}

int main()
{
	return nx::Testing::get().runSession("NX Util", TestSession);
}