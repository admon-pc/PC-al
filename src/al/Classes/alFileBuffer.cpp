#include "al.h"

alFileBuffer::alFileBuffer()
{
}

alFileBuffer::alFileBuffer(uint32_t size)
{
	m_size = size;
	m_data = (uint8_t*)alMemory::Malloc(m_size);
	m_pointer = m_data;
	m_end = m_data + m_size;
}


alFileBuffer::~alFileBuffer()
{
	Free();
}

bool alFileBuffer::ReadFile(const char* fn)
{
	bool ret = false;
	AL_ASSERT_ST(fn);
	if (fn)
	{
		uint64_t fileSz = alLib::GetFileSize(fn);
		if (fileSz)
		{
			FILE* f = 0;
			fopen_s(&f, fn, "rb");
			if (f)
			{
				m_size = (size_t)fileSz;
				m_data = (uint8_t*)alMemory::Malloc(m_size);
				m_pointer = m_data;
				m_end = m_data + m_size;
				fread_s(m_data, m_size, m_size, 1, f);
				fclose(f);
				ret = true;
			}
		}
	}
	return ret;
}

void alFileBuffer::Free()
{
	if (m_data)
	{
		alMemory::Free(m_data);
		m_data = 0;
		m_size = 0;
		m_end = 0;
		m_pointer = 0;
	}
}

void alFileBuffer::Seek(size_t offset, int origin)
{
	switch (origin)
	{
	default:
	case SEEK_SET:
		m_pointer = m_data + offset;
		break;
	case SEEK_CUR:
		m_pointer += offset;
		if (m_pointer >= m_end)
		{
			m_pointer = m_end;
		}
		break;
	case SEEK_END:
		m_pointer = m_end;
		break;
	}
}

size_t alFileBuffer::Tell()
{
	return (size_t)m_pointer;
}

bool alFileBuffer::IsEOF()
{
	return m_pointer >= m_end;
}

bool alFileBuffer::IsOpen()
{
	return m_data != nullptr;
}

size_t alFileBuffer::Read(void* _buf, size_t sz)
{
	AL_ASSERT_ST(_buf);
	AL_ASSERT_ST(sz);

	size_t ret = 0;
	uint8_t* buf = (uint8_t*)_buf;
	for (size_t i = 0; i < sz; ++i)
	{
		if (m_pointer < m_end)
		{
			buf[i] = *m_pointer;
			++m_pointer;
			++ret;
		}
		else
			break;
	}

	return ret;
}
