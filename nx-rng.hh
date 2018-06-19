// Include guard
#pragma once

// Local includes
#include "nx-type.hh"
#include "nx-ptr.hh"

// Namespace "nx::rng"
namespace nx { namespace rng {
	
// ------------------------------------------------------------ //
//		Forward declarations
// ------------------------------------------------------------ //

// Abstract classes
template<typename T> class IGenerator;
template<typename T> class AbstractGenerator;

// Scrambler for PRNGs similar to xorshift
enum class Scrambler { None, Plus, Star, StarStar };

// Generators WIP: The implementationof these classes may change
template<Scrambler S> class Xorshift128;
template<Scrambler S> class Xorshift256;
template<Scrambler S> class Xorshift512;
template<Scrambler S> class Xorshift1024;

template<Scrambler S> class Xorshiro128;
template<Scrambler S> class Xorshiro256;
template<Scrambler S> class Xorshiro512;
template<Scrambler S> class Xorshiro1024;

// 128 bit Xorshiro generators (+O variant)
using Xorshiro128n = Xorshiro128<Scrambler::None>;
using Xorshiro128p = Xorshiro128<Scrambler::Plus>;
using Xorshiro128s = Xorshiro128<Scrambler::Star>;
using Xorshiro128ss = Xorshiro128<Scrambler::StarStar>;

// 256 bit Xorshiro generators (-R variant)
using Xorshiro256n = Xorshiro256<Scrambler::None>;
using Xorshiro256p = Xorshiro256<Scrambler::Plus>;
using Xorshiro256s = Xorshiro256<Scrambler::Star>;
using Xorshiro256ss = Xorshiro256<Scrambler::StarStar>;

// 512 bit Xorshiro generators (-R variant)
using Xorshiro512n = Xorshiro512<Scrambler::None>;
using Xorshiro512p = Xorshiro512<Scrambler::Plus>;
using Xorshiro512s = Xorshiro512<Scrambler::Star>;
using Xorshiro512ss = Xorshiro512<Scrambler::StarStar>;

// 1024 bit Xorshiro generators (+O variant)
using Xorshiro1024n = Xorshiro1024<Scrambler::None>;
using Xorshiro1024p = Xorshiro1024<Scrambler::Plus>;
using Xorshiro1024s = Xorshiro1024<Scrambler::Star>;
using Xorshiro1024ss = Xorshiro1024<Scrambler::StarStar>;

// default random generator type
using Random = Xorshiro128ss;

// ------------------------------------------------------------ //
//		Abstract classes
// ------------------------------------------------------------ //

// [INTERFACE] Generator - Interface for random number generator
template<typename T> class IGenerator
{
public:
	virtual ~IGenerator() = default;
	
	virtual T last() const = 0;
	virtual T next() = 0;
	virtual T jump() = 0;
};

template<typename T> class AbstractGenerator : public Object, public virtual IGenerator<T>
{
public:
	~AbstractGenerator() = default;
};

// ------------------------------------------------------------ //
//		Utilities
// ------------------------------------------------------------ //

// Little PRGN for extending a 64 bit seed - Looks random enough to me ...
inline uint64_t mutateSeed(uint64_t & seed) noexcept
	{return seed = seed * 0xa06eae275b4e718f + 0x1e5c4e6a2cc40bef;}
	
// Create a random array
template<typename R> Array<uint64_t> * createRandomArray(R & random, size_t length) noexcept
{
	// Create array
	auto result = Array<uint64_t>::create(length);
	
	// Fill array with random data
	if (result)
		for (size_t i = 0; i < length; ++ i)
			(*result)[i] = random.next();
	
	// Return it 
	return result;
}

// Create a random array
template<typename R> UniquePtr<Array<uint64_t>> makeRandomArray(R & random, size_t length) noexcept
	{return UniquePtr<Array<uint64_t>>(createRandomArray(random, length));}


// ------------------------------------------------------------ //
//		Generators
// ------------------------------------------------------------ //
	
// Xorshiro128 generator - Tier 1: "Beginner"
template<Scrambler S> class Xorshiro128 : public AbstractGenerator<uint64_t>
{
public:
	// Typename for forcing static bindind
	using This = Xorshiro128;

	// Constructors
	Xorshiro128(uint64_t seed = 42) noexcept
		{This::reset(seed);}
	Xorshiro128(const char * seed, size_t len) noexcept
		{This::reset(seed, len);}
	
	void reset(uint64_t seed) noexcept
	{
		state[0] = seed;
		state[1] = mutateSeed(seed);
	}
	
	uint64_t last() const noexcept override
	{
		if (S == Scrambler::None)
			return state[0];
		
		if (S == Scrambler::Plus)
			return state[0] + state[1];
		
		if (S == Scrambler::Star)
			return state[0] * 0x9e3779b97f4a7c13;
		
		if (S == Scrambler::StarStar)
			return rotateBitsLeft(state[0] * 5, 7) * 9;
	}
	
	uint64_t next() noexcept override
	{
		uint64_t x = state[0];
		uint64_t y = state[1];
		
		// Alpgorithm parameters
		static constexpr uint64_t a = 24;
		static constexpr uint64_t b = 16;
		static constexpr uint64_t c = 37;
		
		// Advance the state
		y ^= x;
		state[0] = rotateBitsLeft(x, a) ^ y ^ (y << b);
		state[1] = rotateBitsLeft(y, c);
		
		// Return the result
		return This::last();
	}
	
	uint64_t jump() noexcept override
	{
		// Polynomial for Jump function (depends on the value of <a, b, c> in the algorithm)
		static constexpr uint64_t jump_function[2] = {0xdf900294d8f554a5, 0x170865df4b3201fc};
		
		uint64_t x = 0;
		uint64_t y = 0;
		
		// Calculate new internal state from the current one
		for(size_t i = 0; i < 2; i ++)
		{
			for(size_t b = 0; b < 64; b ++)
			{
				if (jump_function[i] & ((uint64_t) 1 << b))
				{
					x ^= state[0];
					y ^= state[1];
				}
				This::next();
			}
		}
		
		// Apply the new state
		state[0] = x;
		state[1] = y;
		
		// Return the current random number
		return This::last();
	}
	
private:	
	// Internal 128 bit state
	uint64_t state[2];
};

// Xorshiro256 generator - Tier 2: "Advanced user or PRNG Nerd"
template<Scrambler S> class Xorshiro256 : public AbstractGenerator<uint64_t>
{
public:
	// Typename for forcing static binding
	using This = Xorshiro256;

	// Constructors
	Xorshiro256(uint64_t seed = 42) noexcept
		{This::reset(seed);}
	Xorshiro256(const char * seed, size_t len) noexcept
		{This::reset(seed, len);}
	
	void reset(uint64_t seed) noexcept
	{
		state[0] = seed;
		state[1] = mutateSeed(seed);
		state[2] = mutateSeed(seed);
		state[3] = mutateSeed(seed);
	}
	
	uint64_t last() const noexcept override
	{
		if (S == Scrambler::None)
			return state[0];
		
		if (S == Scrambler::Plus)
			return state[0] + state[3];
		
		if (S == Scrambler::Star)
			return state[0] * 0x9e3779b97f4a7c13;
		
		if (S == Scrambler::StarStar)
			return rotateBitsLeft(state[0] * 5, 7) * 9;
	}	
	
	uint64_t next() noexcept override
	{
		// Alpgorithm parameters
		static constexpr uint64_t a = 17;
		static constexpr uint64_t b = 45;
		
		// Advance the state
		uint64_t t = state[1] << a;
		
		state[2] ^= state[0];
		state[3] ^= state[1];
		state[1] ^= state[2];
		state[0] ^= state[3];

		state[2] ^= t;
		state[3] = rotateBitsLeft(state[3], b);
		
		// Return the result
		return This::last();
	}
	
	uint64_t jump() noexcept
	{
		// Polynomial for Jump function (depends on the value of <a, b> in the algorithm)
		static constexpr uint64_t jump_function[4] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

		uint64_t x = 0;
		uint64_t y = 0;
		uint64_t z = 0;
		uint64_t w = 0;
		
		// Calculate new internal state from the current one
		for(size_t i = 0; i < 4; i ++)
		{
			for(size_t b = 0; b < 64; b ++)
			{
				if (jump_function[i] & ((uint64_t) 1 << b))
				{
					x ^= state[0];
					y ^= state[1];
					z ^= state[2];
					w ^= state[3];
				}
				This::next();	
			}
		}
		
		// Apply the new state
		state[0] = x;
		state[1] = y;
		state[2] = z;
		state[3] = w;
		
		// Return the current random number
		return This::last();
	}
	
private:
	// Internal 256 bit state
	uint64_t state[4];
};

// Xorshiro512 generator - Tier 2: "Overkill"
template<Scrambler S> class Xorshiro512 : public AbstractGenerator<uint64_t>
{
	// Typename for forcing static binding
	using This = Xorshiro512;

	// Constructors
	Xorshiro512(uint64_t seed = 42) noexcept
		{This::reset(seed);}
	Xorshiro512(const char * seed, size_t len) noexcept
		{This::reset(seed, len);}
	
	void reset(uint64_t seed) noexcept
	{
		state[0] = seed;
		for (size_t i = 1; i < 8; ++ i)
			state[i] = mutateSeed(seed);
	}
	
	uint64_t last() const noexcept override
	{
		if (S == Scrambler::None)
			return state[0];
		
		if (S == Scrambler::Plus)
			return state[0] + state[2];
		
		if (S == Scrambler::Star)
			return state[0] * 0x9e3779b97f4a7c13;
		
		if (S == Scrambler::StarStar)
			return rotateBitsLeft(state[0] * 5, 7) * 9;
	}	
	
	uint64_t next() noexcept override
	{
		// Alpgorithm parameters
		static constexpr uint64_t a = 11;
		static constexpr uint64_t b = 21;
		
		// Advance the state
		uint64_t t = state[1] << a;
		
		state[2] ^= state[0];
		state[5] ^= state[1];
		state[1] ^= state[2];
		state[7] ^= state[3];
		state[3] ^= state[4];
		state[4] ^= state[5];
		state[0] ^= state[6];
		state[6] ^= state[7];

		state[6] ^= t;
		state[7] = rotateBitsLeft(state[3], b);
		
		// Return the result
		return This::last();
	}
	
	uint64_t jump() noexcept
	{
		// Polynomial for Jump function (depends on the value of <a, b> in the algorithm)
		static constexpr uint64_t jump_function[8] = {
			0x33ed89b6e7a353f9, 0x760083d7955323be, 0x2837f2fbb5f22fae, 0x4b8c5674d309511c,
			0xb11ac47a7ba28c25, 0xf1be7667092bcc1c, 0x53851efdb6df0aaf, 0x1ebbc8b23eaf25db
		};
		
		uint64_t x[8] = {0};
		
		// Calculate new internal state from the current one
		for(size_t i = 0; i < 4; i ++)
		{
			for(size_t b = 0; b < 64; b ++)
			{
				if (jump_function[i] & ((uint64_t) 1 << b))
				{
					for (size_t k = 0; k < 8; ++ k) 
						x[k] ^= state[k];
				}
				This::next();	
			}
		}
		
		// Apply the new state
		for (size_t k = 0; k < 8; ++ k)
			state[k] = x[k];
		
		// Return the current random number
		return This::last();
	}
	
private:
	// Internal 512 bit state
	uint64_t state[8];
};


// Xorshiro1024 generator - Tier 4: "Beyond overkill"
template<Scrambler S> class Xorshiro1024 : public AbstractGenerator<uint64_t>
{
public:
	// Typename for forcing static bindind
	using This = Xorshiro1024;

	// Constructors
	Xorshiro1024(uint64_t seed = 42) noexcept
		{This::reset(seed);}
	Xorshiro1024(const char * seed, size_t len) noexcept
		{This::reset(seed, len);}
	
	void reset(uint64_t seed) noexcept
	{
		state[0] = seed;
		for (size_t i = 1; i < 16; ++ i)
			state[i] = mutateSeed(seed);
	}
	
	uint64_t last() const noexcept override
	{
		if (S == Scrambler::None)
			return state[shift];
		
		if (S == Scrambler::Plus)
			return state[shift] + state[(shift + 1) & 0xF];
		
		if (S == Scrambler::Star)
			return state[shift] * 0x9e3779b97f4a7c13;
		
		if (S == Scrambler::StarStar)
			return rotateBitsLeft(state[shift] * 5, 7) * 9;
	}	
	
	uint64_t next() noexcept override
	{
		const size_t p = shift;
		const size_t q = (shift + 1) & 0xF;
		shift = q;
		
		uint64_t x = state[p];
		uint64_t y = state[q];
		
		// Alpgorithm parameters
		static constexpr uint64_t a = 25;
		static constexpr uint64_t b = 27;
		static constexpr uint64_t c = 36;
		
		// Advance the state
		y ^= x;
		state[q] = rotateBitsLeft(x, a) ^ y ^ (y << b);
		state[p] = rotateBitsLeft(y, c);
		
		// Return the result
		return This::last();
	}
	
	uint64_t jump() noexcept override
	{
		// TODO: Implement Jump function
	}
	
private:
	// Internal 1024 bit state + shift count
	size_t   shift = 0;
	uint64_t state[16];
};

// Close namespace "nx::rng"
}}
