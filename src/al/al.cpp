#include "al.h"

#include <filesystem>

#include "Image/alImage.h"
#include "Image/alImageLoader.h"

#include "GS/alGS.h"

#include "Geometry/alMeshLoader.h"
#include "Geometry/alMeshLoaderOBJ.h"

#include "Classes/alColor.h"
#include "GUI/alGUI.h"

#include "Image/alImageLoader.BMP.h"
#include "Image/alImageLoader.PNG.h"
#include "Image/alImageLoader.TGA.h"
#ifdef AL_CONFIG_COMPILE_WITH_JPEG
#include "Image/alImageLoader.JPG.h"
#endif

#include "Archive/alArchive.h"
#include "System/alSystemWindowWin32.h"
#include "al_internal.h"
#include "al.defailtFont.mini.inl"

#include "System/alSystemPopup.h"
#include "System/alSystemPopupWin32.h"

alLibGlobalData g_alLibGlobalData;

AL_LINK_LIBRARY(zlib);
AL_LINK_LIBRARY(jpeglib);
AL_LINK_LIBRARY(libpng);
AL_LINK_LIBRARY(fastlz);
AL_LINK_LIBRARY(minizip);


// GS
alGS* alCreateGS_d3d11();
AL_LINK_LIBRARY(al.d3d11);

#ifdef LoadImage
#undef LoadImage
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

alLibImpl* g_alLib = 0;

namespace al_internal
{
	template<typename Type>
	void _stringGetWords(
		alArray<alString_base<Type>>* out_array,
		const alString_base<Type>& string,
		bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false)
	{
		alString_base<Type> word;
		size_t sz = string.size();
		for (size_t i = 0; i < sz; ++i) 
		{
			auto ch = string[i];
			if (ch < 256 && (uint8_t)ch == ' ')
			{
				if (word.size()) 
				{
					out_array->push_back(word);
					word.clear();
				}
				if (add_space)
				{
					out_array->push_back(alString_base<Type>((Type)ch));
				}
			}
			else if (ch < 256 && (uint8_t)ch == '\t') 
			{
				if (word.size())
				{
					out_array->push_back(word);
					word.clear();
				}
				if (add_tab) 
				{
					out_array->push_back(alString_base<Type>((Type)ch));
				}
			}
			else if (ch < 256 && (uint8_t)ch == '\n')
			{
				if (word.size()) 
				{
					out_array->push_back(word);
					word.clear();
				}
				if (add_newLine) 
				{
					out_array->push_back(alString_base<Type>((Type)ch));
				}
			}
			else
			{
				if (ispunct(ch))
				{
					if (ch == '_')
					{
						word += ch;
					}
					else
					{
						if (word.size())
						{
							out_array->push_back(word);
							word.clear();
						}
					}
				}
				else if (!ispunct(ch))
					word += ch;
			}
		}
		if (word.size()) 
		{
			out_array->push_back(word);
			word.clear();
		}
	}
}

// ==================================================
//                                    alLibGlobalData
// ==================================================
alLibGlobalData::alLibGlobalData()
{
#ifdef AL_PLATFORM_WIN32
	m_processHeap = GetProcessHeap();
	m_moduleHandle = GetModuleHandle(NULL);
#endif
}

alLibGlobalData::~alLibGlobalData()
{
	AL_DESTROY(g_alLib);
}

// ==================================================
//                                          alLibImpl
// ==================================================
alLibImpl::alLibImpl() {}
alLibImpl::~alLibImpl()
{
	AL_DESTROY(g_alLib->m_fontMini);

	for (uint32_t i = 0; i < g_alLib->m_meshLoaders.m_size; ++i)
	{
		delete g_alLib->m_meshLoaders.m_data[i];
	}

	for (uint32_t i = 0; i < g_alLib->m_imageLoaders.m_size; ++i)
	{
		delete g_alLib->m_imageLoaders.m_data[i];
	}

	for (uint32_t i = 0; i < (uint32_t)alCursorType::_count; ++i)
	{
		AL_DESTROY(m_cursorsDefault[i]);
	}
	AL_DESTROY(m_input);
}

// ==================================================
//                                              alLib
// ==================================================
void alLib::InitializeLib()
{
	if (!g_alLib)
	{
		g_alLib = alCreate<alLibImpl>();
		for (uint32_t i = 0; i < (uint32_t)alMatrixType::_count; ++i)
		{
			g_alLib->m_matrixPtrs[i] = 0;
		}

		g_alLib->m_imageLoader_PNG = new alImageLoaderPNG;
		g_alLib->m_imageLoaders.push_back(g_alLib->m_imageLoader_PNG);

		g_alLib->m_imageLoaders.push_back(new alImageLoaderBMP);
		g_alLib->m_imageLoaders.push_back(new alImageLoaderTGA);
#ifdef AL_CONFIG_COMPILE_WITH_JPEG
		g_alLib->m_imageLoaders.push_back(new alImageLoaderJPG);
#endif

		g_alLib->m_meshLoaders.push_back(new alMeshLoaderOBJ);


		g_alLib->m_input = alCreate<alInput>();

#ifdef AL_PLATFORM_WIN32
		RAWINPUTDEVICE device;
		device.usUsagePage = 0x01;
		device.usUsage = 0x02;
		device.dwFlags = 0;
		device.hwndTarget = 0;
		RegisterRawInputDevices(&device, 1, sizeof device);

		for (uint32_t i = 0; i < (uint32_t)alCursorType::_count; ++i)
		{
			g_alLib->m_cursorsDefault[i] = alCreate<alCursorWin32>();
			g_alLib->m_cursorsDefault[i]->OnCreate((alCursorType)i);

			switch ((alCursorType)i)
			{
			default:
			case alCursorType::Arrow: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_ARROW); break;
			case alCursorType::Cross: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_CROSS); break;
			case alCursorType::Hand: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_HAND); break;
			case alCursorType::Help: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_HELP); break;
			case alCursorType::IBeam: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_IBEAM); break;
			case alCursorType::No: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_NO); break;
			case alCursorType::Size: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_SIZEALL); break;
			case alCursorType::SizeNESW: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_SIZENESW); break;
			case alCursorType::SizeNS: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_SIZENS); break;
			case alCursorType::SizeNWSE: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_SIZENWSE); break;
			case alCursorType::SizeWE: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_SIZEWE); break;
			case alCursorType::UpArrow: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_UPARROW); break;
			case alCursorType::Wait: ((alCursorWin32*)g_alLib->m_cursorsDefault[i])->m_handle = LoadCursor(0, IDC_WAIT); break;
			}

			g_alLib->m_cursors[i] = g_alLib->m_cursorsDefault[i];
		}
#endif

	}
}

bool alLib::GetCursorDisableAutoChange()
{
	return g_alLib->m_cursorDisableAutoChange;
}

void alLib::SetCursorDisableAutoChange(bool v)
{
	g_alLib->m_cursorDisableAutoChange = v;
}

alSystemWindow* alLib::CreateSystemWindow(alSystemWindowCallback* cb)
{
	return g_alLib->CreateSystemWindow(cb);
}

alSystemPopup* alLib::CreateSystemPopup()
{
	return alCreate<alSystemPopupWin32>();
}

alInput* alLib::GetInput()
{
	return g_alLib->m_input;
}

uint64_t alLib::GetTime()
{
	static bool isInit = false;
	static uint64_t baseTime;
	if (!isInit)
	{
		auto now = std::chrono::high_resolution_clock::now();
		baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		isInit = true;
	}
	auto now = std::chrono::high_resolution_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	return ms - baseTime;
}

void alLib::Update()
{
	g_alLib->m_input->Reset();

	static uint64_t t1 = 0;
	uint64_t t2 = GetTime();
	float32_t tick = float32_t(t2 - t1);
	t1 = t2;
	g_alLib->m_dt = tick / 1000.f;

#ifdef AL_PLATFORM_WIN32

	// Вот что интересно.
	// В данном проекте вот такая форма работает как надо.
	// А вот что ниже, работает на Windows сообщениях,
	// всё замораживается, нужно крутить мышкой.
	// А в проекте с WinMain наоборот, данная форма морозит программу,
	// и нужна версия без while.
	// Может быть причина в том что здесь используется main а там WinMain.
	// Код ТОЙ проги взят отсюда.
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	/*MSG msg;
	GetMessage(&msg, NULL, 0, 0);
	TranslateMessage(&msg);
	DispatchMessage(&msg);*/
	
#endif

	int32_t ctrl_shift_alt = 0;
	if (g_alLib->m_input->IsKeyHold(alInputKey::K_LALT) || g_alLib->m_input->IsKeyHold(alInputKey::K_RALT))
		ctrl_shift_alt |= 1;

	if (g_alLib->m_input->IsKeyHold(alInputKey::K_LSHIFT) || g_alLib->m_input->IsKeyHold(alInputKey::K_RSHIFT))
		ctrl_shift_alt |= 2;

	if (g_alLib->m_input->IsKeyHold(alInputKey::K_LCTRL) || g_alLib->m_input->IsKeyHold(alInputKey::K_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  g_alLib->m_input->m_kbm = alKeyboardModifier::None;          break;
	case 1:  g_alLib->m_input->m_kbm = alKeyboardModifier::Alt;           break;
	case 2:  g_alLib->m_input->m_kbm = alKeyboardModifier::Shift;         break;
	case 3:  g_alLib->m_input->m_kbm = alKeyboardModifier::ShiftAlt;      break;
	case 4:  g_alLib->m_input->m_kbm = alKeyboardModifier::Ctrl;          break;
	case 5:  g_alLib->m_input->m_kbm = alKeyboardModifier::CtrlAlt;       break;
	case 6:  g_alLib->m_input->m_kbm = alKeyboardModifier::ShiftCtrl;     break;
	case 7:  g_alLib->m_input->m_kbm = alKeyboardModifier::ShiftCtrlAlt;  break;
	}
}

void alLib::AddEvent(const alEvent& e, bool uniq)
{
	if (g_alLib->m_events_num < AL_EVENT_MAX)
	{
		g_alLib->m_events[g_alLib->m_events_num] = e;
		++g_alLib->m_events_num;
	}
}

bool alLib::PollEvent(alEvent& e)
{
	if (g_alLib->m_events_num == 0)
	{
		g_alLib->m_events_current = 0;
		return false;
	}

	e = g_alLib->m_events[g_alLib->m_events_current];
	--g_alLib->m_events_num;
	++g_alLib->m_events_current;

	return true;
}

uint64_t alLib::GetFileSize(const char* f)
{
	if(std::filesystem::exists(f))
		return std::filesystem::file_size(f);
	return 0;
}

void alLib::RemoveFile(const char* f)
{
	if (std::filesystem::exists(f))
		std::filesystem::remove(f);
}

void alLib::GetFileName(const char* f, alStringA* out)
{
	std::filesystem::path p(f);
	auto name = p.stem();
	out->assign(name.c_str());
}

alMat4* alLib::GetMatrix(alMatrixType t)
{
	if (!g_alLib->m_matrixPtrs[(uint32_t)t])
		return &g_alLib->m_defaultMatrix[(uint32_t)t];
	return g_alLib->m_matrixPtrs[(uint32_t)t];
}

void alLib::SetMatrix(alMatrixType t, alMat4* m)
{
	g_alLib->m_matrixPtrs[(uint32_t)t] = m;
}

alGS* alLib::CreateGS(alVideoDriverType t)
{
	switch (t)
	{
	case alVideoDriverType::Unknown:
	default:
		break;
	case alVideoDriverType::Direct3D11:
		return alCreateGS_d3d11();
	}
	return 0;
}

uint32_t alLib::GetImageLoaderNum()
{
	return g_alLib->m_imageLoaders.size();
}

alImageLoader* alLib::GetImageLoader(uint32_t index)
{
	return g_alLib->m_imageLoaders[index];
}

alImageLoader* alLib::GetImageLoaderPNG()
{
	return g_alLib->m_imageLoader_PNG;
}

alImage* alLib::LoadALImage(const char* fn)
{
	AL_ASSERT_ST(fn);
	if (!std::filesystem::exists(fn))
	{
		alLog::PrintWarning("File %s not found\n", fn);
	}

	alStringA ext;
	StringGetExtension(fn, false, ext);
	ext.ToLower();

	if (ext.size())
	{
		for (size_t i = 0; i< g_alLib->m_imageLoaders.m_size; ++i)
		{
			const auto & loader = g_alLib->m_imageLoaders.m_data[i];
			auto extNum = loader->GetExtensionNum();
			for (size_t ei = 0; ei < extNum; ++ei)
			{
				if (strcmp(loader->GetExtension(ei), ext.data()) == 0)
				{
					alImage* img = loader->Load(fn);
					if (img)
						alLog::Print("Load Image: %s\n", fn);

					return img;
				}
			}
		}
	}
	return 0;
}

void alLib::StringGetWords(
	const char* str, 
	alArray<alStringA>* arr,
	bool add_space,
	bool add_tab,
	bool add_newLine)
{
	al_internal::_stringGetWords(arr, alStringA(str), add_space, add_tab, add_newLine);
}

void alLib::StringGetExtension(const char* str, bool addDot, alStringA& out)
{
	size_t sz = strlen(str);
	if (sz)
	{
		for (size_t i = sz-1; i >= 0; --i)
		{
			auto c = str[i];
			if (c == '/' || c == '.')
				break;
			else out += c;
			if (!i) break;
		}
		if (addDot && out.size())
		{
			out += ".";
		}
		out.Flip();
		//stringFlip<Type>(ret);
	}
}

float32_t* alLib::GetDeltaTime()
{
	return &g_alLib->m_dt;
}

alCursor* alLib::CreateCursor(alCursorType ct, const char* fn)
{
	alCursor* newCursor = 0;
#ifdef AL_PLATFORM_WIN32
	newCursor = alCreate<alCursorWin32>();
	alCursorWin32* ptr = (alCursorWin32*)newCursor;
	ptr->m_handle = (HCURSOR)::LoadImageA(GetModuleHandle(0), fn, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#else
#error only for Windows
#endif
	newCursor->OnCreate(ct);
	return newCursor;
}

alCursor* alLib::GetCursor(alCursorType ct)
{
	return g_alLib->m_cursors[(uint32_t)ct];
}

void alLib::ResetCursor(alCursorType ct)
{
	g_alLib->m_cursors[(uint32_t)ct] = g_alLib->m_cursorsDefault[(uint32_t)ct];
}

void alLib::ShowCursor(bool v)
{
	if (v)
	{
		GetCursor(alCursorType::Arrow)->Activate();
	}
	else
	{
#ifdef AL_PLATFORM_WIN32
		::SetCursor(0); // hide
#else
#error Need to implement
#endif
	}
}

void alLib::SetCursorClip(alVec4i* New, alVec4i* Old, alSystemWindow* w)
{
	alVec4i o = g_alLib->m_cursorClip;
	if (Old)
	{
		Old->x = g_alLib->m_cursorClip.x;
		Old->y = g_alLib->m_cursorClip.y;
		Old->z = g_alLib->m_cursorClip.z;
		Old->w = g_alLib->m_cursorClip.w;
	}

	if (New)
	{
		AL_ASSERT_ST(w);
		g_alLib->m_cursorClip.x = New->x;
		g_alLib->m_cursorClip.y = New->y;
		g_alLib->m_cursorClip.z = New->z;
		g_alLib->m_cursorClip.w = New->w;
#ifdef AL_PLATFORM_WIN32
		POINT p1;
		p1.x = (LONG)g_alLib->m_cursorClip.x;
		p1.y = (LONG)g_alLib->m_cursorClip.y;

		POINT p2;
		p2.x = (LONG)g_alLib->m_cursorClip.z;
		p2.y = (LONG)g_alLib->m_cursorClip.w;
		
		alSystemWindowOSDataWin32* data_win32 = new alSystemWindowOSDataWin32;

		ClientToScreen(data_win32->m_hwnd, &p1);
		ClientToScreen(data_win32->m_hwnd, &p2);

		g_alLib->m_cursorClip.x = p1.x;
		g_alLib->m_cursorClip.y = p1.y;
		g_alLib->m_cursorClip.z = p2.x;
		g_alLib->m_cursorClip.w = p2.y;
#else
#error Need to implement
#endif
	}

#ifdef AL_PLATFORM_WIN32
	if (!New)
	{
		RECT cclip;
		//	GetWindowRect(GetDesktopWindow(), &cclip);
		cclip.left = 0;
		cclip.top = 0;
		cclip.right = GetSystemMetrics(SM_CXMAXTRACK);
		cclip.bottom = GetSystemMetrics(SM_CYMAXTRACK);

		g_alLib->m_cursorClip.x = cclip.left;
		g_alLib->m_cursorClip.y = cclip.top;
		g_alLib->m_cursorClip.z = cclip.right;
		g_alLib->m_cursorClip.w = cclip.bottom;
	}

	RECT cclip;
	cclip.left = (LONG)g_alLib->m_cursorClip.x;
	cclip.top = (LONG)g_alLib->m_cursorClip.y;
	cclip.right = (LONG)g_alLib->m_cursorClip.z;
	cclip.bottom = (LONG)g_alLib->m_cursorClip.w;
	ClipCursor(&cclip);
#else
#error Need to implement
#endif
}

void alLib::SetCursor(alCursorType ct, alCursor* c)
{
	g_alLib->m_cursors[(uint32_t)ct] = c;
	c->Activate();
}

void alLib::CopyTextToClipboard(alUnicodeString* str)
{
	AL_ASSERT_ST(str);
	if (!str->Size())
		return;
#ifdef AL_PLATFORM_WIN32
	if (!OpenClipboard(0))
		return;

	auto len = str->Size();
	EmptyClipboard();
	HGLOBAL clipbuffer;
	clipbuffer = GlobalAlloc(GMEM_DDESHARE, (len + 1) * sizeof(WCHAR));

	wchar_t* buffer;
	buffer = (wchar_t*)GlobalLock(clipbuffer);

	alStringW wstr;
	str->ToUTF16(wstr);

	memcpy(buffer, wstr.data(), wstr.size() * sizeof(wchar_t));
	buffer[len] = 0;

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_UNICODETEXT, clipbuffer);
	CloseClipboard();
#else
#error Need implementation....
#endif
}

void alLib::GetTextFromClipboard(alUnicodeString* str)
{
	AL_ASSERT_ST(str);
#ifdef AL_PLATFORM_WIN32
	if (!OpenClipboard(0))
		return;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	char16_t* buffer = (char16_t*)GlobalLock(hData);
	GlobalUnlock(hData);
	CloseClipboard();

	str->Assign(buffer);
#else
#error Need implementation....
#endif
}

void alLib::SaveImage(const char* fn, alImage* image, alSaveImageType type)
{
	switch (type)
	{
	default:
	case alSaveImageType::png:
	{
		for (size_t i = 0; i < g_alLib->m_imageLoaders.m_size; ++i)
		{
			const auto& loader = g_alLib->m_imageLoaders.m_data[i];
			auto extNum = loader->GetExtensionNum();
			for (size_t ei = 0; ei < extNum; ++ei)
			{
				if (strcmp(loader->GetExtension(ei), "png") == 0)
				{
					loader->Save(image, fn);
					alLog::Print("Save Image: %s\n", fn);
					return;
				}
			}
		}
	}break;
	}
}

void alLib::LoadMesh(const char* fn, alMeshLoaderCallback* cb)
{
	for (size_t i = 0; i < g_alLib->m_meshLoaders.m_size; ++i)
	{
		const auto& loader = g_alLib->m_meshLoaders.m_data[i];
		auto extNum = loader->GetExtensionNum();
		for (size_t ei = 0; ei < extNum; ++ei)
		{
			if (strcmp(loader->GetExtension(ei), "obj") == 0)
			{
				loader->Load(fn, cb);
				return;
			}
		}
	}
}

void alLib::InitializeDefaultFont(alGS* gs)
{
	if (g_alLib->m_fontMini)
		return;

	alColor palette[2] = { ColorTransparent, ColorWhite };

	g_alLib->m_fontMini = alCreate<alGUIFont>();
	auto font = g_alLib->m_fontMini;

	alImage imgFont;
	imgFont.Create(128, 128);

	alVec4f uv;
	alVec2u where;
	imgFont.Fill(al_internal::default_font::glyph_A, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'A', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_B, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'B', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_C, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'C', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_D, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'D', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_E, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'E', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_F, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'F', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_G, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'G', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_H, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'H', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_I, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'I', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_J, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'J', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_K, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'K', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_L, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'L', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_M, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'M', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_N, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'N', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_O, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'O', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_P, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'P', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_Q, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'Q', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_R, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'R', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_S, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'S', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_T, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'T', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_U, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'U', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_V, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'V', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_W, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'W', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_X, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'X', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_Y, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'Y', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_Z, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'Z', 0, 9, 4, &uv);

	where.Set(0, 10);
	imgFont.Fill(al_internal::default_font::glyph_space, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U' ', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_a, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'a', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_b, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'b', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_c, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'c', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_d, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'd', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_e, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'e', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_f, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'f', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_g, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'g', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_h, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'h', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_i, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'i', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_j, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'j', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_k, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'k', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_l, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U'l', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_m, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'm', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_n, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'n', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_o, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'o', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_p, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'p', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_q, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'q', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_r, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'r', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_s, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U's', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_t, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U't', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_u, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'u', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_v, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'v', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_w, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'w', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_x, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'x', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_y, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'y', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_z, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'z', 0, 9, 3, &uv);

	where.Set(0, 20);
	imgFont.Fill(al_internal::default_font::glyph_1, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U'1', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_2, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'2', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_3, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'3', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_4, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'4', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_5, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'5', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_6, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'6', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_7, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'7', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_8, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'8', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_9, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'9', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_0, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'0', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_rbracket_o, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U'(', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_rbracket_c, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U')', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_sbracket_o, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U'[', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_sbracket_c, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U']', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_abracket_o, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'<', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_abracket_c, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'>', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_cbracket_o, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'{', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_cbracket_c, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'}', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_dot, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'.', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_comma, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U',', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_doubledot, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U':', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_semicolon, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U';', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_questmark, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'?', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_minus, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'-', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_plus, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'+', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_exclmark, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'!', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_equal, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'=', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_underscore, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'_', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_asterisk, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'*', 0, 9, 3, &uv);

	where.Set(0, 30);
	imgFont.Fill(al_internal::default_font::glyph_vertbar, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'|', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_apostrophe, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'\'', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_backtick, palette, alVec2u(2, 9), &where, 0, &uv);
	font->SetGlyph(U'`', 0, 9, 2, &uv);
	imgFont.Fill(al_internal::default_font::glyph_slash, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'/', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_bslash, palette, alVec2u(4, 9), &where, 0, &uv);
	font->SetGlyph(U'\\', 0, 9, 4, &uv);
	imgFont.Fill(al_internal::default_font::glyph_caret, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'^', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_percent, palette, alVec2u(8, 9), &where, 0, &uv);
	font->SetGlyph(U'%', 0, 9, 8, &uv);
	imgFont.Fill(al_internal::default_font::glyph_number, palette, alVec2u(6, 9), &where, 0, &uv);
	font->SetGlyph(U'#', 0, 9, 6, &uv);
	imgFont.Fill(al_internal::default_font::glyph_quot, palette, alVec2u(3, 9), &where, 0, &uv);
	font->SetGlyph(U'\"', 0, 9, 3, &uv);
	imgFont.Fill(al_internal::default_font::glyph_tilde, palette, alVec2u(7, 9), &where, 0, &uv);
	font->SetGlyph(U'~', 0, 9, 7, &uv);
	imgFont.Fill(al_internal::default_font::glyph_at, palette, alVec2u(8, 9), &where, 0, &uv);
	font->SetGlyph(U'@', 0, 9, 8, &uv);
	imgFont.Fill(al_internal::default_font::glyph_dollar, palette, alVec2u(5, 9), &where, 0, &uv);
	font->SetGlyph(U'$', 0, 9, 5, &uv);
	imgFont.Fill(al_internal::default_font::glyph_ampersand, palette, alVec2u(7, 9), &where, 0, &uv);
	font->SetGlyph(U'&', 0, 9, 7, &uv);
	imgFont.Fill(al_internal::default_font::glyph_space, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'\t', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_space, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'\r', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_space, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'\n', 0, 9, 1, &uv);
	imgFont.Fill(al_internal::default_font::glyph_space, palette, alVec2u(1, 9), &where, 0, &uv);
	font->SetGlyph(U'\v', 0, 9, 1, &uv);
	font->m_spacing = 1;

	alGSTextureInfo ti;
	ti.UseMipMaps(false);
	ti.m_filter = alGSTextureFilter::PPP;
	ti.m_imagePtr = &imgFont;
	alGSTexture* t = gs->CreateTexture(&ti);
	if(t)
		font->AddTexture(t);

	alLib::SaveImage("test.png", &imgFont, alSaveImageType::png);
}

alGUIFont* alLib::GetDefaultFont()
{
	return g_alLib->m_fontMini;
}

alGUIContext* alLib::CreateGUIContext(alSystemWindow* window, alGS* gs)
{
	return alCreate<alGUIContext>(window, gs);
}

alGUIFont* alLib::CreateGUIFont()
{
	return alCreate<alGUIFont>();
}

alGUIColorTheme* alLib::GetDefaultColorTheme()
{
	static bool isInit = false;
	if (!isInit)
	{
		isInit = true;
		g_alLib->m_colorTheme.m_text_textColor = ColorWhite;
		g_alLib->m_colorTheme.m_panel_bg1 = ColorGray;
		g_alLib->m_colorTheme.m_panel_bg2 = ColorDarkGray;
		
		g_alLib->m_colorTheme.m_popup_bg1 = 0xFFEFF4FF;
		g_alLib->m_colorTheme.m_popup_bg2 = 0xFFEFF4FF;
		g_alLib->m_colorTheme.m_popup_itemText = 0xFF111111;
		g_alLib->m_colorTheme.m_popup_bg1_mouseHover = 0xFF0C97FF;
		g_alLib->m_colorTheme.m_popup_bg2_mouseHover = 0xFF0C97FF;
		g_alLib->m_colorTheme.m_popup_itemText_mouseHover = 0xFFEFF4FF;
		
		
		g_alLib->m_colorTheme.m_button_bg1_enabled = 0xFF2B63FF;
		g_alLib->m_colorTheme.m_button_bg2_enabled = 0xFF2B63FF;
		g_alLib->m_colorTheme.m_button_bg1_disabled = 0xFFA0A0A0;
		g_alLib->m_colorTheme.m_button_bg2_disabled = 0xFF808080;
		g_alLib->m_colorTheme.m_button_bg1_mouseHover = 0xFF0C97FF;
		g_alLib->m_colorTheme.m_button_bg2_mouseHover = 0xFF0C97FF;
		g_alLib->m_colorTheme.m_button_bg1_press = 0xFF0074C6;
		g_alLib->m_colorTheme.m_button_bg2_press = 0xFF0074C6;
		g_alLib->m_colorTheme.m_button_textColor_enabled = 0xFFEEEEEE;
		g_alLib->m_colorTheme.m_button_textColor_disabled = 0xFFC0C0C0;
		g_alLib->m_colorTheme.m_button_textColor_mouseHover = 0xFFEEEEEE;
		g_alLib->m_colorTheme.m_button_textColor_press = ColorWhite;
		g_alLib->m_colorTheme.m_button_border = ColorGray;
		
		g_alLib->m_colorTheme.m_buttonIcon_disabled = ColorDarkGray;
		g_alLib->m_colorTheme.m_buttonIcon_enabled = ColorWhite;
		g_alLib->m_colorTheme.m_buttonIcon_mouseHover = ColorYellow;
		g_alLib->m_colorTheme.m_buttonIcon_press = ColorLime;

		g_alLib->m_colorTheme.m_edit_bg1 = ColorLightGrey;
		g_alLib->m_colorTheme.m_edit_bg2 = ColorLightGrey;
		g_alLib->m_colorTheme.m_edit_textEnabled = 0xFF111111;
		g_alLib->m_colorTheme.m_edit_textDisabled = 0xffD9D9D9;
		g_alLib->m_colorTheme.m_edit_textDefault = 0xFF666666;
		g_alLib->m_colorTheme.m_edit_textSelected = 0xFFEEEEEE;
		g_alLib->m_colorTheme.m_edit_bg1NA = 0xffD0D0D0;
		g_alLib->m_colorTheme.m_edit_bg2NA = 0xffD0D0D0;
		g_alLib->m_colorTheme.m_edit_selectbg = 0xFF3399FF;
		g_alLib->m_colorTheme.m_edit_textCursor = ColorWhite;
		g_alLib->m_colorTheme.m_edit_scrollBG = 0xFFD7DEF0;
		g_alLib->m_colorTheme.m_edit_scrollRect = 0xFFA7AEB0;
		g_alLib->m_colorTheme.m_edit_bottomBarBG = 0xFFECF4FC;
		g_alLib->m_colorTheme.m_edit_bottomBarUIText = 0xFF000000;
		g_alLib->m_colorTheme.m_edit_lineBarBG = 0xFFECF4FC;
		g_alLib->m_colorTheme.m_edit_lineBarText = 0xFF111111;
		g_alLib->m_colorTheme.m_edit_currlineBG = 0xFFD2DFF7;
		
		
		

		
		g_alLib->m_colorTheme.m_combo_bg_enabled = 0xFF2B63FF;
		g_alLib->m_colorTheme.m_combo_bg_disabled = 0xFFA0A0A0;
		g_alLib->m_colorTheme.m_combo_bg_mouseHover = 0xFF0C97FF;
		g_alLib->m_colorTheme.m_combo_bg_press = 0xFF0074C6;
		g_alLib->m_colorTheme.m_combo_textColor_enabled = 0xFFEEEEEE;
		g_alLib->m_colorTheme.m_combo_textColor_disabled = 0xFFC0C0C0;
		g_alLib->m_colorTheme.m_combo_textColor_mouseHover = 0xFFEEEEEE;
		g_alLib->m_colorTheme.m_combo_textColor_press = ColorWhite;
	}

	return &g_alLib->m_colorTheme;
}

int32_t alLib::StringStrcmp(const char32_t* p1, const char32_t* p2)
{
	const char32_t* s1 = (const char32_t*)p1;
	const char32_t* s2 = (const char32_t*)p2;
	char32_t c1, c2;

	do
	{
		c1 = (char32_t)*s1++;
		c2 = (char32_t)*s2++;
		if (c1 == U'\0')
			return c1 - c2;
	} while (c1 == c2);

	return c1 - c2;
}

