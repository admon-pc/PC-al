#ifndef _ALARR_H_
#define _ALARR_H_

// `clear` must do m_size = 0;
template<typename type>
class alArray
{
	size_t     m_allocated = 0;
	void Reallocate(size_t new_capacity)
	{
		new_capacity += 4 * (m_allocated / 2);
		auto tmp_size = new_capacity * sizeof(type);
		type* new_data = static_cast<type*>(alMemory::Malloc(tmp_size));
		memset(new_data, 0, tmp_size);

		if (m_data)
		{
			for (size_t i = 0; i < m_size; ++i)
			{
				new(&new_data[i]) type(m_data[i]);
				(&m_data[i])->~type();
			}
			alMemory::Free(m_data);
		}
		m_data = new_data;
		m_allocated = new_capacity;
	}
public:
	alArray() :m_allocated(0), m_size(0), m_data(0) {}
	alArray(const alArray<type>& other)
	{
		assign(other);
	}
	alArray(alArray<type>&& other)
	{
		m_allocated = other.m_allocated;
		m_data = other.m_data;
		m_size = other.m_size;
		other.m_allocated = 0;
		other.m_data = 0;
		other.m_size = 0;
	}
	~alArray() { FreeMemory(); }

	alArray<type>& operator=(const alArray<type>& other)
	{
		assign(other);
		return *this;
	}
	alArray<type>& operator=(alArray<type>&& other)
	{
		m_allocated = other.m_allocated;
		m_data = other.m_data;
		m_size = other.m_size;
		other.m_allocated = 0;
		other.m_data = 0;
		other.m_size = 0;
		return *this;
	}

	void assign(const alArray<type>& other)
	{
		FreeMemory();
		if (other.m_allocated)
		{
			Reallocate(other.m_allocated);
			if (other.m_size)
			{
				for (size_t i = 0; i < m_size; ++i)
				{
					push_back(other.m_data[i]);
				}
			}
		}
	}

	const type& operator[](size_t id) const { return m_data[id]; }
	type& operator[](size_t id) { return m_data[id]; }

	size_t capacity()
	{
		return m_allocated;
	}

	size_t size()
	{
		return m_size;
	}

	void ShrinkToFit()
	{
		auto new_capacity = m_size;
		if (new_capacity)
		{
			auto tmp_size = new_capacity * sizeof(type);
			type* new_data = static_cast<type*>(alMemory::Malloc(tmp_size));
			memset(new_data, 0, tmp_size);

			if (m_data)
			{
				for (size_t i = 0; i < m_size; ++i)
				{
					new(&new_data[i]) type(m_data[i]);
					(&m_data[i])->~type();
				}
				alMemory::Free(m_data);
			}
			m_data = new_data;
			m_allocated = new_capacity;
		}
		else
		{
			FreeMemory();
		}
	}

	void reserve(size_t new_capacity)
	{
		if (new_capacity > m_allocated)
			Reallocate(new_capacity);
	}

	void insert(size_t where, const type& object)
	{
		if (where >= m_size)
		{
			push_back(object);
		}
		else
		{
			size_t new_size = m_size + 1;
			if (new_size > m_allocated)
				Reallocate(new_size);

			for (size_t i = m_size; i > where;)
			{
				m_data[i] = m_data[i - 1];
				--i;
			}
			new(&m_data[where]) type(object);

			m_size = new_size;
		}
	}

	void push_back(const type& object)
	{
		size_t new_size = m_size + 1;
		if (new_size > m_allocated)
			Reallocate(new_size);
		new(&m_data[m_size]) type(object);
		m_size = new_size;
	}

	void PushFront(const type& object)
	{
		insert(0, object);
	}

	void FreeMemory()
	{
		if (m_data)
		{
			for (size_t i = 0; i < m_allocated; ++i)
			{
				(&m_data[i])->~type();
			}
			alMemory::Free(m_data);

			m_allocated = m_size = 0;
			m_data = nullptr;
		}
	}

	void clear()
	{
		m_size = 0;
	}

	type* Get(size_t index)
	{
		if (index >= m_allocated)
			return nullptr;

		return &m_data[index];
	}

	/*
	struct _pred
	{
	bool operator() (const Map::renderNode& a, const Map::renderNode& b) const
	{
	return a.m_position.y > b.m_position.y;
	}
	};
	m_renderSprites.sort_insertion(_pred());
	*/
	template<class _Pr>
	void SortInsertion(_Pr _pred)
	{
		size_t i, j;
		type t;
		for (i = 1; i < m_size; ++i)
		{
			for (j = i; j > 0 && _pred(m_data[j - 1], m_data[j]); --j)
			{
				t = m_data[j];
				m_data[j] = m_data[j - 1];
				m_data[j - 1] = t;
			}
		}
	}

	void erase(size_t index)
	{
		for (size_t i = index; i < m_size; ++i)
		{
			if (i + 1 < m_size)
				m_data[i] = m_data[i + 1];
			else
				m_data[i] = 0;
		}
		if (m_size)
			--m_size;
	}

	bool EraseFirst(const type& ref)
	{
		for (size_t i = 0u; i < m_size; ++i)
		{
			if (m_data[i] == ref)
			{
				erase(i);
				return true;
			}
		}
		return false;
	}

	// --- Custom Iterator Class ---
	class Iterator {
	private:
		type* ptr;
	public:
		Iterator(type* p) : ptr(p) {}

		// 1. Dereference
		type& operator*() const { return *ptr; }
		type* operator->() { return ptr; }

		// 2. Pre-increment
		Iterator& operator++() {
			ptr++;
			return *this;
		}

		// Post-increment
		Iterator operator++(int) {
			Iterator tmp = *this;
			++(*this);
			return tmp;
		}

		// 3. Comparison
		bool operator==(const Iterator& other) const { return ptr == other.ptr; }
		bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
	};

	// --- Custom Const Iterator Class ---
	class ConstIterator {
	private:
		const type* ptr;
	public:
		ConstIterator(const type* p) : ptr(p) {}

		const type& operator*() const { return *ptr; }
		const type* operator->() { return ptr; }

		ConstIterator& operator++() {
			ptr++;
			return *this;
		}

		ConstIterator operator++(int) {
			ConstIterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const ConstIterator& other) const { return ptr == other.ptr; }
		bool operator!=(const ConstIterator& other) const { return ptr != other.ptr; }
	};

	// --- Container Begin / End Methods ---

	// Mutable iterations
	Iterator begin() { return Iterator(m_data); }
	Iterator end() { return Iterator(m_data + m_size); }

	// Const iterations
	ConstIterator begin() const { return ConstIterator(m_data); }
	ConstIterator end() const { return ConstIterator(m_data + m_size); }

	ConstIterator cbegin() const { return ConstIterator(m_data); }
	ConstIterator cend() const { return ConstIterator(m_data + m_size); }

	type* m_data = 0;
	size_t m_size = 0;
};



#endif

