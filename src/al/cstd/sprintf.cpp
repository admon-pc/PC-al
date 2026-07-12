#include "al.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

namespace al_internal
{
	enum class vsnprintf_specifier
	{
		d_i, // %d %i
		u, // %d %i
		o,
		X,
		F,
		E,
		G,
		A,
		c,
		s,
		p,
		n,
		percent,    // %%

		_empty
	};
	enum class vsnprintf_flag
	{
		minus,
		plus,
		space,
		number_sign,
		zero,

		_empty
	};
	enum class vsnprintf_length
	{
		hh, h, l, ll, j, z, t, L,
		_empty
	};

	struct vsnprintf_data
	{
		vsnprintf_flag m_flag = vsnprintf_flag::_empty;
		uint32_t m_width = 0;
		uint32_t m_precision = 0;
		vsnprintf_length m_length = vsnprintf_length::_empty;
		vsnprintf_specifier m_specifier = vsnprintf_specifier::_empty;
		bool m_uppercase = false;
	};

	const char32_t* alLib_vsnprintf_decodeFormat(
		const char32_t* format, 
		vsnprintf_data& out)
	{
		const char32_t* pfmt = format;
		switch (*pfmt)
		{
		case U'%':
			out.m_specifier = al_internal::vsnprintf_specifier::percent;
			++pfmt;
			return pfmt;
		}

		while (*pfmt)
		{
			switch (*pfmt)
			{
			case U'-':
				out.m_flag = vsnprintf_flag::minus;
				break;
			case U'+':
				out.m_flag = vsnprintf_flag::plus;
				break;
			case U' ':
				out.m_flag = vsnprintf_flag::space;
				break;
			case U'#':
				out.m_flag = vsnprintf_flag::number_sign;
				break;
			case U'0':
				out.m_flag = vsnprintf_flag::zero;
				break;
			default:
				goto endFlags;
			}
			++pfmt;
		}
	endFlags:

		//influenced by EAStdC
		while (((uint32_t)(*pfmt - U'0')) < 10)
		{
			out.m_width = (uint32_t)((out.m_width * 10) + (*pfmt - '0'));
			++pfmt;
		}

		if (*pfmt == U'.')
		{
			++pfmt;
			while (((uint32_t)(*pfmt - U'0')) < 10)
			{
				out.m_precision = (uint32_t)((out.m_precision * 10) + (*pfmt - '0'));
				++pfmt;
			}
		}

		switch (*pfmt)
		{
		case U'h':
			out.m_length = al_internal::vsnprintf_length::h;
			if (*(++pfmt) == U'h')
			{	
				out.m_length = al_internal::vsnprintf_length::hh;
				++pfmt;
			}
			else
				--pfmt;

			++pfmt;
			break;
		case U'l':
			out.m_length = al_internal::vsnprintf_length::l;
			if (*(++pfmt) == U'l')
			{
				out.m_length = al_internal::vsnprintf_length::ll;
				++pfmt;
			}
			else
				--pfmt;

			++pfmt;
			break;
		case U'j':
			out.m_length = al_internal::vsnprintf_length::j;
			++pfmt;
			break;
		case U'z':
			out.m_length = al_internal::vsnprintf_length::z;
			++pfmt;
			break;
		case U't':
			out.m_length = al_internal::vsnprintf_length::t;
			++pfmt;
			break;
		case U'L':
			out.m_length = al_internal::vsnprintf_length::L;
			++pfmt;
			break;
		}

		switch (*pfmt)
		{
		case U'd':
		case U'i':
			out.m_specifier = al_internal::vsnprintf_specifier::d_i;
			++pfmt;
			break;
		case U'u':
			out.m_specifier = al_internal::vsnprintf_specifier::u;
			++pfmt;
			break;
		case U'o':
			out.m_specifier = al_internal::vsnprintf_specifier::o;
			++pfmt;
			break;

		case U'X':
			out.m_uppercase = true;
		case U'x':
			out.m_specifier = al_internal::vsnprintf_specifier::X;
			++pfmt;
			break;

		case U'F':
			out.m_uppercase = true;
		case U'f':
			out.m_specifier = al_internal::vsnprintf_specifier::F;
			++pfmt;
			break;

		case U'E':
			out.m_uppercase = true;
		case U'e':
			out.m_specifier = al_internal::vsnprintf_specifier::E;
			++pfmt;
			break;

		case U'G':
			out.m_uppercase = true;
		case U'g':
			out.m_specifier = al_internal::vsnprintf_specifier::G;
			++pfmt;
			break;

		case U'A':
			out.m_uppercase = true;
		case U'a':
			out.m_specifier = al_internal::vsnprintf_specifier::A;
			++pfmt;
			break;

		case U'c':
			out.m_specifier = al_internal::vsnprintf_specifier::c;
			++pfmt;
			break;

		case U's':
			out.m_specifier = al_internal::vsnprintf_specifier::s;
			++pfmt;
			break;

		case U'p':
			out.m_specifier = al_internal::vsnprintf_specifier::p;
			++pfmt;
			break;

		case U'n':
			out.m_specifier = al_internal::vsnprintf_specifier::n;
			++pfmt;
			break;
		default:
			break;
		}

		return pfmt;
	}
}// end namespace al_internal

uint32_t alLib::sprintf(char32_t* str, const char32_t* format, ...)
{
	va_list args;
	va_start(args, format);

	uint32_t result = alLib::vsnprintf(str, INT_MAX, format, args);

	va_end(args);
	return result;
}

uint32_t alLib::vsnprintf(
	char32_t* buf,
	size_t bufSz,
	const char32_t* format,
	va_list arg)
{
	if (!bufSz)
		return 0;
	
	size_t writtenSz = 0;

	auto fmt_to_char = [](const char32_t* b, const char32_t* e, char* dst)
	{
		size_t numOfChars = uint32_t(e - b);
		if (numOfChars)
		{
			int c = 0;
			while (true)
			{
				*dst = (char)*b;
				++dst;
				*dst = 0;
				++b;

				if (e == b)
					break;
				++c;
				if (c == 50)
					break;
			}
		}
	};

	auto write_buf = [](
		char32_t* _buf, 
		size_t _bufSz, 
		size_t* _writtenSz,
		char* input)
	{
		while (*input)
		{
			if (*_writtenSz < _bufSz)
			{
				_buf[*_writtenSz] = *input;
				*_writtenSz += 1;
				_buf[*_writtenSz] = 0;
			}
			else
				break;

			++input;
		}
	};

	al_internal::vsnprintf_data formatData;
	auto fmt = format;
	while (*fmt)
	{
		if (*fmt == '%')
		{
			auto begin = fmt;
			fmt = al_internal::alLib_vsnprintf_decodeFormat(++fmt, formatData);
			auto end = fmt;

			switch (formatData.m_specifier)
			{
			case al_internal::vsnprintf_specifier::percent:
				buf[writtenSz] = U'%';
				++writtenSz;
				buf[writtenSz] = 0;
				break;
			case al_internal::vsnprintf_specifier::o:
			case al_internal::vsnprintf_specifier::X:
			case al_internal::vsnprintf_specifier::u:
			case al_internal::vsnprintf_specifier::d_i:
			{
				char fmtbuf[50];
				fmt_to_char(begin, end, fmtbuf);

				char buf8[50];
				switch (formatData.m_specifier)
				{
				case al_internal::vsnprintf_specifier::d_i:
					if(formatData.m_length == al_internal::vsnprintf_length::l)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, long int));
					else if (formatData.m_length == al_internal::vsnprintf_length::ll)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, long long int));
					else if (formatData.m_length == al_internal::vsnprintf_length::hh)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, signed char));
					else if (formatData.m_length == al_internal::vsnprintf_length::h)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, short int));
					else if (formatData.m_length == al_internal::vsnprintf_length::j)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, intmax_t));
					else if (formatData.m_length == al_internal::vsnprintf_length::z)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, size_t));
					else if (formatData.m_length == al_internal::vsnprintf_length::t)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, ptrdiff_t));
					else
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, int));
					break;
				case al_internal::vsnprintf_specifier::o:
				case al_internal::vsnprintf_specifier::X:
				case al_internal::vsnprintf_specifier::u:
					if (formatData.m_length == al_internal::vsnprintf_length::l)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, unsigned long int));
					else if (formatData.m_length == al_internal::vsnprintf_length::ll)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, unsigned long long int));
					else if (formatData.m_length == al_internal::vsnprintf_length::hh)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, unsigned char));
					else if (formatData.m_length == al_internal::vsnprintf_length::h)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, unsigned short int));
					else if (formatData.m_length == al_internal::vsnprintf_length::j)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, uintmax_t));
					else if (formatData.m_length == al_internal::vsnprintf_length::z)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, size_t));
					else if (formatData.m_length == al_internal::vsnprintf_length::t)
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, ptrdiff_t));
					else
						::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, unsigned int));
					break;
				}
				write_buf(buf, bufSz, &writtenSz, buf8);
			}break;
			case al_internal::vsnprintf_specifier::F:
			case al_internal::vsnprintf_specifier::E:
			case al_internal::vsnprintf_specifier::G:
			case al_internal::vsnprintf_specifier::A:
			{
				char fmtbuf[50];
				fmt_to_char(begin, end, fmtbuf);

				char buf8[50];
				
				if (formatData.m_length == al_internal::vsnprintf_length::L)
					::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, long double));
				else
					::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, double));
				write_buf(buf, bufSz, &writtenSz, buf8);
			}break;
			case al_internal::vsnprintf_specifier::c:
			{
				char fmtbuf[50];
				fmt_to_char(begin, end, fmtbuf);

				char buf8[50];

				::sprintf_s(buf8, 50, fmtbuf, va_arg(arg, char));
				write_buf(buf, bufSz, &writtenSz, buf8);
			}break;
			case al_internal::vsnprintf_specifier::s:
			{

				auto str = va_arg(arg, const char32_t*);
				while (*str)
				{
					if (writtenSz < bufSz)
					{
						buf[writtenSz] = *str;
						++writtenSz;
						++str;
						buf[writtenSz] = 0;
					}
					else
					{
						break;
					}
				}
			}break;
			}
		}
		else
		{
			if (writtenSz < bufSz)
			{
				buf[writtenSz] = *fmt;
				++writtenSz;
				buf[writtenSz] = 0;
				++fmt;
			}
			else
			{
				break;
			}
		}
	}


	return writtenSz;
}

