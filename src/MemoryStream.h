#ifndef MEMORY_STREAM_H
#define MEMORY_STREAM_H

enum class Endianness {
	LittleEndian,
	BigEndian
};

constexpr Endianness STREAM_ENDIANNESS = Endianness::BigEndian;
constexpr Endianness PLATFORM_ENDIANNESS = Endianness::LittleEndian;

constexpr uint32_t DEFAULT_STREAM_SIZE = 32;

class OutputMemoryStream
{
public:

	// Constructor
	OutputMemoryStream(uint32_t inSize = DEFAULT_STREAM_SIZE):
		mBuffer(nullptr), mHead(0), mCapacity(0)
	{ ReallocBuffer(inSize); }

	// Destructor
	~OutputMemoryStream()
	{ std::free(mBuffer); }

	// Get pointer to the data in the stream
	const char *GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }

	// Write method
	void Write(const void *inData, size_t inByteCount);

	// Generic write for arithmetic types
	template< typename T >
	void Write( T inData )
	{
		static_assert(
				std::is_arithmetic< T >::value ||
				std::is_enum< T >::value,
				"Generic Write only supports primitive data types" );

		if( STREAM_ENDIANNESS == PLATFORM_ENDIANNESS )
		{
			Write( &inData, sizeof( inData ) );
		}
		else
		{
			T swappedData = ByteSwap( inData );
			Write( &swappedData, sizeof( swappedData ) );
		}
	}

	// Generic write for vectors of arithmetic types
	template< typename T >
	void Write( const std::vector< T >& inVector )
	{
		uint32_t elementCount = inVector.size();
		Write( elementCount );
		for( const T& element : inVector )
		{
			Write( element );
		}
	}

	// Write for strings
	void Write( const std::string& inString )
	{
		size_t elementCount = inString.size() ;
		Write( elementCount );
		Write( inString.data(), elementCount * sizeof( char ) );
	}
	

private:

	// Resize the buffer
	void ReallocBuffer(uint32_t inNewLength);

	char *mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;
};

class InputMemoryStream
{
public:

	// Constructor
	InputMemoryStream(uint32_t inSize = DEFAULT_STREAM_SIZE) :
		mBuffer(static_cast<char*>(std::malloc(inSize))), mCapacity(inSize), mHead(0)
	{ }

	// Destructor
	~InputMemoryStream()
	{ std::free(mBuffer); }

	// Returns the number of bytes not read yet
	uint32_t GetRemainingDataSize() const
	{ return mCapacity - mHead; }

	// Read method
	void Read(void *outData, size_t inByteCount);

	// Generic read for arithmetic types
	template< typename T >
	void Read( T& outData )
	{
		static_assert(
				std::is_arithmetic< T >::value ||
				std::is_enum< T >::value,
				"Generic Read only supports primitive data types" );

		if( STREAM_ENDIANNESS == PLATFORM_ENDIANNESS )
		{
			Read( &outData, sizeof( outData ) );
		}
		else
		{
			T swappedData = ByteSwap( outData );
			Read( &swappedData, sizeof( outData ) );
		}
	}

	// Generic read for vectors of arithmetic types
	template< typename T >
	void Read( std::vector< T >& outVector )
	{
		size_t elementCount;
		Read( elementCount );
		outVector.resize( elementCount );
		for( const T& element : outVector )
		{
			Read( element );
		}
	}

private:

	char *mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;
};

#endif // MEMORY_STREAM_H
