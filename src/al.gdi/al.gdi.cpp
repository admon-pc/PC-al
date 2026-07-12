#include "al.gdi.h"
#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"
#include "Image/alImage.h"
#include "GUI/alGUI.h"

#ifdef DrawText
#undef DrawText
#endif
#pragma comment (lib,"Gdiplus.lib")

alGSGDI* g_gdi = 0;

alGS* alCreateGS_WinGDI()
{
	return alCreate<alGSGDI>();
}

alGSGDI::alGSGDI()
{
	g_gdi = this;
}

alGSGDI::~alGSGDI()
{

	/*for (size_t i = 0; i < m_allWindowsData.m_size; ++i)
	{
		auto o = m_allWindowsData.m_data[i];
	}*/

	AL_DESTROY(m_whiteTexture)

}

const char* alGSGDI::GetVideoDriverName()
{
	return "GDI+";
}

bool alGSGDI::Init(alSystemWindow* sw)
{
	m_activeWindow = sw;
	alSystemWindowOSDataWin32* win32window = (alSystemWindowOSDataWin32*)sw->GetOSData();
	//alGSGDIWindowData* pwindowGPUData = new alGSGDIWindowData;
	//pwindowGPUData->m_hwnd = win32window->m_hwnd;
	//pwindowGPUData->m_dc = GetDC(win32window->m_hwnd);
	//sw->m_GPUData = pwindowGPUData;
	//m_allWindowsData.push_back(pwindowGPUData);

	m_bitmapWindow = CreateCompatibleBitmap(win32window->m_dc, 300, 300);
	m_bitmapWindowDC = CreateCompatibleDC(win32window->m_dc);
	m_clipRegion = CreateRectRgn(
		0,
		0,
		10,
		10);
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	SetClearColor(ColorMediumSeaGreen);
	SetBkMode(m_bitmapWindowDC, TRANSPARENT);

	return true;
}

bool alGSGDI::InitWindow(alSystemWindow* sw)
{
	return true;
}

void alGSGDI::GetDepthRange(alVec2f* d)
{
	AL_ASSERT_ST(d);
	d->x = 0.f;
	d->y = 1.f;
}

void alGSGDI::SetClearColor(const alColor& c)
{
	m_clearColor = c;
}

void alGSGDI::BeginDraw()
{
	SelectObject(m_bitmapWindowDC, m_bitmapWindow);
}

void alGSGDI::ClearDepth()
{
}

void alGSGDI::ClearColor()
{
	SetScissorRect(alVec4f(0, 0, m_activeWindow->m_clientSize.x, m_activeWindow->m_clientSize.y));
	DrawRectangle(alVec4f(0, 0, m_activeWindow->m_clientSize.x, m_activeWindow->m_clientSize.y), m_clearColor, m_clearColor);
}

void alGSGDI::ClearAll()
{
	ClearColor();
}

void alGSGDI::Draw()
{
}

void alGSGDI::DrawMeshGUI(alGSMesh* mesh, const alVec2f& offset, const alColor& color)
{
}

void alGSGDI::EndDraw()
{
	alSystemWindowOSDataWin32* win32window = (alSystemWindowOSDataWin32*)m_activeWindow->GetOSData();
//	alGSGDIWindowData* windowGPUData = (alGSGDIWindowData*)m_activeWindow->m_GPUData;

	BitBlt(win32window->m_dc, 0, 0, m_activeWindow->m_clientSize.x,
		m_activeWindow->m_clientSize.y, m_bitmapWindowDC, 0, 0, SRCCOPY);
}

void alGSGDI::SwapBuffers()
{
}

void alGSGDI::UpdateWindowData()
{
	alSystemWindowOSDataWin32* data = (alSystemWindowOSDataWin32*)m_activeWindow->GetOSData();
	DeleteObject(m_bitmapWindow);
	m_bitmapWindow = CreateCompatibleBitmap(data->m_dc, m_activeWindow->m_clientSize.x, m_activeWindow->m_clientSize.y);
}

alGSTexture* alGSGDI::CreateTexture(alGSTextureInfo* ti)
{
	alGSGDITexture* gditexture = alCreate<alGSGDITexture>();
	gditexture->m_ti = *ti;
	if (!gditexture->Init())
	{
		AL_DESTROY(gditexture);
	}
	
	return gditexture;
}

alGSMesh* alGSGDI::CreateMesh(alGSMeshInfo* mi)
{
	return 0;
}

void alGSGDI::SetRenderTarget(alGSTexture* t)
{
}

void alGSGDI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
}

void alGSGDI::SetScissorRect(const alVec4f& rect)
{
	DeleteObject(m_clipRegion);
	m_clipRegion = CreateRectRgn(
		(int)rect.x,
		(int)rect.y,
		(int)rect.z,
		(int)rect.w);
	SelectClipRgn(m_bitmapWindowDC, m_clipRegion);
}

void alGSGDI::EnableVSync()
{
}

void alGSGDI::DisableVSync()
{
}

void alGSGDI::EnableDepth()
{
}

void alGSGDI::DisableDepth()
{
}

alGSShader* alGSGDI::CreateShader(const alGSShaderCreationInfo& inf)
{
	return 0;
}

void alGSGDI::SetShader(alGSShader* s)
{
}

alGSTexture* alGSGDI::GetWhiteTexture()
{
	return m_whiteTexture;
}

void alGSGDI::SetMesh(alGSMesh* gsmesh)
{
}

void alGSGDI::SetRasterizationType(alGSRasterizationType rt)
{
}

void alGSGDI::SetPrimitiveType(alGSPrimitiveType t)
{
}

void alGSGDI::DrawRectangle(const alVec4f& rect,
	const alColor& color, const alColor& color2,
	alGSTexture* texture, alVec4f* UVs)
{
	SelectObject(m_bitmapWindowDC, GetStockObject(DC_BRUSH));
	SetDCBrushColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));

	SelectObject(m_bitmapWindowDC, GetStockObject(DC_PEN));
	SetDCPenColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));

	Rectangle(m_bitmapWindowDC, rect.x, rect.y, rect.z, rect.w);
}

void alGSGDI::BeginDrawGUI()
{
}

void alGSGDI::EndDrawGUI()
{
}

void alGSGDI::DrawText(const char32_t* text, 
	uint32_t size, 
	alGUIFont* font, 
	const alVec2f& position, 
	const alColor& color, 
	alVec2f* endPoint)
{
	alGUIFont_gdi* gdifont = (alGUIFont_gdi*)font;

	SelectObject(m_bitmapWindowDC, gdifont->m_font);

	alVec2f _position = position;
	static alColor oldcolor;
	if (oldcolor != color)
	{
		oldcolor = color;
	}
		SetTextColor(m_bitmapWindowDC, RGB(color.GetAsByteRed(), color.GetAsByteGreen(), color.GetAsByteBlue()));

	for (uint32_t i = 0; i < size; ++i)
	{
		wchar_t wch = text[i];
		TextOutW(m_bitmapWindowDC, _position.x, _position.y, &wch, 1);

		_position.x += font->m_maxWidth;
		if (wch == L' ')
			_position.x += font->m_spaceWidth;
		if (wch == L'\t')
			_position.x += font->m_tabWidth;

	}

	if (endPoint)
	{
		*endPoint = _position;
	}
}
