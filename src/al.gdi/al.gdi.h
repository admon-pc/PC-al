#ifndef _ALGDI_H_
#define _ALGDI_H_

#include "al.h"
#include "GS/alGS.h"
#include "Classes/alColor.h"

#include <objidl.h>
#include <gdiplus.h>

#include "al.gdi.texture.h"

#ifdef DrawText
#undef DrawText
#endif

struct alGSGDIWindowData
{
	//HDC m_dc;
	//HWND m_hwnd;
};


class alGSGDI : public alGS
{
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR       m_gdiplusToken;
	HBITMAP m_bitmapWindow;
	HDC m_bitmapWindowDC = 0;
	HRGN m_clipRegion;

	alColor m_clearColor;

	alArray<alGSGDIWindowData*> m_allWindowsData;

	alGSGDITexture* m_whiteTexture = 0;

public:
	alGSGDI();
	virtual ~alGSGDI();

	virtual const char* GetVideoDriverName() override;
	virtual bool Init(alSystemWindow*) override;
	virtual bool InitWindow(alSystemWindow*) override;
	virtual void GetDepthRange(alVec2f*) override;
	virtual void SetClearColor(const alColor&) override;
	virtual void BeginDraw() override;
	virtual void ClearDepth() override;
	virtual void ClearColor() override;
	virtual void ClearAll() override;
	virtual void Draw() override;
	virtual void DrawMeshGUI(alGSMesh* mesh, const alVec2f& offset, const alColor& color) override;
	virtual void EndDraw() override;
	virtual void BeginDrawGUI() override;
	virtual void EndDrawGUI() override;
	virtual void SwapBuffers() override;
	virtual void UpdateWindowData() override;
	virtual alGSTexture* CreateTexture(alGSTextureInfo*) override;
	virtual alGSMesh* CreateMesh(alGSMeshInfo*) override;
	virtual void SetRenderTarget(alGSTexture*) override;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
	virtual void SetScissorRect(const alVec4f& rect) override;
	virtual void EnableVSync() override;
	virtual void DisableVSync() override;
	virtual void EnableDepth() override;
	virtual void DisableDepth() override;
	virtual alGSShader* CreateShader(const alGSShaderCreationInfo&) override;
	virtual void SetShader(alGSShader*) override;
	virtual alGSTexture* GetWhiteTexture() override;
	virtual void SetMesh(alGSMesh*) override;
	virtual void SetRasterizationType(alGSRasterizationType) override;
	virtual void SetPrimitiveType(alGSPrimitiveType) override;
	virtual void DrawRectangle(const alVec4f& corners, 
		const alColor& color1, const alColor& color2, 
		alGSTexture* texture = 0, alVec4f* UVs = 0)override;
	virtual void DrawText(const char32_t* text, uint32_t size, 
		alGUIFont* font, const alVec2f& position, 
		const alColor& color, alVec2f* endPoint)override;

};

#endif
