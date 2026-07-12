#include <windows.h>

#include "al.h"
#include "Image/alImage.h"

#include "System/alSystemWindow.h"
#include "GS/alGS.h"
#include "Input/alInput.h"
#include "Classes/alColor.h"

#include "GUI/alGUI.h"

#include "Archive/alArchive.h"
#include "Containers/alArray.h"

#ifdef LoadImage
#undef LoadImage
#endif


AL_LINK_LIBRARY(al);

enum
{
	TextureID_NEW_GREEN,
	TextureID_NEW_RED,
	TextureID_OPEN_GREEN,
	TextureID_OPEN_RED,
	TextureID_PLUS_GREEN,
	TextureID_PLUS_RED,
	TextureID_MINUS_GREEN,
	TextureID_MINUS_RED,
	TextureID_FROMSCRATCH_GREEN,
	TextureID_FROMSCRATCH_RED,
	TextureID_GENERATE_GREEN,
	TextureID_GENERATE_RED,
	
	TextureID__end,
};

struct GlyphNode
{
	alImage* image = 0; //61 для равно
	
	int textureID = 0;

	int overhang = 0;
	int underhang = 0;
	
	int sizeX = 0;
	int sizeY = 0;

	int leftTopX = 0;
	int leftTopY = 0;
	int rightBottomX = 0;
	int rightBottomY = 0;

	char32_t character = 0;
};


class SystemWindowCallback;
class FontTool
{
	void _state_NewOrOpen();
	void _state_GenerateOrFromScratch();
	void _state_Generate();
	void _state_Edit();

	void (FontTool::*OnRun)();

public:
	FontTool();
	~FontTool();

	bool Init();
	void Run();

	void create_tmp_font();

	bool m_run = true;
	SystemWindowCallback* m_windowCallback = 0;
	alSystemWindow* m_mainWindow = 0;

	alGS* m_gs = 0;

	GlyphNode* myglyphs[0x10FFFF];

	alGSTexture* m_whiteTexture = 0;
	
	alGSTextureCacheNode* m_guiTextureNodes[TextureID__end];
	alGSTextureCache* m_guiTextures = 0;
};

class SystemWindowCallback : public alSystemWindowCallback
{
	FontTool* m_demo = 0;
public:
	SystemWindowCallback(FontTool* dd) : m_demo(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		if (m_demo->m_gs)
		{
			m_demo->m_gs->UpdateWindowData();
		}
	}
	
	virtual alVec2i OnGPUUpdateSize(alSystemWindow* w) 
	{
		alVec2i s;
		s.x = w->m_clientSize.x / 2;
		s.y = w->m_clientSize.y / 2;
		return s; 
	}
	
	virtual alVec2i OnMinMaxInfo(alSystemWindow* w)
	{
		return alVec2i(800, 600); 
	}

	virtual void OnClose(alSystemWindow* window)
	{
		auto windowID = window->GetID();
		if (!windowID)
		{
			m_demo->m_run = false;
		}
	}
};

class CPopup
{
public:
	CPopup();
	CPopup(const CPopup&);
	~CPopup();

	CPopup* CreateSubMenu(const wchar_t* text);
	void AddItem(const wchar_t*, uint32_t id, const wchar_t* shortcut);
	void AddSeparator();
	void Show(uint32_t x, uint32_t y);


	HMENU m_hPopupMenu = 0;

	alArray<CPopup*> m_subMenus;
};


CPopup::CPopup()
{
	m_hPopupMenu = CreatePopupMenu();
}

CPopup::CPopup(const CPopup&)
{
	m_hPopupMenu = CreatePopupMenu();
}

CPopup::~CPopup()
{
	for (size_t i = 0, sz = m_subMenus.size(); i < sz; ++i)
	{
		delete m_subMenus[i];
	}
	DestroyMenu(m_hPopupMenu);
}

CPopup* CPopup::CreateSubMenu(const wchar_t* text)
{
	m_subMenus.push_back(new CPopup);
	//	AppendMenu(m_hPopupMenu, MF_POPUP | MF_BYPOSITION | MF_STRING, (UINT_PTR)newSubMenu->m_hPopupMenu, text);
	return 0;
}

void CPopup::AddItem(const wchar_t* _text, uint32_t id, const wchar_t* shortcut)
{
	alStringW text = _text;
	if (shortcut)
	{
		text += L"\t";
		text += shortcut;
	}
	AppendMenu(m_hPopupMenu, MF_BYPOSITION | MF_STRING, id, text.data());
}

void CPopup::AddSeparator()
{
	AppendMenu(m_hPopupMenu, MF_SEPARATOR, 0, 0);
}

void CPopup::Show(uint32_t x, uint32_t y)
{
	/*HWND hWnd = (HWND)g_app->m_hwnd;
	SetForegroundWindow(hWnd);
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(m_hPopupMenu, TPM_TOPALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);*/
}



FontTool::FontTool()
{
	CPopup* pp = new CPopup;
	OnRun = &FontTool::_state_NewOrOpen;

	for (size_t i = 0; i < 0x10FFFF; i++)
	{
		myglyphs[i] = 0;
	}

	for (int i = 0; i < TextureID__end; ++i)
	{
		m_guiTextureNodes[i] = 0;
	}
	m_windowCallback = alCreate<SystemWindowCallback>(this);
}

FontTool::~FontTool()
{
	for (size_t i = 0; i < 0x10FFFF; i++)
	{
		if (myglyphs[i])
		{
			AL_DESTROY(myglyphs[i]->image);
		}

		AL_DESTROY(myglyphs[i]);
	}
	AL_DESTROY(m_guiTextures);
	AL_DESTROY(m_gs);
	AL_DESTROY(m_windowCallback);
	AL_DESTROY(m_mainWindow);
}

bool FontTool::Init()
{
	m_mainWindow = alLib::CreateSystemWindow(m_windowCallback);
	if (!m_mainWindow)
		return false;


	m_mainWindow->Show();

	m_gs = alLib::CreateGS(alVideoDriverType::Direct3D11);
	if (!m_gs->Init(m_mainWindow))
		return false;

	m_whiteTexture = m_gs->GetWhiteTexture();
	m_guiTextures = alCreate<alGSTextureCache>(m_gs);
	m_guiTextureNodes[TextureID_FROMSCRATCH_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/fscrg.png");
	m_guiTextureNodes[TextureID_FROMSCRATCH_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/fscrr.png");
	m_guiTextureNodes[TextureID_GENERATE_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/geng.png");
	m_guiTextureNodes[TextureID_GENERATE_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/genr.png");
	m_guiTextureNodes[TextureID_MINUS_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/minusg.png");
	m_guiTextureNodes[TextureID_MINUS_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/minusr.png");
	m_guiTextureNodes[TextureID_NEW_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/newg.png");
	m_guiTextureNodes[TextureID_NEW_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/newr.png");
	m_guiTextureNodes[TextureID_OPEN_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/openg.png");
	m_guiTextureNodes[TextureID_OPEN_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/openr.png");
	m_guiTextureNodes[TextureID_PLUS_GREEN] = m_guiTextures->GetTexture("../data/tools/fonttool/plsg.png");
	m_guiTextureNodes[TextureID_PLUS_RED] = m_guiTextures->GetTexture("../data/tools/fonttool/plsr.png");

	return true;
}

void FontTool::Run()
{
	float32_t* dt = alLib::GetDeltaTime();

	alInput* input = alLib::GetInput();
	while (m_run)
	{
		alLib::Update();

		((*this).*(OnRun))();

	}
}

void FontTool::_state_NewOrOpen()
{
	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();
	m_gs->BeginDrawGUI();

	uint32_t tw = m_guiTextureNodes[TextureID_NEW_RED]->m_texture->GetWidth();
	uint32_t th = m_guiTextureNodes[TextureID_NEW_RED]->m_texture->GetHeight();

	alVec4f buttonRect;
	buttonRect.x = 0.f;
	buttonRect.y = 0.f;
	buttonRect.z = tw;
	buttonRect.w = th;

	auto input = alLib::GetInput();

	int textureID = TextureID_NEW_RED;
	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
	{
		textureID = TextureID_NEW_GREEN;

		if (input->m_isLMBDown)
		{
			OnRun = &FontTool::_state_GenerateOrFromScratch;
		}
	}

	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
		m_guiTextureNodes[textureID]->m_texture);


	tw = m_guiTextureNodes[TextureID_OPEN_RED]->m_texture->GetWidth();
	th = m_guiTextureNodes[TextureID_OPEN_RED]->m_texture->GetHeight();
	buttonRect.x = 0.f;
	buttonRect.y += buttonRect.w + 5.f;
	buttonRect.z = tw;
	buttonRect.w = buttonRect.y + th;

	textureID = TextureID_OPEN_RED;
	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
	{
		textureID = TextureID_OPEN_GREEN;

		if (input->m_isLMBDown)
		{
			//OnRun = &FontTool::_state_GenerateOrFromScratch;
		}
	}
	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
		m_guiTextureNodes[textureID]->m_texture);

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
}

void FontTool::_state_GenerateOrFromScratch()
{
	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();
	m_gs->BeginDrawGUI();

	int textureID = TextureID_GENERATE_RED;
	uint32_t tw = m_guiTextureNodes[textureID]->m_texture->GetWidth();
	uint32_t th = m_guiTextureNodes[textureID]->m_texture->GetHeight();

	alVec4f buttonRect;
	buttonRect.x = 0.f;
	buttonRect.y = 0.f;
	buttonRect.z = tw;
	buttonRect.w = th;

	auto input = alLib::GetInput();

	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
	{
		textureID = TextureID_GENERATE_GREEN;

		if (input->m_isLMBDown)
		{
			OnRun = &FontTool::_state_Generate;

			create_tmp_font();
		}
	}

	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
		m_guiTextureNodes[textureID]->m_texture);

	textureID = TextureID_FROMSCRATCH_RED;
	tw = m_guiTextureNodes[textureID]->m_texture->GetWidth();
	th = m_guiTextureNodes[textureID]->m_texture->GetHeight();
	buttonRect.x = 0.f;
	buttonRect.y += buttonRect.w + 5.f;
	buttonRect.z = (float32_t)tw;
	buttonRect.w = buttonRect.y + th;

	if (alMath::PointInRect(input->m_cursorCoords.x, input->m_cursorCoords.y, buttonRect))
	{
		textureID = TextureID_FROMSCRATCH_GREEN;

		if (input->m_isLMBDown)
		{
			//OnRun = &FontTool::_state_Generate;
		}
	}
	m_gs->DrawRectangle(buttonRect, ColorWhite, ColorWhite,
		m_guiTextureNodes[textureID]->m_texture);

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
}

void FontTool::_state_Generate()
{
}

void FontTool::_state_Edit()
{
}

void FontTool::create_tmp_font()
{
	int fontSize = 10;
	HDC dc = CreateDC(L"DISPLAY", L"DISPLAY", 0, 0);

	bool bold = false;
	bool italic = false;
	bool aa = false;

	HFONT font = CreateFontW(
		-MulDiv(fontSize, GetDeviceCaps(dc, LOGPIXELSY), 72), 0,
		0, 0,
		bold ? FW_BOLD : 0,
		italic, 0, 0,
		ANSI_CHARSET, 0, 0,
		aa ? ANTIALIASED_QUALITY : 0,
		0, L"Consolas");

	SelectObject(dc, font);
	SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);

	int size = GetFontUnicodeRanges(dc, 0);
	
	uint8_t* buf = new uint8_t[size];
	LPGLYPHSET glyphs = (LPGLYPHSET)buf;
	GetFontUnicodeRanges(dc, glyphs);

	int maxSizeX = 0;
	int maxSizeY = 0;
	//alImage img;

	//printf("glyphs->cRanges %i\n", glyphs->cRanges);
	for (DWORD range = 0; range < glyphs->cRanges; ++range)
	{
		WCRANGE* current = &glyphs->ranges[range];
		//printf("Range %i, num of glyphs %i\n", range, current->cGlyphs);
		for (int ch = current->wcLow; ch < current->wcLow + current->cGlyphs; ++ch)
		{
			wchar_t currentchar = ch;
			if (IsDBCSLeadByte((BYTE)ch))
				continue;

		//	wprintf(L"Char%i: %c\n", ch, currentchar);

			// get the dimensions
			SIZE size;
			ABC abc;
			GetTextExtentPoint32W(dc, &currentchar, 1, &size);
			
			int underhang = 0;
			int overhang = 0;

			if (GetCharABCWidthsW(dc, currentchar, currentchar, &abc)) // for unicode fonts, get overhang, underhang, width
			{
			//	size.cx = abc.abcB;		// full font width (ignoring padding/underhang )
				underhang = abc.abcA;	// underhang/padding left - can also be negative (in which case it's overhang left)
				overhang = abc.abcC;	// overhang/padding right - can also be negative (in which case it's underhand right)

				if (abc.abcB - abc.abcA + abc.abcC < 1)
					continue; // nothing of width 0
			}
			if (size.cy < 1)
				continue;
			if (size.cx < 1)
				continue;
			
			HBITMAP bmp = CreateCompatibleBitmap(dc, size.cx, size.cy);
			HDC bmpdc = CreateCompatibleDC(dc);
			LOGBRUSH lbrush;
			lbrush.lbColor = RGB(0, 0, 0);
			lbrush.lbHatch = 0;
			lbrush.lbStyle = BS_SOLID;
			HBRUSH brush = CreateBrushIndirect(&lbrush);
			HPEN pen = CreatePen(PS_NULL, 0, 0);
			HGDIOBJ oldbmp = SelectObject(bmpdc, bmp);
			HGDIOBJ oldbmppen = SelectObject(bmpdc, pen);
			HGDIOBJ oldbmpbrush = SelectObject(bmpdc, brush);
			HGDIOBJ oldbmpfont = SelectObject(bmpdc, font);
			SetTextColor(bmpdc, RGB(255, 255, 255));
			Rectangle(bmpdc, 0, 0, size.cx, size.cy);
			SetBkMode(bmpdc, TRANSPARENT);

			

			TextOutW(bmpdc, 
				0,
				0,
				&currentchar,
				1);


			BITMAP b;
			GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);
			WORD cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);
			

			PBITMAPINFO pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
				sizeof(BITMAPINFOHEADER));
			pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			pbmi->bmiHeader.biWidth = b.bmWidth;
			pbmi->bmiHeader.biHeight = b.bmHeight;
			pbmi->bmiHeader.biPlanes = b.bmPlanes;
			pbmi->bmiHeader.biBitCount = b.bmBitsPixel;
			pbmi->bmiHeader.biCompression = BI_RGB;
			pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
				* pbmi->bmiHeader.biHeight;
			pbmi->bmiHeader.biClrImportant = 0;
			LPBYTE lpBits; // memory pointer
			PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbmi;
			lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
			GetDIBits(dc, bmp, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);
			if (cClrBits <= 8) // we're not supporting these
			{
			}
			else if (cClrBits <= 16)
			{
			}
			else if (cClrBits <= 24)
			{
			}
			else
			{
				alImage* img = alCreate<alImage>();
				img->Create(size.cx, size.cy);

				int rowsize = img->m_pitch;
				uint8_t* dst = img->m_data;
				uint8_t* src = lpBits;
				for (int i = 0; i < pbih->biHeight; ++i)
				{
					memcpy(dst, src, rowsize);

					dst += rowsize;
					src += rowsize;
				}

				uint8_t* m;
				for (m = img->m_data; m < img->m_data+ img->m_dataSize; m += 4)
				{
					if (m[0] > 0) // pixel has colour
					{
					//	printf("%i ", m[0]);

						m[3] = m[0];  // set alpha
						m[0] = m[1] = m[2] = 255; // everything else is full
					}
				}

				//char cbuf[100];
				//sprintf_s(cbuf, 100, "file0x%X.png", (uint32_t)ch);
				//alLib::SaveImage(cbuf, &img, alSaveImageType::png);
				img->FlipVertical();
				myglyphs[currentchar] = alCreate<GlyphNode>();
				myglyphs[currentchar]->image = img;
				myglyphs[currentchar]->character = currentchar;
				myglyphs[currentchar]->overhang = overhang;
				myglyphs[currentchar]->underhang = underhang;
				myglyphs[currentchar]->sizeX = size.cx;
				myglyphs[currentchar]->sizeY = size.cy;
				myglyphs[currentchar]->leftTopX = 0;
				myglyphs[currentchar]->leftTopY = 0;
				myglyphs[currentchar]->rightBottomX = 0;
				myglyphs[currentchar]->rightBottomY = 0;

				if (size.cx > maxSizeX)
					maxSizeX = size.cx;
				if (size.cy > maxSizeY)
					maxSizeY = size.cy;
			}

			LocalFree(pbmi);
			GlobalFree(lpBits);
			DeleteDC(bmpdc);
			DeleteObject(brush);
			DeleteObject(pen);
			DeleteObject(bmp);
		}
	}
	
	int imageIndex = 0;
	alImage img;
	img.Create(512, 512);
	int drawPositionX = 0;
	int drawPositionY = 0;

	alUnicodeString ustr;

	alArray<alStringA> zipfiles;

	for (int i = 0; i < 0x10FFFF; ++i)
	{
		if (myglyphs[i])
		{
			myglyphs[i]->textureID = imageIndex;

			int rbX = drawPositionX + myglyphs[i]->sizeX;
			int rbY = drawPositionY + myglyphs[i]->sizeY;

			if (rbX > 512)
			{
				drawPositionX = 0;
				rbX = drawPositionX + myglyphs[i]->sizeX;

				drawPositionY = rbY;
				rbY = drawPositionY + myglyphs[i]->sizeY;

				if (rbY > 512)
				{
					char buf[100];
					sprintf_s(buf, 100, "font%i.png", imageIndex);
					alLib::SaveImage(buf, &img, alSaveImageType::png);
					
					zipfiles.push_back(buf);

					imageIndex++;

					drawPositionY = 0;
					rbY = drawPositionY + myglyphs[i]->sizeY;

					img.Fill(alColor(0.f,0.f,0.f,0.f));
				}
			}

			img.Fill(myglyphs[i]->image, alVec2u(drawPositionX, drawPositionY), 0, 0);

			// write code, not char
			ustr.Append((uint32_t)myglyphs[i]->character);

			ustr.Append(" ");
			ustr.Append(drawPositionX);
			ustr.Append(" ");
			ustr.Append(drawPositionY);
			ustr.Append(" ");
			ustr.Append(drawPositionX + myglyphs[i]->sizeX);
			ustr.Append(" ");
			ustr.Append(drawPositionY + myglyphs[i]->sizeY);
			ustr.Append(" ");
			ustr.Append(myglyphs[i]->underhang);
			ustr.Append(" ");
			ustr.Append(myglyphs[i]->overhang);
			ustr.Append(" ");
			ustr.Append(myglyphs[i]->textureID);
			ustr.Append("\n");

			drawPositionX = rbX;
		}
	}

	if (imageIndex == 0)
	{
		char buf[100];
		sprintf_s(buf, 100, "font%i.png", imageIndex);
		alLib::SaveImage(buf, &img, alSaveImageType::png);
		zipfiles.push_back(buf);
	}

	{
		char buf[100];
		sprintf_s(buf, 100, "font.txt");
		ustr.SaveToFileUTF32(buf);
		zipfiles.push_back(buf);
	}

	if(zipfiles.m_size)
	{
		alArray<alFileBuffer*> fbs;
		alArchiveFileZipFileData* files = new alArchiveFileZipFileData[zipfiles.m_size];
		
		for (size_t i = 0; i < zipfiles.m_size; ++i)
		{
			sprintf_s(files[i].m_fileName, 0xff, zipfiles.m_data[i].c_str());
			
			alFileBuffer* fb = new alFileBuffer;
			fbs.push_back(fb);
			fb->ReadFile(files[i].m_fileName);
			files[i].m_data = fb->Data();
			files[i].m_dataSize = fb->Size();
		}

		alArchive::SaveZip(files, zipfiles.m_size, "font.zip");

		for (size_t i = 0; i < zipfiles.m_size; ++i)
		{
			alLib::RemoveFile(files[i].m_fileName);
		}

		for (size_t i = 0; i < fbs.m_size; ++i)
		{
			delete fbs.m_data[i];
		}
		delete[] files;
	}
}
	

int main()
{
	alLib::InitializeLib();

	FontTool* dd = new FontTool;
	if (dd->Init())
	{
		dd->Run();
	}

	return 1;
}


