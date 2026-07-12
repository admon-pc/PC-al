#ifndef _AL_SYSTEMWINDOWWIN32_H_
#define _AL_SYSTEMWINDOWWIN32_H_

#ifdef AL_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#ifdef DrawText
#undef DrawText
#endif
struct alSystemWindowOSDataWin32
{
	DWORD m_oldStyle = 0;
	HWND m_hwnd;
	HDC m_dc;
	WINDOWPLACEMENT m_wndPlcmnt;
	wchar_t m_className[100];
};
#endif

#endif
