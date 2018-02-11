// Include guard
#pragma once

// Standard includes
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

// C++ includes
#include <exception>

// GCC bug: snprintf is not in stdio.h
#if defined(__GNUC__)
	extern "C" int snprintf(char *, int, const char *, ...);
#endif

// Namespace "nx"
namespace nx {

// Each test is expected to be a single translation unit
class Testing
{
public:
	// Identity wrapper - disables type deduction
	template<typename T> struct Id { using Result = T; };
	
	// Rings for grouping test results
	enum Ring {
		TEST_CASE = 0,
		TEST_GROUP = 1,
		TEST_SESSION = 2,
		
		NOTHING = -1
	};
	constexpr static size_t RINGS = 3;
	
	// Source Location
	struct Location 
	{
		const char * const file;
		const char * const func;
		const uint64_t line;
		
		Location(const char * file, const char * func, uint64_t line)
			: file(file), func(func), line(line) {}
	};

	// Constructor
	Testing();
	
	// Single instance
	static Testing & get();
	
	// Can be converted to bool
	operator bool ()
		{return good;}
	
	// Logging (actually just calls fprintf)
	void log(const char * format, ...);
	
	// Assertion results
	void assertPassed(size_t n = 1);
	void assertFailed(size_t n = 1);
	
	// Running tests
	template<typename F, typename ... TS> bool run(Ring r, const char * name, const F & func, TS && ... params);
	
	template<typename F, typename ... TS> bool runCase(const char * name, const F & func, TS && ... params)
		{return run(TEST_CASE, name, func, static_cast<TS &&>(params)...);}
	template<typename F, typename ... TS> bool runGroup(const char * name, const F & func, TS && ... params)
		{return run(TEST_GROUP, name, func, static_cast<TS &&>(params)...);}
	template<typename F, typename ... TS> bool runSession(const char * name, const F & func, TS && ... params)
		{return run(TEST_SESSION, name, func, static_cast<TS &&>(params)...);}
		
	// Convert any value to string
	static size_t toString(char * buffer, size_t n, bool value);
	static size_t toString(char * buffer, size_t n, int64_t value);
	static size_t toString(char * buffer, size_t n, uint64_t value);
	static size_t toString(char * buffer, size_t n, double value);
	static size_t toString(char * buffer, size_t n, const void * value);
	static size_t toString(char * buffer, size_t n, const char * value);
	template<typename T> static size_t toString(char * buffer, size_t n, T * value);
	template<typename T> static size_t toString(char * buffer, size_t n, const T & value);

	static size_t toString(char * buffer, size_t n, int8_t value)
		{return toString(buffer, n, static_cast<int64_t>(value));}
	static size_t toString(char * buffer, size_t n, int16_t value)
		{return toString(buffer, n, static_cast<int64_t>(value));}
	static size_t toString(char * buffer, size_t n, int32_t value)
		{return toString(buffer, n, static_cast<int64_t>(value));}
	static size_t toString(char * buffer, size_t n, uint8_t value)
		{return toString(buffer, n, static_cast<uint64_t>(value));}
	static size_t toString(char * buffer, size_t n, uint16_t value)
		{return toString(buffer, n, static_cast<uint64_t>(value));}
	static size_t toString(char * buffer, size_t n, uint32_t value)
		{return toString(buffer, n, static_cast<uint64_t>(value));}
	static size_t toString(char * buffer, size_t n, float value)
		{return toString(buffer, n, static_cast<double>(value));}
	

private:
	// All asserts passed
	bool good;
	
	// Ring data
	struct RingData
	{
		const char * testName;
		const char * ringName;
		
		bool active;
		bool failed;
		
		uint32_t success;
		uint32_t failure;
	}
	rings[RINGS];
	
	Testing(const Testing &) = delete;	
	Testing & operator = (const Testing &) = delete;	
	
	void start(Ring r, const char * name);
	void finish(Ring r);
	
	static bool isLowestRing(Ring r)
		{return r == TEST_CASE;}
	static bool isHighestRing(Ring r)
		{return r == TEST_SESSION;}
};

Testing::Testing()
{
	good = true;
	
	for (auto & ring: rings)
		ring.active = false;
	
	rings[TEST_CASE].ringName = "test case";
	rings[TEST_GROUP].ringName = "test group";
	rings[TEST_SESSION].ringName = "test session";
}

Testing & Testing::get()
{
	static Testing testing;
	return testing;
}

template<typename F, typename ... TS> bool Testing::run(Ring r, const char * name, const F & func, TS && ... params)
{
	if (r < 0 || (size_t) r > RINGS)
	{
		log("TESTSYSTEM: Error: Ring number %d does not exists!\n", static_cast<int>(r));
		log("TESTSYSTEM: Test skipped: %s\n", name ? name : "<anonymous>");
		return false;
	}
	
	start(r, name);
#if defined(__GNUC__) && !defined(__EXCEPTIONS)
	func(*this, static_cast<TS &&>(params) ...);
#else
	try
	{
		func(*this, static_cast<TS &&>(params) ...);
	}
	catch(std::exception & ex)
	{
		log("\tError: Unhandled std::exception: message: %s\n", ex.what());
		rings[r].failed = true;
	}
	catch(...)
	{
		log("\tError: Unhandled exception!\n");
		rings[r].failed = true;
	}
#endif
	finish(r);
	return true;
}


void Testing::assertPassed(size_t n)
{
	RingData & ring = rings[TEST_CASE];	
	if (!ring.active)
		log("TESTSYSTEM: Warning: Assertion called without active test case!\n");
		
	ring.success += n;
}

void Testing::assertFailed(size_t n)
{
	RingData & ring = rings[TEST_CASE];
	if (!ring.active)
		log("TESTSYSTEM: Warning: Assertion called without active test case!\n");
	
	good = false;
	ring.failed = true;
	ring.failure += n;
}

void Testing::log(const char * format, ...)
{
	va_list list;
    va_start(list, format);
	vfprintf(stdout, format, list);
	va_end(list);
}

void Testing::start(Ring r, const char * name)
{
	RingData & ring = rings[r];
	
	if (ring.active)
	{
		log("TESTSYSTEM: Warning: Interrupting active %s: %s", ring.ringName, ring.testName);
		finish(r);
	}
	
	ring.testName = name ? name : "<anonymous>";
	ring.active = true;
	ring.failed = false;
	ring.success = 0;
	ring.failure = 0;
	
	switch (r)
	{
		case TEST_CASE:
			log("\t[ RUN     ] Running test case: %s\n", ring.testName);
			break;
			
		case TEST_GROUP:
			log("\t[ ------- ] Starting group: %s\n", ring.testName);
			break;
			
		case TEST_SESSION:
			log("\n");
			log("\t------------------------------------------------------------\n");
			log("\t    Starting session: %s\n", ring.testName);
			log("\t------------------------------------------------------------\n");
			log("\n");
			break;
			
		default:;
	}
}

void Testing::finish(Ring r)
{
	RingData & ring = rings[r];
	
	if (!ring.active)
	{
		log("TESTSYSTEM: Warning: Cannot finish, no %s is currently active!", ring.ringName);
		return;
	}
	
	ring.active = false;
	
	if (!isHighestRing(r))
	{
		RingData & next = rings[r + 1];
		if (ring.failed)
			next.failure += 1;
		else
			next.success += 1;
	}
	
	switch (r)
	{
		case TEST_CASE:
			{
				const char * status = ring.failed ? "[   FAIL! ]" : "[     OK! ]";
				log("\t%s Finished! %d/%d passed (%d failed)\n", status, ring.success, ring.success + ring.failure, ring.failure);
			}
			break;
		case TEST_GROUP:
			{
				const char * status = ring.failed ? "[ FAILURE ]" : "[ SUCCESS ]";
				log("\t------------------------------------------------------------\n");
				log("\t%s Finished group: %s: %d/%d passed (%d failed)\n", status, ring.testName, ring.success, ring.success + ring.failure, ring.failure);
				log("\t------------------------------------------------------------\n");
			}
			break;
		case TEST_SESSION:
			{
				const char * status = ring.failed ? "[ FAILURE ]" : "[ SUCCESS ]";
				log("\n");
				log("\t------------------------------------------------------------\n");
				log("\t%s Finished session: %s: %d/%d passed (%d failed)\n", status, ring.testName, ring.success, ring.success + ring.failure, ring.failure);
				log("\t------------------------------------------------------------\n");
				log("\n");
			}
			break;
			
		default:;
	}	
}

size_t Testing::toString(char * buffer, size_t n, bool value)
{
	return snprintf(buffer, n, "%s", value ? "true" : "false");
}

size_t Testing::toString(char * buffer, size_t n, int64_t value)
{
	return snprintf(buffer, n, "%lld", value);
}

size_t Testing::toString(char * buffer, size_t n, uint64_t value)
{
	return snprintf(buffer, n, "%llu", value);
}

size_t Testing::toString(char * buffer, size_t n, double value)
{
	return snprintf(buffer, n, "%f", value);
}

size_t Testing::toString(char * buffer, size_t n, const void * value)
{
	return snprintf(buffer, n, "Pointer (%p)", value);
}

size_t Testing::toString(char * buffer, size_t n, const char * value)
{
	size_t len = 0; for (; (len < n) && value[len]; ++ len);
	if (len < n - 2)
		return snprintf(buffer, n, "\"%s\"", value);
	else
		return snprintf(buffer, n, "\"%*s ...\"", value, n - 7);
}

template<typename T> size_t Testing::toString(char * buffer, size_t n, T * value)
{
	if (value)
	{
		size_t m = snprintf(buffer, n, "Pointer (%p): ", static_cast<const void*>(value));
		return m + toString(buffer + m, n - m, *value);
	}
	else
	{
		return snprintf(buffer, n, "NULL");
	}
}

template<typename T> size_t Testing::toString(char * buffer, size_t n, const T & value)
{
	return snprintf(buffer, n, "Object (%p)", static_cast<const void *>(&value));
}

// close namespace "nx"
}

// Dual static buffers
constexpr size_t BUFFER_SIZE = 1024;
static char valueTextBuffer[BUFFER_SIZE];
static char exprTextBuffer[BUFFER_SIZE];

template<typename T> void expectEqual(typename nx::Testing::Id<const T &>::Result value, const T & expr)
{
	if (value == expr)
		nx::Testing::get().assertPassed();
	else
	{
		nx::Testing::get().assertFailed();
		// Log out details of the failed assert
		nx::Testing::toString(valueTextBuffer, BUFFER_SIZE, value);
		nx::Testing::toString(exprTextBuffer, BUFFER_SIZE, expr);
		nx::Testing::get().log("Error: Assertion failed: Expected %s, got %s\n", valueTextBuffer, exprTextBuffer);
	}
}
