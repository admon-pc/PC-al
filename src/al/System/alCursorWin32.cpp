#include "al.h"
#include "System/alCursor.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

alCursorWin32::alCursorWin32()
{
}

alCursorWin32::~alCursorWin32()
{
}

void alCursorWin32::OnCreate(alCursorType ct)
{
	m_type = ct;
}

void alCursorWin32::Activate()
{
	if (g_alLib->m_showCursor)
		::SetCursor(m_handle);
}

void* alCursorWin32::GetHandle()
{
	return m_handle;
}


