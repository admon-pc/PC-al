#ifndef _AL_TEXTREADER_H_
#define _AL_TEXTREADER_H_

#include "String/alUnicodeString.h"

template<typename _type>
class alTextReader
{
	const _type* m_data = 0;
	const _type* m_curPos = 0;
	const _type* m_end = 0;
	alUnicodeString ustr;

	bool _is_space(_type c)
	{
		switch (c)
		{
		case ' ':
		case '\r':
		case '\n':
		case '\v':
		case '\f':
		case '\t':
			return true;
		}
		return false;
	}

public:
	alTextReader(const _type* ptr, size_t size) :
		m_data(ptr)
	{
		GoToBegin();
		m_end = m_data + size;
	}
	~alTextReader() {}
	void GoToBegin() { m_curPos = m_data; }
	bool IsEnd() { return m_curPos >= m_end; }

	void SkipSpaces()
	{
		while (*m_curPos)
		{
			if (*m_curPos < 200)
			{
				if (_is_space(*m_curPos))
					++m_curPos;
				else
					break;
			}
			else
				break;
		}
	}

	alTextReader<_type> GetLine(bool skipSpaces)
	{
		if (skipSpaces)
			SkipSpaces();

		const _type* begin = m_curPos;
		size_t size = 0;

		while (*m_curPos)
		{
			++size;
			++m_curPos;

			if (*m_curPos == U'\n')
			{
				break;
			}
		}

		return alTextReader<_type>(begin, size);
	}

	uint32_t GetUint()
	{
		ustr.Clear();
		SkipSpaces();
		while (*m_curPos)
		{
			if (_is_space(*m_curPos))
				break;
			else
			{
				ustr.PushBack((char32_t)*m_curPos);
				++m_curPos;
			}
		}
		return ustr.ToUint();
	}

	int32_t GetInt()
	{
		ustr.Clear();
		SkipSpaces();
		while (*m_curPos)
		{
			if (_is_space(*m_curPos))
				break;
			else
			{
				ustr.PushBack((char32_t)*m_curPos);
				++m_curPos;
			}
		}
		return ustr.ToInt();
	}

	float32_t GetFloat()
	{
		ustr.Clear();
		SkipSpaces();
		while (*m_curPos)
		{
			if (_is_space(*m_curPos))
				break;
			else
			{
				ustr.PushBack((char32_t)*m_curPos);
				++m_curPos;
			}
		}
		return ustr.ToFloat();
	}
};

#endif
