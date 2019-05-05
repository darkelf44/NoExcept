// Include guard
#pragma once

// Local includes
#include "nx-core.hh"

// Namespace "nx::meta" - Meta programming resources
namespace nx { namespace meta {

// [META TYPE] List - List of types
template<typename... TS> struct List {};

// [META TYPE] Value - Wraps a constexpr value
template<typename T, T v> struct Value {static constexpr T result = v;};

// Bool types
template<bool v> using Bool = Value<bool, v>;

// Integer types
template<int8_t v>  using Int8  = Value<int8_t, v>;
template<int16_t v> using Int16 = Value<int16_t, v>;
template<int32_t v> using Int32 = Value<int32_t, v>;
template<int64_t v> using Int64 = Value<int64_t, v>;
template<uint8_t v>  using UInt8  = Value<uint8_t, v>;
template<uint16_t v> using UInt16 = Value<uint16_t, v>;
template<uint32_t v> using UInt32 = Value<uint32_t, v>;
template<uint64_t v> using UInt64 = Value<uint64_t, v>;

template<intptr_t v> using IntPtr = Value<intptr_t, v>;
template<uintptr_t v> using UIntPtr = Value<uintptr_t, v>;


// close namespace "nx::meta"
}}
