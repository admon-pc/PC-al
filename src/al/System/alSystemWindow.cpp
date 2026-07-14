#include "al.h"
#include "Input/alInput.h"
#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

#ifdef AL_PLATFORM_WIN32
LRESULT CALLBACK alSystemWindow_WndProc(HWND, UINT, WPARAM, LPARAM);
#endif

// ==================================================
//                                          alLibImpl
// ==================================================
alSystemWindow* alLibImpl::CreateSystemWindow(alSystemWindowCallback* cb)
{
	alSystemWindow* sw = alCreate<alSystemWindow>(cb);
	if (sw)
	{
#ifdef AL_PLATFORM_WIN32
		wchar_t className[100];
		static int number = 0;
		swprintf_s(className, 100, L"alWindow_%i", ++number);

		WNDCLASSEXW wcex;
		memset(&wcex, 0, sizeof(wcex));
		wcex.cbSize = sizeof(WNDCLASSEX);
	    wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = alSystemWindow_WndProc;
		wcex.hInstance = g_alLibGlobalData.m_moduleHandle;
		//wcex.hIcon = LoadIcon(g_alLibGlobalData.m_moduleHandle, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	 //   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	   // wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
		wcex.lpszClassName = className;
		//wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		if (!RegisterClassExW(&wcex))
		{
			AL_DESTROY(sw);
			sw = 0;
		}
		else
		{
			HWND hWnd = CreateWindowW(
				className,
				L"_",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT,
				0,
				CW_USEDEFAULT,
				0,
				nullptr,
				nullptr,
				g_alLibGlobalData.m_moduleHandle,
				nullptr);

			if (!hWnd)
			{
				AL_DESTROY(sw);
				sw = 0;
			}
			else
			{
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)sw);

				alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)sw->GetOSData();
				data_win32->m_hwnd = hWnd;
				data_win32->m_dc = GetDC(hWnd);
				swprintf_s(data_win32->m_className, 100, L"%s", className);

			}
		}
#endif

	}
	return sw;
}

// ==================================================
//                                     alSystemWindow
// ==================================================
alSystemWindow::alSystemWindow(alSystemWindowCallback* cb)
{
	AL_ASSERT_ST(cb);
	m_callback = cb;

#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = new alSystemWindowOSDataWin32;
	m_OSData = data_win32;
#endif
}

alSystemWindow::~alSystemWindow()
{
	Hide();

	if (m_OSData)
	{
#ifdef AL_PLATFORM_WIN32
		alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
		UnregisterClass(data_win32->m_className, g_alLibGlobalData.m_moduleHandle);
		DestroyWindow(data_win32->m_hwnd);
		ReleaseDC(data_win32->m_hwnd, data_win32->m_dc);
		delete data_win32;
#endif
	}
}

void alSystemWindow::Show()
{
#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
	ShowWindow(data_win32->m_hwnd, SW_SHOW);
	UpdateWindow(data_win32->m_hwnd);
#endif
}

void alSystemWindow::Hide()
{
#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
	ShowWindow(data_win32->m_hwnd, SW_HIDE);
#endif
}

void alSystemWindow::SetTitle(const char* title)
{
#ifdef AL_PLATFORM_WIN32
	alUnicodeString ustr(title);
	alStringW strw;
	ustr.ToUTF16(strw);

	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
	SetWindowText(data_win32->m_hwnd, strw.data());
#endif
}

void alSystemWindow::SetPositionAndSize(uint32_t posX, uint32_t posY, uint32_t sizeX, uint32_t sizeY)
{
#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
	
	RECT r;
	GetWindowRect(data_win32->m_hwnd, &r);
//	printf("Window size before %u %u\n", r.right - r.left, r.bottom - r.top);
	GetClientRect(data_win32->m_hwnd, &r);
//	printf("Window client rect size before %u %u\n", r.right - r.left, r.bottom - r.top);

	MoveWindow(data_win32->m_hwnd, posX, posY, posX + sizeX, posY + sizeY, TRUE);

	GetWindowRect(data_win32->m_hwnd, &r);
//	printf("Window size after %u %u\n", r.right - r.left, r.bottom - r.top);
	GetClientRect(data_win32->m_hwnd, &r);
//	printf("Window client rect size after %u %u\n", r.right - r.left, r.bottom - r.top);

	int clientSizeX = r.right - r.left;
	int clientSizeY = r.bottom - r.top;
	//MoveWindow(data_win32->m_hwnd, posX, posY, posX + sizeX + (sizeX - clientSizeX), posY + sizeY + (sizeY - clientSizeY), TRUE);
	MoveWindow(data_win32->m_hwnd, posX, posY, sizeX + (sizeX-clientSizeX), sizeY + (sizeY-clientSizeY), TRUE);
	GetClientRect(data_win32->m_hwnd, &r);
//	printf("Window client rect size after %u %u\n", r.right - r.left, r.bottom - r.top);
#endif
}

void alSystemWindow::ToFullscreenMode()
{
	if (m_isFullscreen)
		return;

#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;

	DWORD dwStyle = GetWindowLong(data_win32->m_hwnd, GWL_STYLE);
	data_win32->m_oldStyle = dwStyle;
	MONITORINFO mi = { sizeof(mi) };
	if (GetWindowPlacement(data_win32->m_hwnd, &data_win32->m_wndPlcmnt) &&
		GetMonitorInfo(MonitorFromWindow(data_win32->m_hwnd, MONITOR_DEFAULTTOPRIMARY), &mi))
	{
		//m_currentSize.x = (float32_t)mi.rcMonitor.right - (float32_t)mi.rcMonitor.left;
		//m_currentSize.y = (float32_t)mi.rcMonitor.bottom - (float32_t)mi.rcMonitor.top;
		//_set_current_rect();

		SetWindowLong(data_win32->m_hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(data_win32->m_hwnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		m_isFullscreen = true;
#endif
	}
}

void alSystemWindow::ToWindowMode()
{
	if (!m_isFullscreen)
		return;
#ifdef AL_PLATFORM_WIN32
	alSystemWindowOSDataWin32* data_win32 = (alSystemWindowOSDataWin32*)m_OSData;
	SetWindowLong(data_win32->m_hwnd, GWL_STYLE, data_win32->m_oldStyle);
	//m_currentSize = m_creationSize;
	//_set_current_rect();
	SetWindowPlacement(data_win32->m_hwnd, &data_win32->m_wndPlcmnt);
	SetWindowPos(data_win32->m_hwnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
#endif
	m_isFullscreen = false;
}


#ifdef AL_PLATFORM_WIN32
LRESULT CALLBACK alSystemWindow_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId = LOWORD(wParam);
	alSystemWindow* wnd = (alSystemWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	//printf("%u ", message);

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		auto input = alLib::GetInput();
		input->m_isLMBDown_window = true;
		return 0;
	}
	case WM_LBUTTONUP:
	{
		auto input = alLib::GetInput();
		input->m_isLMBDown_window = false;
		return 0;
	}
	case WM_GETMINMAXINFO:
	{
		if (wnd)
		{
			auto mmi = wnd->GetWindowCallback()->OnMinMaxInfo(wnd);
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = mmi.x;
			lpMMI->ptMinTrackSize.y = mmi.y;
		}
		else
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 800;
			lpMMI->ptMinTrackSize.y = 600;
		}
	}break;
	case WM_DROPFILES:
	{
		if (wnd)
		{
			//	printf("DROP FILES\n");
			HDROP hDrop = (HDROP)wParam;
			if (hDrop)
			{
				UINT num = DragQueryFileW(hDrop, 0xFFFFFFFF, 0, 0);
				//printf("%u FILES\n", num);
				for (UINT i = 0; i < num; ++i)
				{
					wchar_t buf[0x500];
					DragQueryFileW(hDrop, i, buf, 0x500);
					buf[0x500 - 1] = 0;

					POINT pt;
					DragQueryPoint(hDrop, &pt);

					wnd->GetWindowCallback()->OnDropFiles(
						wnd,
						num,
						i, 
						buf, 
						pt.x, 
						pt.y);
					//	wprintf(L"FILE: %s\n", buf);
				}

				DragFinish(hDrop);
			}
		}

		return 0;
	}break;
	case WM_DESTROY:
	{
		return 0;
	}
	case WM_INPUT:
		if (wnd)
		{
			/*if (pD->m_onRawInput)
				pD->m_onRawInput(pD,
					GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT,
					(HRAWINPUT)lParam);*/


			static alArray<char> m_RawInputMessageData;

			HRAWINPUT hRawInput = (HRAWINPUT)lParam;
			UINT dataSize;
			GetRawInputData(
				hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize == 0)
				break;
			if (dataSize > m_RawInputMessageData.size())
				m_RawInputMessageData.reserve(dataSize);

			void* dataBuf = &m_RawInputMessageData[0];
			GetRawInputData(
				hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

			const RAWINPUT* raw = (const RAWINPUT*)dataBuf;
			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				auto input = alLib::GetInput();
				HANDLE deviceHandle = raw->header.hDevice;

				const RAWMOUSE& mouseData = raw->data.mouse;

				USHORT flags = mouseData.usButtonFlags;
				short wheelDelta = (short)mouseData.usButtonData;
				LONG x = mouseData.lLastX, y = mouseData.lLastY;

				/*wprintf(
					L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
					deviceHandle, flags, wheelDelta, x, y);*/

				input->m_mouseDelta.x = (float32_t)x;
				input->m_mouseDelta.y = (float32_t)y;
				if (wheelDelta)
					input->m_wheelDelta = (float32_t)wheelDelta / (float32_t)WHEEL_DELTA;

				POINT cursorPoint;
				GetCursorPos(&cursorPoint);
				ScreenToClient(hWnd, &cursorPoint);
				input->m_cursorCoords.x = cursorPoint.x;
				input->m_cursorCoords.y = cursorPoint.y;
				input->m_cursorCoordsForGUI.x = (float32_t)input->m_cursorCoords.x;
				input->m_cursorCoordsForGUI.y = (float32_t)input->m_cursorCoords.y;

				if (flags)
				{
					input->m_isLMBDown = (flags & 0x1) == 0x1;
					input->m_isLMBUp = (flags & 0x2) == 0x2;
					if (input->m_isLMBDown) input->m_isLMBHold = true;
					if (input->m_isLMBUp)    input->m_isLMBHold = false;

					input->m_isRMBDown = (flags & 0x4) == 0x4;
					input->m_isRMBUp = (flags & 0x8) == 0x8;
					if (input->m_isRMBDown)  input->m_isRMBHold = true;
					if (input->m_isRMBUp)    input->m_isRMBHold = false;

					input->m_isMMBDown = (flags & 0x10) == 0x10;
					input->m_isMMBUp = (flags & 0x20) == 0x20;
					if (input->m_isMMBDown)  input->m_isMMBHold = true;
					if (input->m_isMMBUp)    input->m_isMMBHold = false;

					input->m_isX1MBDown = (flags & 0x100) == 0x100;
					input->m_isX1MBUp = (flags & 0x200) == 0x200;
					if (input->m_isX1MBDown)  input->m_isX1MBHold = true;
					if (input->m_isX1MBUp)    input->m_isX1MBHold = false;

					input->m_isX2MBDown = (flags & 0x40) == 0x40;
					input->m_isX2MBUp = (flags & 0x80) == 0x80;
					if (input->m_isX2MBDown)  input->m_isX2MBHold = true;
					if (input->m_isX2MBUp)    input->m_isX2MBHold = false;
				}
			}
		}
		break;
	case WM_MOVE:
	{
		if (wnd)
			wnd->GetWindowCallback()->OnMove(wnd);
	}
	break;
	case WM_CAPTURECHANGED:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCMOUSEHOVER:
	case WM_NCMOUSELEAVE:
	{
		break;
	}
	case WM_ACTIVATE:
	{
		switch (wmId)
		{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if (wnd)
			{
				auto input = alLib::GetInput();
				switch (input->m_kbm) {
				case alKeyboardModifier::None:
				case alKeyboardModifier::Ctrl:
				case alKeyboardModifier::Shift:
				case alKeyboardModifier::ShiftCtrl:
				default:
					break;
				case alKeyboardModifier::Alt:
				case alKeyboardModifier::ShiftAlt:
				case alKeyboardModifier::ShiftCtrlAlt:
				case alKeyboardModifier::CtrlAlt:
				{
					input->m_kbm = alKeyboardModifier::None;
				}
				break;
				}

				input->ResetHold();
				//input->m_key_hold[(uint32_t)alInputKey::K_ALT] = 0;
				//input->m_key_hold[(uint32_t)alInputKey::K_LALT] = 0;
				//input->m_key_hold[(uint32_t)alInputKey::K_RALT] = 0;

				wnd->GetWindowCallback()->OnActivate(wnd);
			}
			break;
		case WA_INACTIVE:
			if (wnd)
				wnd->GetWindowCallback()->OnDeactivate(wnd);
			break;
		}
		break;
	}
	case WM_PAINT:
	{
		if (wnd)
			wnd->GetWindowCallback()->OnPaint(wnd);
		break;
	}
	case WM_SETCURSOR:
	{
		if (!g_alLib->m_showCursor)
		{
			SetCursor(NULL); // Hides cursor in client area
			return TRUE;    // Prevents default cursor handling
		}

		if (alLib::GetCursorDisableAutoChange())
			return TRUE;
		auto id = LOWORD(lParam);
		switch (id)
		{
		default:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::Arrow)->GetHandle());
			return TRUE;
		case HTLEFT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeWE)->GetHandle());
			return TRUE;
		case HTRIGHT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeWE)->GetHandle());
			return TRUE;
		case HTTOP:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNS)->GetHandle());
			return TRUE;
		case HTBOTTOM:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNS)->GetHandle());
			return TRUE;
		case HTTOPLEFT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNWSE)->GetHandle());
			return TRUE;
		case HTBOTTOMRIGHT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNWSE)->GetHandle());
			return TRUE;
		case HTBOTTOMLEFT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNESW)->GetHandle());
			return TRUE;
		case HTTOPRIGHT:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::SizeNESW)->GetHandle());
			return TRUE;
		case HTHELP:
			SetCursor((HCURSOR)alLib::GetCursor(alCursorType::Help)->GetHandle());
			return TRUE;
		}
	}break;
	case WM_SIZE:
	{
		if (wnd)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			wnd->m_clientSize.x = rc.right - rc.left;
			wnd->m_clientSize.y = rc.bottom - rc.top;

			if (wnd->m_clientSize.x <= 0)
				wnd->m_clientSize.x = 1;
			if (wnd->m_clientSize.y <= 0)
				wnd->m_clientSize.y = 1;

			if (wnd->m_clientSize.x != wnd->m_clientSizeOld.x || wnd->m_clientSize.y != wnd->m_clientSizeOld.y)
			{
				//	ClientResize(hWnd, (s32)pD->m_currentSize.x, (s32)pD->m_currentSize.y);

				wnd->m_clientSizeOld = wnd->m_clientSize;
				//if (pD->m_onSize)
					//pD->m_onSize(pD);

			//	wnd->m_clientSize = wnd->m_clientSize;

				//?????
				//maybe after next switch?
				if (wmId != SIZE_MINIMIZED)
				{
					alEvent e;
					e.m_type = alEventType::Window;
					e.m_event_window.m_event = alEvent_Window::size_changed;
					alLib::AddEvent(e, true);
					wnd->GetWindowCallback()->OnSizeChanged(wnd);
				}
			}
		}
		switch (wmId)
		{
		case SIZE_MAXIMIZED:
			if (wnd)
				wnd->GetWindowCallback()->OnMaximize(wnd);
			break;
		case SIZE_MINIMIZED:
			if (wnd)
				wnd->GetWindowCallback()->OnMinimize(wnd);
			break;
		case SIZE_RESTORED:
			if (wnd)
				wnd->GetWindowCallback()->OnRestore(wnd);
			break;
		}
		
		return 0;
	}break;

	case WM_SIZING:
	{
		if (wnd)
		{
			wnd->GetWindowCallback()->OnSize(wnd);
			//RECT rc;
			//GetClientRect(hWnd, &rc);
			//pD->m_currentSize.x = (f32)rc.right - (f32)rc.left;
			//pD->m_currentSize.y = (f32)rc.bottom - (f32)rc.top;
			//pD->_set_current_rect();
			//if (pD->m_currentSize.x != pD->m_oldSize.x || pD->m_currentSize.y != pD->m_oldSize.y)
			//{
			//	//	ClientResize(hWnd, (s32)pD->m_currentSize.x, (s32)pD->m_currentSize.y);
			//	if (pD->m_onSize)
			//		pD->m_onSize(pD);
			//	pD->m_oldSize = pD->m_currentSize;
			//	miEvent e;
			//	e.m_type = miEventType::Window;
			//	e.m_event_window.m_event = miEvent_Window::size_changed;
			//	miAddEvent(e, true);
			//}
		}
	}
	break;
	case WM_CHAR:
	{
		switch (wParam)
		{
		case 0x08: // or '\b'
		// Process a backspace.
			break;

		//case 0x09: // or '\t'
		//	// Process a tab.
		//	break;

		case 0x0A: // or '\n'
			// Process a linefeed.
			break;

		case 0x0D:
			// Process a carriage return.
			break;

		case 0x1B:
			// Process an escape.
			break;
		default:
		{
			if (wnd)
			{
				auto input = alLib::GetInput();
				input->m_character = (TCHAR)wParam;
				/*wchar_t buf[10];
				swprintf_s(buf, 10, L"%c", ch);
				SetWindowTextW(hWnd, buf);*/
			}
		}
			break;
		}
		break;
	}
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_KEYMENU)
			return 0;
		break;
	}
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		bool isPress = true;

		alInputKey key = (alInputKey)wParam;

		if (message == WM_SYSKEYUP) isPress = false;
		if (message == WM_KEYUP) isPress = false;

		const UINT MY_MAPVK_VSC_TO_VK_EX = 3;

		if (key == alInputKey::K_SHIFT)
		{ // shift -> lshift rshift
			key = static_cast<alInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255u), MY_MAPVK_VSC_TO_VK_EX));
		}
		if (key == alInputKey::K_CTRL)
		{ // ctrl -> lctrl rctrl
			key = static_cast<alInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
			if (lParam & 0x1000000)
				key = static_cast<alInputKey>(163);
		}

		if (key == alInputKey::K_ALT)
		{ // alt -> lalt ralt
			key = static_cast<alInputKey>(MapVirtualKey((static_cast<UINT>(lParam >> 16) & 255), MY_MAPVK_VSC_TO_VK_EX));
			if (lParam & 0x1000000)
				key = static_cast<alInputKey>(165);
			//printf("alt = %i\n",(int)ev.keyboardEvent.key);
		}

		uint8_t keys[256u];
		GetKeyboardState(keys);
		//WORD chars[2];


		auto input = alLib::GetInput();
		if (isPress)
		{
			if ((uint32_t)key < 256)
			{
				input->m_key_hold[(uint32_t)key] = 1;
				input->m_key_hit[(uint32_t)key] = 1;
			}
		}
		else
		{
			if ((uint32_t)key < 256)
			{
				input->m_key_hold[(uint32_t)key] = 0;
				input->m_key_released[(uint32_t)key] = 1;
			}
		}

		if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		else
		{
			return 0;
		}
	}break;
	case WM_NCMOUSEMOVE:
	{
		return 0;
	}
	case WM_COMMAND:
	{
		if (wnd)
			wnd->GetWindowCallback()->OnPopupCommand(LOWORD(wParam));
	}return 0;
	case WM_CLOSE:
	{
		if (wnd)
			wnd->GetWindowCallback()->OnClose(wnd);
		else
			PostQuitMessage(0);
		return 0;
	}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif

