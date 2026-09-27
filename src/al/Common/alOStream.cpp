#include "al.h"

#include "../al_internal.h"
extern alLibImpl* g_alLib;

alOStream& alOStream::get_default_ostream()
{
	return g_alLib->m_ostream_default;
}

alOStream_default::alOStream_default()
{
}

alOStream_default::~alOStream_default()
{
}

alOStream& alOStream_default::write(const char* s, streamsize l)
{
	return *this; 
}

alOStream& alOStream_default::write(const wchar_t* s, streamsize l)
{
	return *this; 
}

alOStream& alOStream_default::write(const char32_t* s, streamsize l)
{
	return *this; 
}

alOStream& alOStream_default::put(char c)
{
	return *this; 
}

alOStream& alOStream_default::put(wchar_t c)
{
	return *this; 
}

alOStream& alOStream_default::put(char32_t c)
{
	return *this; 
}

alOStream_default::pos_type alOStream_default::tellp() 
{
	return m_position; 
}

alOStream& alOStream_default::seekp(pos_type pos)
{
	return *this; 
}

alOStream& alOStream_default::seekp(off_type off, int dir/*SEEK_SET SEEK_CUR SEEK_END*/)
{
	return *this; 
}

alOStream& alOStream_default::flush()
{
	return *this; 
}

void alOStream_default::print(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprint(format, ap);
	va_end(ap);
}

void alOStream_default::print(const wchar_t* format, ...)
{
	va_list ap;
	va_start(ap, format);
	vprint(format, ap);
	va_end(ap);
}

void alOStream_default::print(const char32_t* format, ...)
{
	va_list args;
	va_start(args, format);
	vprint(format, args);
	va_end(args);	
}

void alOStream_default::vprint(const char* format, va_list arg)
{
	vsnprintf(m_buffer8, m_buffer8Size, format, arg);
	fprintf(m_stdout, m_buffer8);
}

void alOStream_default::vprint(const wchar_t* format, va_list arg)
{
	vswprintf(m_buffer16, m_buffer16Size, format, arg);
	wprintf_s(m_buffer16);
}

void alOStream_default::vprint(const char32_t* format, va_list arg)
{
	uint32_t result = alLib::vsnprintf(m_buffer32, m_buffer32Size, format, arg);
	alUnicodeConverter::char32_to_wchar(m_buffer32, m_buffer32Size, &g_alLib->m_ostream_bufferString);
	print(g_alLib->m_ostream_bufferString.c_str());
}

