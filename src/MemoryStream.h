#ifndef MEMORY_STREAM_H
#define MEMORY_STREAM_H

class OutputMemoryStream
{
public:

	// Constructor
	OutputMemoryStream():
		mBuffer(nullptr), mHead(0), mCapacity(0)
	{ ReallocBuffer(32); }

	// Destructor
	~OutputMemoryStream()
	{ std::free(mBuffer); }

	// Get pointer to the data in the stream
	const char *GetBufferPtr() const { return mBuffer; }
	uint32_t GetLength() const { return mHead; }

	// Write methods
	void Write(const void *inData, size_t inByteCount);
	void Write(uint32_t inData) { Write(&inData, sizeof(inData)); }
	void Write(int32_t inData) { Write(&inData, sizeof(inData)); }

private:

	void ReallocBuffer(uint32_t inNewLength);

	char *mBuffer;
	uint32_t mHead;
	uint32_t mCapacity;
};

#endif // MEMORY_STREAM_H
