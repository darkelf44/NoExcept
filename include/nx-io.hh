// Include guard
#pragma once

// Local includes
#include "nx-type.hh"
#include "nx-ptr.hh"

// Namespace "nx::io"
namespace nx { namespace io {
	
/**
	[INTERFACE] Peekable
	
	Any buffered input can trivially offer a single item peak ahead. This enables many useful algorithms, like reading
	until a certain value.
	
	Since this interface is very common, the stream classes provide a virtual method for it.
 */
template<typename T>
class Peekable
{
public:
	// Virtual interface
	virtual Maybe<T> peek() = 0;
	virtual Maybe<T> read() = 0;
};

// [ENUM] SeekMode
enum class SeekMode
{
	FromStart,
	FromEnd,
	FromHere,
};

/**
	[INTERFACE] Seekable
	
	File based inputs are usually seekable. Seeking involves telling the stream position and changing it. Streams that
	are both readable and writeable may have different positions for reading and writing.
	
	Since this interface is very common, the stream classes provide a virtual method for it.
 */
class Seekable
{
public:
	// Virtual interface
	virtual int64_t tell() = 0;
	virtual int64_t seek(int64_t position, SeekMode mode = SeekMode::FromStart) = 0;
};


template<typename T> class InputStream : public Object
{
public:
	// Virtual interface
	virtual intptr_t read(byte * buf, intptr_t len) = 0;
	virtual int64_t skip(int64_t n) = 0;
	virtual void close() = 0;
	
	// Optional interfaces
	virtual Seekable * asSeekable();
	virtual Peekable<T> * asPeekable();
	virtual OutputStream<T> * asOutput();
	
	// Methods
	Maybe<T> read();
	UniquePtr<Array<byte>> read(int64_t);
};

template<typename T> class OutputStream: public Object
{
public:
	// Virtual interface
	virtual intptr_t write(byte * buf, intptr_t len) = 0;
	virtual int64_t skip(int64_t n) = 0;
	virtual void close() = 0;
	
	// Optional interfaces
	virtual Seekable * asSeekable();
	virtual Peekable<T> * asPeekable();
	virtual InputStream<T> * asInput();
	
	// Methods
	void write(int32_t data);
	void write(Array<byte> * data);
};

// Byte streams
using IStream = InputStream<byte>;
using OStream = OutputStream<byte>;

class File : public Object
{
	File(const UString & path);
	
	
	
	virtual void close();
};

// Close namespace "nx::io"
}}
