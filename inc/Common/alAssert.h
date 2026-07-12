#ifndef _AL_ASSERT_H_
#define _AL_ASSERT_H_

class alAssert
{
public:
	static void OnAssert(const char* message, const char * file, uint32_t line);
	static void OnAssertST(const char* message, const char* file, uint32_t line);
};

#ifdef AL_DEBUG
#define AL_ASSERT(x)if(!!(x)==false){alAssert::OnAssert(#x, AL_FILE, (uint32_t)AL_LINE); AL_DEBUGBREAK;}
#define AL_ASSERT_ST(x)if(!!(x)==false){alAssert::OnAssertST(#x, AL_FILE, (uint32_t)AL_LINE); AL_DEBUGBREAK;}
#else
#define AL_ASSERT(x)((void)0)
#define AL_ASSERT_ST(x)((void)0)
#endif


#endif
