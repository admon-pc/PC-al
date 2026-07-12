#include "al.h"
#include "System/alSystemPopup.h"
#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"
#include "alSystemPopupWin32.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;

alSystemPopupWin32::alSystemPopupWin32()
{
	m_hPopupMenu = CreatePopupMenu();
}

alSystemPopupWin32::~alSystemPopupWin32()
{
	for (size_t i = 0, sz = m_subMenus.size(); i < sz; ++i)
	{
		alDestroy(m_subMenus[i]);
	}
	DestroyMenu(m_hPopupMenu);
}

alSystemPopup* alSystemPopupWin32::CreateSubMenu(const char32_t* text)
{
	AL_ASSERT_ST(text);
	alUnicodeString ustr;
	ustr.Assign(text);
	alStringW strw;
	ustr.ToUTF16(strw);

	auto newSubMenu = alCreate<alSystemPopupWin32>();
	m_subMenus.push_back(newSubMenu);
	AppendMenu(m_hPopupMenu, MF_POPUP | MF_BYPOSITION | MF_STRING, (UINT_PTR)newSubMenu->m_hPopupMenu, 
		strw.c_str());
	return newSubMenu;
}

void alSystemPopupWin32::AddItem(const char32_t* text, uint32_t id, const char32_t* shortcut)
{
	AL_ASSERT_ST(text);
	alUnicodeString ustr;
	ustr.Assign(text);
	alStringW strw;
	ustr.ToUTF16(strw);
	if (shortcut)
	{
		strw += L"\t";
		strw += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, strw.data());
}


void alSystemPopupWin32::AddSeparator()
{
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void alSystemPopupWin32::Show(alSystemWindow* w, uint32_t x, uint32_t y)
{
	alSystemWindowOSDataWin32* windowData = (alSystemWindowOSDataWin32*)w->GetOSData();

	HWND hWnd = (HWND)windowData->m_hwnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
}

