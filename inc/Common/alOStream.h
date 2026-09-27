#pragma once
#ifndef _AL_OSTREAM_H_
#define _AL_OSTREAM_H_

class alOStream
{
public:
	alOStream() {}
	virtual ~alOStream() {}

	using streamsize = size_t;
	using pos_type = size_t;
	using off_type = size_t;

	virtual alOStream& write(const char* s, streamsize l) { return *this; }
	virtual alOStream& write(const wchar_t* s, streamsize l) { return *this; }
	virtual alOStream& write(const char32_t* s, streamsize l) { return *this; }

	virtual alOStream& put(char c) { return *this; }
	virtual alOStream& put(wchar_t c) { return *this; }
	virtual alOStream& put(char32_t c) { return *this; }

	virtual pos_type tellp() { return m_position; }
	virtual alOStream& seekp(pos_type pos) { return *this; }
	virtual alOStream& seekp(off_type off, int dir/*SEEK_SET SEEK_CUR SEEK_END*/) { return *this; }

	virtual alOStream& flush() { return *this; }

	virtual void print(const char* format, ...) {}
	virtual void print(const wchar_t* format, ...) {}
	virtual void print(const char32_t* format, ...) {}

	virtual void vprint(const char* format, va_list arg) {}
	virtual void vprint(const wchar_t* format, va_list arg) {}
	virtual void vprint(const char32_t* format, va_list arg) {}

	// Default alOStream will print text into stdio.
	// You can set stderr or stdio using m_stdout
	// alLog uses this.
	static alOStream& get_default_ostream();

	FILE* m_stdout = stdout;

protected:
	pos_type m_position = 0;
};

#endif
