// Include test framework
#include "nx-test.hh"

// Include "nx" library
#include <nx-rng.hh>

template<typename T> void TestRandomGenerator(nx::Testing & test)
{
	// Test if the generator is usable
	test.runCase("Usability", [] (bool)
		{
			T rand;
		}
	);

	// Test if the generator creates reproducible results
	test.runCase("Reproducibility", [] (bool)
		{
			T rand;
		}
	);
}

void TestSession(nx::Testing & test)
{
	// Test Xorshiro 128 generator
	test.runGroup("Xorshiro128", TestRandomGenerator<nx::rng::Xorshiro128n>);
	test.runGroup("Xorshiro128+", TestRandomGenerator<nx::rng::Xorshiro128p>);
	test.runGroup("Xorshiro128*", TestRandomGenerator<nx::rng::Xorshiro128s>);
	test.runGroup("Xorshiro128**", TestRandomGenerator<nx::rng::Xorshiro128ss>);
	
	// Test Xorshiro 128 generator
	test.runGroup("Xorshiro256", TestRandomGenerator<nx::rng::Xorshiro256n>);
	test.runGroup("Xorshiro256+", TestRandomGenerator<nx::rng::Xorshiro256p>);
	test.runGroup("Xorshiro256*", TestRandomGenerator<nx::rng::Xorshiro256s>);
	test.runGroup("Xorshiro256**", TestRandomGenerator<nx::rng::Xorshiro256ss>);
}

int main()
{
	return nx::Testing::get().runSession("NX Random", TestSession);
}