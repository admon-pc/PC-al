#pragma once
#ifndef _ALFIFO_H_
#define _ALFIFO_H_

#include <mutex>
#include <condition_variable>

template<typename _type, uint32_t _size>
class alFIFO
{
	alFIFO(const alFIFO& other) {};
	alFIFO(alFIFO&& other) {};

	_type m_data[_size];
	uint32_t m_push = 0;
	uint32_t m_pop = 0;
	uint32_t m_curSize = 0;

	std::mutex m_mutex;

	/*
	// Blocking pop; waits until an element is available
    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condVar_.wait(lock, [this] { return !queue_.empty(); });
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }
	*/
	//std::condition_variable m_cv;

	void _pop()
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

public:
	alFIFO()
	{
	}

	~alFIFO()
	{
	}

	void push(const _type& item)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

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

	//	m_cv.notify_one();
	}

	const _type& get()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_data[m_pop];
	}
	const _type& get_pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		const _type& r = m_data[m_pop];

		_pop();

		return r;
	}

	void pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		_pop();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_curSize == 0;
	}
};



#endif

