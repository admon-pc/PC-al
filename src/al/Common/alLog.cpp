#include "al.h"

#include <stdarg.h>
void alLogDefaultPrintfFunction(const char* str);
class alLogImpl
{
public:
	alLogImpl()
	{
		m_buffer = new char[0xffff];
	}

	~alLogImpl()
	{
		delete[]m_buffer;
	}

	void Print()
	{
		m_printFunction(m_buffer);
	}

	void(*m_printFunction)(const char*) = alLogDefaultPrintfFunction;

	char* m_buffer = 0;
	size_t m_bufferSize = 0xffff;

	FILE* m_outType = stdout;
}
g_alLogImpl;
void alLogDefaultPrintfFunction(const char* str)
{
	fprintf(g_alLogImpl.m_outType, "%s", str);
}

void alLog::Print(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	vsnprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, s, ap);
	g_alLogImpl.m_outType = stdout;
	g_alLogImpl.Print();
	va_end(ap);

}

void alLog::PrintInfo(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	snprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, "%s", "Info: ");
	g_alLogImpl.Print();
	vsnprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, s, ap);
	g_alLogImpl.m_outType = stdout;
	g_alLogImpl.Print();
	va_end(ap);
}

void alLog::PrintWarning(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	snprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, "%s", "Warning: ");
	g_alLogImpl.Print();
	vsnprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, s, ap);
	g_alLogImpl.m_outType = stdout;
	g_alLogImpl.Print();
	va_end(ap);
}

void alLog::PrintError(const char* s, ...)
{
	va_list ap;
	va_start(ap, s);
	snprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, "%s", "Error: ");
	g_alLogImpl.Print();
	vsnprintf(g_alLogImpl.m_buffer, g_alLogImpl.m_bufferSize, s, ap);
	g_alLogImpl.m_outType = stderr;
	g_alLogImpl.Print();
	va_end(ap);
}

