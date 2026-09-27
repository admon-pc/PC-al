#pragma once
#ifndef _AL_OSTREAMIMPL_H_
#define _AL_OSTREAMIMPL_H_

class alOStream_default : public alOStream
{
	enum 
	{
		m_buffer32Size = 1024,
		m_buffer16Size = 2048,
		m_buffer8Size = 4096,
	};
	char32_t m_buffer32[m_buffer32Size];
	wchar_t  m_buffer16[m_buffer16Size];
	char m_buffer8[m_buffer8Size];

public:
	alOStream_default();
	virtual ~alOStream_default();

	using streamsize = size_t;
	using pos_type = size_t;
	using off_type = size_t;

	virtual alOStream& write(const char* s, streamsize l) override;
	virtual alOStream& write(const wchar_t* s, streamsize l) override;
	virtual alOStream& write(const char32_t* s, streamsize l) override;

	virtual alOStream& put(char c) override;
	virtual alOStream& put(wchar_t c) override;
	virtual alOStream& put(char32_t c) override;

	virtual pos_type tellp() override;
	virtual alOStream& seekp(pos_type pos) override;
	virtual alOStream& seekp(off_type off, int dir/*SEEK_SET SEEK_CUR SEEK_END*/) override;

	virtual alOStream& flush() override;

	virtual void print(const char* format, ...) override;
	virtual void print(const wchar_t* format, ...) override;
	virtual void print(const char32_t* format, ...) override;

	virtual void vprint(const char* format, va_list arg) override;
	virtual void vprint(const wchar_t* format, va_list arg) override;
	virtual void vprint(const char32_t* format, va_list arg) override;

	virtual void on_write(const char*) override;
	virtual void on_write(const wchar_t*) override;
	virtual void on_write(const char32_t*) override;
};

#endif

