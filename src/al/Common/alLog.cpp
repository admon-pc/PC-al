#include "al.h"

#include <stdarg.h>

class alLogImpl
{
public:
	alLogImpl()
	{
	}

	~alLogImpl()
	{
	}
	alOStream* m_ostream = 0;
}
g_alLogImpl;

void alLog::SetOStream(alOStream* s)
{
	g_alLogImpl.m_ostream = s;
}

void alLog::Print(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	g_alLogImpl.m_ostream->vprint(s, ap);
	va_end(ap);

}

void alLog::PrintInfo(const char* s, ...)
{
	g_alLogImpl.m_ostream->print("Info: ");
	va_list ap;
	va_start(ap, s);
	g_alLogImpl.m_ostream->vprint(s, ap);
	va_end(ap);
}

void alLog::PrintWarning(const char* s, ...)
{
	g_alLogImpl.m_ostream->print("Warning: ");
	va_list ap;
	va_start(ap, s);
	g_alLogImpl.m_ostream->vprint(s, ap);
	va_end(ap);
}

void alLog::PrintError(const char* s, ...)
{
	g_alLogImpl.m_ostream->m_stdout = stderr;
	g_alLogImpl.m_ostream->print("Error: ");
	va_list ap;
	va_start(ap, s);
	g_alLogImpl.m_ostream->vprint(s, ap);
	va_end(ap);
	g_alLogImpl.m_ostream->m_stdout = stdout;
}

