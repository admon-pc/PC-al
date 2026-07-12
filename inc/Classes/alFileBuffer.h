#ifndef _AL_FILBUF_H_
#define _AL_FILBUF_H_

class alFileBuffer
{
	uint8_t* m_data = 0;
	
	uint8_t* m_pointer = 0;
	uint8_t* m_end = 0;

	size_t m_size = 0;
public:
	alFileBuffer();
	alFileBuffer(uint32_t size);
	~alFileBuffer();

	uint8_t* Data() { return m_data; }
	size_t Size() { return m_size; }

	size_t Read(void* buf, size_t sz);

	void Seek(size_t offset, int origin);
	size_t Tell();

	bool IsEOF();
	bool IsOpen();

	bool ReadFile(const char*);
	void Free();
};

#endif
