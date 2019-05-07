// Include guard
#pragma once

// Local includes
#include "nx-core.hh"

// Namespace "nx"
namespace nx {

// ------------------------------------------------------------ //
//		Encoding
// ------------------------------------------------------------ //

enum class Encoding : uint32_t
{
	Unknown	= 0,
	
	// 1 byte char encodings
	UTF_8	= 100,	//	1-4 byte UTF-8
	ASCII	= 101,	//	7 byte ASCII
	
	// 2 byte char encodings
	UTF_16	= 200,
	
	// 4 byte char encodings
	UTF_32	= 400,
	
	// Windows codepages (1000000 - 1999999)
	
	// ISO codepages (as part of Windows codepages)
	ISO_8859_1 = 1028591,
	ISO_8859_2 = 1028592,
	ISO_8859_3 = 1028593,
	ISO_8859_4 = 1028594,
	ISO_8859_5 = 1028595,
	ISO_8859_6 = 1028596,
	ISO_8859_7 = 1028597,
	ISO_8859_8 = 1028598,
	ISO_8859_9 = 1028599,
	ISO_8859_10 = 1028600,
	ISO_8859_11 = 1028601,
	ISO_8859_12 = 1028602,
	ISO_8859_13 = 1028603,
	ISO_8859_14 = 1028604,
	ISO_8859_15 = 1028605,
	ISO_8859_16 = 1028606,
	
	// Custom encodings (2000000 - 3999999)
	CustomFirst = 2000000,
	CustomLast  = 3999999,
}

// Windows codepage to Encoding
inline constexpr Encoding codepage(uint32_t cp)
	{return cp < 1000000 ? 1000000 + cp : 0;}

// Namespace "nx::str::meta"
namespace str { namespace meta {
	
// Namespace "nx::str::meta::proto"
namespace proto {

// [MAPPING] EncodingToCharType - Maps an encoding to its native charaacter type
template<Encoding E> struct EncodingToCharType { using Result = nx::type::EnableIf<(E >= 1000000) || (E < 2000000) , char>; };
template<> EncodingToCharType<Encoding::ASCII> { using Result = char; };
template<> EncodingToCharType<Encoding::UTF_8> { using Result = char; };
template<> EncodingToCharType<Encoding::UTF_16> { using Result = char16_t; };
template<> EncodingToCharType<Encoding::UTF_32> { using Result = char32_t; };

// Close namespace "nx::str::meta::proto"
}

template<Encoding E> using EncodingToCharType = typename proto::EncodingToCharType<E>::Result;

// Close namespace "nx::str::meta"
}}


// ------------------------------------------------------------ //
//		Abstract String
// ------------------------------------------------------------ //

template<typename T> class AbstractString
{
public:
	// Character type
	using CharType = T;
	
	// Constructors & destructors
	AbstractString():
		n(0), text(nullptr) {}
	AbstractString(const char * str);
	AbstractString(const char * str, size_t len);

	// Copy & move
	AbstractString(AbstractString &&) = default;
	AbstractString(const AbstractString & str) : text(Array<CharType>::create(str.text)) {}
	AbstractString & operator = (AbstractString &&) = default;
	AbstractString & operator = (const AbstractString &) {text = Array<CharType>::create(str.text);}
	
	// Size
	inline size_t size() const
		{return text->length;}
	
	// Data
	const T * data() const
		{return text->data;}
	
	// Getters
	T get(size_t i) const
		{return text && (i < text->length) ? text->data[i] : T();}
	T get(size_t i, T def) const
		{return text && (i < text->length) ? text->data[i] : def;}
		
	// Operators
	T operator [] (size_t i) const
		{return text->data[i];}
		
private:
	// Only a single pointer
	UniquePtr<const Array<CharType>> text;
};


// ------------------------------------------------------------ //
//		Abstract String Buffer
// ------------------------------------------------------------ //

template<typename T> class AbstractStringBuffer
{
public:
	// Character type
	using CharType = T;
	
	// Constructors & destructors
	AbstractStringBuffer() {}
	AbstractStringBuffer(const char * str);
	AbstractStringBuffer(const char * str, size_t len);

	// TODO: this
private:
	List<CharType> text;
};


// ------------------------------------------------------------ //
//		String
// ------------------------------------------------------------ //

template<Encoding E> class String : public AbstractString<str::meta::EncodingToCharType<E>>
{
public:
	using CharType = str::meta::EncodingToCharType<E>;
	using BaseType = AbstractString<CharType>;
	
	// Constructors
	String() {}
	explicit String(const char * str) : BaseType(str) {}
	explicit String(const char * str, size_t len) : BaseType(str, len) {}
	
	// Methods
	bool startsWith(const String & str) const;
	bool endsWith(const String & str) const;
	
	String removeStart(const String & str) const;
	String removeEnd(const String & str) const;
	
	String replaceStart(const String & str, const String & repl);
	String replaceEnd(const String & str, const String & repl);
	
	String slice(intptr_t from) const;
	String slice(intptr_t from, intptr_t until) const;
	
	String splice(const String & str, intptr_t from) const;
	String splice(const String & str, intptr_t from, intptr_t until) const;
	
	String replace(const String & str, const String & repl, intptr_t count = -1) const;
	
	String join(const List<String> & list) const;
	String join(const Iterable<String> & list) const;
	
	List<String> split(intptr_t maxsplit = -1) const;
	List<String> split(const String & str, intptr_t maxsplit = -1) const;
	List<String> rsplit(intptr_t maxsplit = -1) const;
	List<String> rsplit(const String & str, intptr_t maxsplit = -1) const;
	
	intptr_t find(CharType ch) const;
	intptr_t find(CharType ch, intptr_t from) const;
	intptr_t find(CharType ch, intptr_t from, intptr_t until) const;
	intptr_t rfind(CharType ch) const;
	intptr_t rfind(CharType ch, intptr_t from) const;
	intptr_t rfind(CharType ch, intptr_t from, intptr_t until) const;
	
	String strip() const;
	String strip(CharType ch) const;
	String strip(const List<CharType> & str) const;
	String lstrip() const;
	String lstrip(CharType ch) const;
	String lstrip(const List<CharType> & str) const;
	String rstrip() const;
	String rstrip(CharType ch) const;
	String rstrip(const List<CharType> & str) const;
	
	String ljust(size_t width) const;
	String ljust(size_t width, CharType fill) const;
	String rjust(size_t width) const;
	String rjust(size_t width, CharType fill) const;
}

// Alias for unicode strings
using UString = String<Encoding::UTF_8>;
using WString = String<Encoding::UTF_16>;


// ------------------------------------------------------------ //
//		String Buffer
// ------------------------------------------------------------ //

template<Encoding E> class StringBuffer : public AbstractStringBuffer<str::meta::EncodingToCharType<E>>
{
	// TODO: this
}

// Alias for unicode writeable
using UStringBuffer = StringBuffer<Encoding::UTF_8>;
using WStringBuffer = StringBuffer<Encoding::UTF_16>;

// Close namespace "nx"
}
