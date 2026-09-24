#pragma once
#ifndef _ALFIFO_H_
#define _ALFIFO_H_

template<typename _type, uint32_t _size>
class alFIFO
{
	alFIFO(const alFIFO& other) {};
	alFIFO(alFIFO&& other) {};

	_type m_data[_size];
	uint32_t m_push = 0;
	uint32_t m_pop = 0;
	uint32_t m_curSize = 0;
public:
	alFIFO()
	{
	}

	~alFIFO()
	{
	}

	void push(const _type& item)
	{
		if ((m_push == m_pop) && (m_curSize == _size))
			++m_pop;

		if (m_pop == _size)
			m_pop = 0;

		m_data[m_push] = item;
		++m_push;
		if (m_push == _size)
			m_push = 0;


		++m_curSize;
		if (m_curSize > _size)
			--m_curSize;
	}

	const _type& get()
	{
		return m_data[m_pop];
	}

	void pop()
	{
		if (m_curSize)
		{
			++m_pop;
			if (m_pop == _size)
				m_pop = 0;

			--m_curSize;
		}
		else
		{
			m_push = 0;
			m_pop = 0;
		}
	}
};



#endif

