#ifndef _AL_STRINGH_
#define _AL_STRINGH_



namespace al_internal
{
	const size_t string_wordSize = 16;
	const float32_t string_to_float_table[17] =
	{
		0.f,
		0.1f,
		0.01f,
		0.001f,
		0.0001f,
		0.00001f,
		0.000001f,
		0.0000001f,
		0.00000001f,
		0.000000001f,
		0.0000000001f,
		0.00000000001f,
		0.000000000001f,
		0.0000000000001f,
		0.00000000000001f,
		0.000000000000001f,
		0.0000000000000001f,
	};
}

template<typename char_type>
class alString_base
{
	typedef char_type* pointer;
	typedef const char_type* const_pointer;
	typedef char_type& reference;
	typedef const char_type& const_reference;
	typedef alString_base this_type;
	typedef const alString_base& this_const_reference;

	pointer m_data;
	size_t     m_allocated;
	size_t     m_size;

	void Reallocate(size_t new_allocated)
	{
		char_type* new_data = (char_type*)alMemory::Malloc(new_allocated * sizeof(char_type));
		if (m_data)
		{
			std::memcpy(new_data, m_data, m_size * sizeof(char_type));
			alMemory::Free(m_data);
		}
		else
		{
			std::memset(new_data, 0, new_allocated);
		}
		m_data = new_data;
		m_allocated = new_allocated;
	}

	template<typename other_type>
	size_t Getlen(const other_type* str)
	{
		uint32_t len = 0;
		if (str[0] != 0)
		{
			const other_type* p = &str[0];
			while ((size_t)*p++)
				len++;
		}
		return len;
	}

	template<typename dst_type, typename src_type>
	void Copy(dst_type* dst, src_type* src) const
	{
		while ((size_t)*src)
		{
			*dst = static_cast<dst_type>(*src);
			dst++;
			src++;
		}
	}

	void _free()
	{
		if (m_data)
			alMemory::Free(m_data);
	}

public:
	typedef char_type value_type;

	alString_base()
		:m_data(nullptr), m_allocated(al_internal::string_wordSize), m_size(0)
	{
		Reallocate(m_allocated);
	}

	template<typename other_type>
	alString_base(const other_type* str)
		: m_data(nullptr), m_allocated(al_internal::string_wordSize), m_size(0)
	{
		Reallocate(m_allocated);
		assign(str);
	}

	alString_base(this_const_reference str)
		: m_data(nullptr), m_allocated(al_internal::string_wordSize), m_size(0)
	{
		Reallocate(m_allocated);
		assign(str);
	}

	alString_base(this_type&& str)
		: m_data(nullptr), m_allocated(al_internal::string_wordSize), m_size(0)
	{
		m_allocated = str.m_allocated;
		m_data = str.m_data;
		m_size = str.m_size;
		str.m_allocated = 0;
		str.m_data = 0;
		str.m_size = 0;
	}

	alString_base(char_type c)
		: m_data(nullptr), m_allocated(al_internal::string_wordSize), m_size(0)
	{
		size_t new_size = 1;
		Reallocate((new_size + 1) + al_internal::string_wordSize);
		m_data[0] = c;
		m_size = new_size;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	~alString_base()
	{
		_free();
	}

	size_t capacity()
	{
		return m_allocated; 
	}

	void ToLower() 
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			auto c = m_data[i];
			if (c <= 'Z' && c >= 'A')
				m_data[i] += 32;
		}
	}

	//	abc -> cba
	void Flip()
	{
		if (!m_size)
			return;
		alString_base<char_type> flippedStr;
		for (size_t i = m_size - 1; i >= 0; --i)
		{
			flippedStr += m_data[i];
			if (!i)
				break;
		}
		this->operator=(flippedStr);
	}

	// '\\' -> '/'
	void FlipSlash()
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == '\\')
				m_data[i] = '/';
		}
	}

	// '/' -> '\\'
	void FlipBackslash()
	{
		if (!m_size)
			return;
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == '/')
				m_data[i] = '\\';
		}
	}

	void reserve(size_t size)
	{
		if (size > m_allocated)
		{
			Reallocate(size);
			m_data[m_size] = 0;
		}
	}

	template<typename other_type>
	void assign(other_type str) 
	{
		m_size = 0;
		m_data[m_size] = static_cast<char_type>(0);
		if (str)
			append(str);
	}

	void assign(this_const_reference str)
	{
		m_size = 0u;
		m_data[m_size] = static_cast<char_type>(0x0);
		append(str);
	}

	template<typename other_type>
	void append(const other_type* str)
	{
		size_t new_size = Getlen(str) + m_size;

		if ((new_size + 1u) > m_allocated)
			Reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

		Copy(&m_data[m_size], str);

		m_size = new_size;
		m_data[m_size] = static_cast<char_type>(0x0);
	}

	void append(this_const_reference str)
	{
		append(str.data());
	}

	void push_back(char_type c)
	{
		size_t new_size = m_size + 1;
		if ((new_size + 1u) > m_allocated)
			Reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));
		m_data[m_size] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}

	void append(char_type c) 
	{
		size_t new_size = m_size + 1;
		if ((new_size + 1u) > m_allocated)
			Reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));
		m_data[m_size] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}

	void append(int32_t c) 
	{
		char buf[32];
		sprintf_s(buf, 32, "%i", c);
		append(buf);
	}

	void append(uint32_t c)
	{
		char buf[32];
		sprintf_s(buf, 32, "%u", c);
		append(buf);
	}

	void append(uint64_t c)
	{
		char buf[32];
		sprintf_s(buf, 32, "%llu", c);
		append(buf);
	}

	void append(float64_t c) 
	{
		char buf[32];
		sprintf_s(buf, 32, "%.12f", c);
		append(buf);
	}

	void append(float32_t c)
	{
		char buf[32];
		sprintf_s(buf, 32, "%.4f", c);
		append(buf);
	}
	void AppendFloat(float32_t c) 
	{
		char buf[32];
		sprintf_s(buf, 32, "%.7f", c);
		append(buf);
	}

	const_pointer c_str() const { return m_data; }
	pointer data() const { return m_data; }
	const size_t size() const { return m_size; }

	void SetSize(size_t size)
	{
		m_size = size;
	}

	this_type& operator=(this_const_reference str) 
	{
		assign(str);
		return *this;
	}

	this_type& operator=(this_type&& str)
	{
		_free();
		m_allocated = str.m_allocated;
		m_data = str.m_data;
		m_size = str.m_size;
		str.m_allocated = 0;
		str.m_data = 0;
		str.m_size = 0;
		return *this;
	}


	template<typename other_type>
	this_type& operator=(other_type* str)
	{
		assign(str);
		return *this;
	}

	this_type operator+(const_pointer str)
	{
		this_type ret(*this);
		ret.append(str);
		return ret;
	}

	this_type operator+(this_const_reference str) 
	{
		return operator+(str.data());
	}

	this_type operator+(size_t num) {
		this_type r(*this);
		r.append(num);
		return r;
	}

	pointer begin() const { return m_data; }
	pointer end() const { return (m_data + (m_size)); }
	const_reference operator[](size_t i) const { return m_data[i]; }
	reference operator[](size_t i) { return m_data[i]; }
	void operator+=(int32_t i) { append(i); }
	void operator+=(uint32_t i) { append(i); }
	void operator+=(uint64_t i) { append(i); }
	void operator+=(float32_t i) { append(i); }
	void operator+=(float64_t i) { append(i); }
	void operator+=(char_type c) { append(c); }

	template<typename other_type>
	void operator+=(other_type* str) { append(str); }
	void operator+=(this_const_reference str) { append(str); }

	bool operator==(this_const_reference other) const 
	{
		if (other.size() != m_size)
			return false;

		const size_t sz = other.size();
		const auto* ptr = other.data();
		for (size_t i = 0; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return false;
		}

		return true;
	}

	bool operator!=(this_const_reference other) const 
	{
		if (other.size() != m_size)
			return true;

		const size_t sz = other.size();
		const auto* ptr = other.data();
		for (size_t i = 0; i < sz; ++i)
		{
			if (ptr[i] != m_data[i])
				return true;
		}

		return false;
	}

	void clear()
	{
		m_size = 0;
		m_data[m_size] = 0;
	}

	void pop_back()
	{
		if (m_size)
		{
			--m_size;
			m_data[m_size] = 0;
		}
	}

	char_type PopBackReturn()
	{
		char_type r = '?';
		if (m_size)
		{
			--m_size;
			r = m_data[m_size];
			m_data[m_size] = 0;
		}
		return r;
	}

	void Replace(char_type a, char_type b)
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			if (m_data[i] == a)
				m_data[i] = b;
		}
	}

	void PopBackBefore(char_type before_this)
	{
		if (size())
			pop_back();

		if (size())
		{
			for (size_t i = size() - 1; i >= 0; --i)
			{
				if (m_data[i] == (char_type)before_this)
					break;
				else
					pop_back();
				if (!i)
					break;
			}
		}
	}

	void shrink_to_fit()
	{
		if (m_size)
		{
			if (m_allocated > (m_size + (1 + (size_t)(m_size * 0.5f))))
			{
				Reallocate(m_size + 1);
				m_data[m_size] = 0;
			}
		}
	}

	void erase(size_t begin, size_t end)
	{
		size_t numCharsToDelete = end - begin + 1; // delete first char: 0 - 0 + 1
		size_t next = end + 1;
		for (size_t i = begin; i < m_size; ++i)
		{
			if (next < m_size)
			{
				m_data[i] = m_data[next];
				++next;
			}
		}
		m_size -= numCharsToDelete;
		m_data[m_size] = 0;
	}

	void PopFront() 
	{
		erase(0, 0);
	}

	bool IsSpace(size_t index) 
	{
		if (m_data[index] == (char_type)' ') return true;
		if (m_data[index] == (char_type)'\r') return true;
		if (m_data[index] == (char_type)'\n') return true;
		if (m_data[index] == (char_type)'\t') return true;
		return false;
	}

	/*alString_base<wchar_t> to_string() const
	{
		alString_base<wchar_t> ret;
		ret.reserve(m_size);
		ret = m_data;
		return ret;
	}*/

	int32_t ToInt()
	{
		size_t len = m_size;
		int32_t result = 0;
		int32_t mul_val = 1;
		bool is_neg = m_data[0] == (char_type)'-';
		for (size_t i = 0, last = len - 1; i < len; ++i)
		{
			int32_t char_value = (int32_t)m_data[last] - 0x30;
			if (char_value >= 0 && char_value <= 9)
			{
				result += char_value * mul_val;
				mul_val *= 10;
			}
			--last;
		}
		if (is_neg)
			result = -result;
		return result;
	}

	float32_t ToFloat()
	{
		float32_t result = 0.f;
		auto ptr = m_data;
		bool is_negative = *ptr == (char_type)'-';

		if (is_negative)
			++ptr;

		int32_t i = 0;
		while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
		{
			i *= 10;
			i += *ptr - (char_type)'0';
			++ptr;
		}
		result = (float32_t)i;

		i = 0;

		if (*ptr == (char_type)'.')
			++ptr;

		int32_t part_2_count = 0;
		while (*ptr >= (char_type)'0' && *ptr <= (char_type)'9')
		{
			i *= 10;
			i += *ptr - (char_type)'0';
			++ptr;
			++part_2_count;
		}
		result += (float32_t)i * al_internal::string_to_float_table[part_2_count];

		return is_negative ? -result : result;
	}

	template<typename other_type>
	void Insert(const other_type* str, size_t where) 
	{
		size_t str_len = Getlen(str);
		size_t new_size = str_len + m_size;
		if ((new_size + 1) > m_allocated)
			Reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

		size_t i = m_size;
		while (i >= where)
		{
			m_data[i + str_len] = m_data[i];
			if (i == 0)
				break;
			--i;
		}
		for (size_t i = 0; i < str_len; ++i)
		{
			m_data[i + where] = str[i];
		}
		m_size = new_size;
		m_data[m_size] = 0;
	}

	void Insert(char_type c, size_t where)
	{
		size_t new_size = m_size + 1;
		if ((new_size + 1u) > m_allocated)
			Reallocate((new_size + 1) + (1 + (size_t)(m_size * 0.5f)));

		size_t i = m_size;
		while (i >= where)
		{
			auto next = i + 1;
			if (next < m_allocated)
				m_data[next] = m_data[i];

			if (i == 0)
				break;
			--i;
		}
		m_data[where] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}
};

typedef alString_base<wchar_t> alStringW;
typedef alString_base<char> alStringA;

#endif

