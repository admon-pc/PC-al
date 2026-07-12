#ifndef _AL_CURSORWIN32_H_
#define _AL_CURSORWIN32_H_

#ifdef AL_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

class alCursorWin32 : public alCursor
{
public:
	alCursorWin32();
	virtual ~alCursorWin32();

	virtual void OnCreate(alCursorType ct) override;
	virtual void Activate() override;
	virtual void* GetHandle() override;

	HCURSOR m_handle = 0;
};
#endif

#endif
