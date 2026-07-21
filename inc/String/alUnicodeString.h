#ifndef _AL_USTRINGH_
#define _AL_USTRINGH_

class alUnicodeConverter
{
	void _find16From32();
	void _find8From32();

public:
	alUnicodeConverter() {}
	~alUnicodeConverter() {}

	void Set(char32_t c)
	{
		m_32 = c;
		_find16From32();
		_find8From32();
	}

	void Set(char16_t c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		_find8From32();
	}

	void Set(char8_t c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		m_8[0] = c;
		m_8[1] = 0;
		m_8[2] = 0;
		m_8[3] = 0;
		m_8Num = 1;
	}
	
	void Set(char c)
	{
		m_32 = c;
		m_16[0] = c;
		m_16[1] = 0;
		m_16Num = 1;
		m_8[0] = c;
		m_8[1] = 0;
		m_8[2] = 0;
		m_8[3] = 0;
		m_8Num = 1;
	}

	void Set(wchar_t c)
	{
		Set((char32_t)c);
	}

	uint32_t Set(const alVec4_t<uint32_t>& c);
	uint32_t Set(char16_t c1, char16_t c2);

	static void wchar_to_char(const wchar_t* str, size_t sz, alStringA* out);
	static void char_to_wchar(const char* str, size_t sz, alStringW* out);

	uint32_t m_32 = 0;
	uint16_t m_16[2] = { 0, 0 };
	uint8_t m_8[4] = {0, 0, 0, 0};

	uint32_t m_16Num = 0;
	uint32_t m_8Num = 0;
};

class alUnicodeString
{
	size_t m_size = 0;
	size_t m_allocated = 0;
	char32_t* m_data = 0;
	void _reallocate(size_t new_allocated);
	void _free();

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

public:
	alUnicodeString();
	alUnicodeString(const char*);
	alUnicodeString(const wchar_t*);
	alUnicodeString(const char8_t*);
	alUnicodeString(const char16_t*);
	alUnicodeString(const char32_t*);
	alUnicodeString(const alUnicodeString&);
	alUnicodeString(alUnicodeString&&) noexcept;
	~alUnicodeString();

	void Assign(const char*);
	void Assign(const wchar_t*);
	void Assign(const char8_t*);
	void Assign(const char16_t*);
	void Assign(const char32_t*);
	void Assign(const alUnicodeString&);

	size_t Size();
	size_t size() { return m_size; }
	size_t Capacity();
	void Reserve(size_t);
	void Clear();
	void PopBack();
	void PushBack(char32_t);
	char32_t* Data();
	char32_t* data() {return m_data;}
	const char32_t* c_str();
	void Flip();

	void Insert(char32_t c, size_t where);

	void Append(const char*);
	void Append(const char8_t*);
	void Append(const wchar_t*);
	void Append(const char16_t*);
	void Append(const char32_t*);
	void Append(const char32_t*, size_t size);
	void Append(const alUnicodeString&);
	void Append(char32_t);
	void Append(uint32_t);
	void Append(uint64_t);
	void Append(int32_t);
	void Append(int64_t);
	void Append(float32_t);
	void AppendFloat(float32_t);
	void Append(float64_t);
	
	int32_t ToInt();
	uint32_t ToUint();
	float32_t ToFloat();
	float64_t ToFloat64();
	void ToUTF8(alStringA&);
	void ToUTF16(alStringW&);

	// return is encoding type
	// 1 - utf8
	// 2 - utf16
	// 3 - utf16 big endian
	// 4 - utf32
	// 5 - utf32 big endian
	uint32_t ReadFromFile(const char*);

	void SaveToFileUTF8(const char*, bool addBOM = true);
	void SaveToFileUTF16(const char*, bool addBOM = true);
	void SaveToFileUTF32(const char*, bool addBOM = true);

	alUnicodeString& operator=(const alUnicodeString&);
	alUnicodeString& operator=(alUnicodeString&&) noexcept;

	void operator+=(const char* str) { Append(str); }
	void operator+=(const char8_t* str) { Append(str); }
	void operator+=(const char16_t* str) { Append(str); }
	void operator+=(const char32_t* str) { Append(str); }


	//m_size = size;
	void _set_size(size_t size);
};

#endif

