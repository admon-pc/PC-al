#ifndef _AL_LOG_H_
#define _AL_LOG_H_

class alLog
{
public:
	static void Print(const char*, ...);
	static void PrintInfo(const char*, ...);
	static void PrintWarning(const char*, ...);
	static void PrintError(const char*, ...);

	static void SetPrintFunction(void(*)(const char*));
};

#endif
