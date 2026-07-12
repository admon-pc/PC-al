#include "al.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

#ifdef AL_PLATFORM_WIN32
#include <Windows.h>
#endif

void* alMemory::Malloc(size_t sz)
{
	AL_ASSERT_ST(sz);
	void* data = 0;

#ifdef AL_PLATFORM_WIN32
	data = HeapAlloc(g_alLibGlobalData.m_processHeap, 0, sz);
#else
	data = malloc(sz);
#endif

	return data;
}

void* alMemory::Calloc(size_t sz)
{
	AL_ASSERT_ST(sz);
	void* data = 0;

#ifdef AL_PLATFORM_WIN32
	data = HeapAlloc(g_alLibGlobalData.m_processHeap, HEAP_ZERO_MEMORY, sz);
#else
	data = calloc(1, sz);
#endif

	return data;
}

void* alMemory::Realloc(void* p, size_t sz)
{
	AL_ASSERT_ST(p);
	AL_ASSERT_ST(sz);

	void* data = 0;

#ifdef AL_PLATFORM_WIN32
	data = HeapReAlloc(g_alLibGlobalData.m_processHeap, HEAP_ZERO_MEMORY, p, sz);
#else
	data = realloc(p, sz);
#endif

	return data;
}

void alMemory::Free(void* p)
{
	AL_ASSERT_ST(p);

#ifdef AL_PLATFORM_WIN32
	HeapFree(g_alLibGlobalData.m_processHeap, 0, p);
#else
	free(p);
#endif
}

