#include "al.h"

#include <assert.h>

void alAssert::OnAssert(const char* message, const char* file, uint32_t line)
{
	printf("Assertion failed: %s, file %s, line %u\n", message, file, line);
}

void alAssert::OnAssertST(const char* message, const char* file, uint32_t line)
{
	printf("Assertion failed: %s, file %s, line %u\n", message, file, line);
}
