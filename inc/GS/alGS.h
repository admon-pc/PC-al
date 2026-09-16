#ifndef _AL_GSH_
#define _AL_GSH_

#include "GS/alGSTexture.h"
#include "GS/alGSMesh.h"
#include "GS/alGSShader.h"

#include "GS/alGSTextureCache.h"

enum class alGSRasterizationType : uint32_t
{
	Wireframe,
	WireframeNoCull,
	Solid,
	SolidNoCull,
};

enum class alGSPrimitiveType : uint32_t
{
	Triangle,
	Line,
	Point
};

class alGS
{
protected:
	alSystemWindow* m_activeWindow = 0;
	alVideoDriverType m_type = alVideoDriverType::Unknown;
public:
	alGS() {}
	virtual ~alGS() {}

	virtual alVideoDriverType GetType()
	{
		return m_type;
	}

	virtual const char* GetVideoDriverName() = 0;
	virtual bool Init(alSystemWindow*) = 0;
	virtual bool InitWindow(alSystemWindow*) = 0;
	virtual void GetDepthRange(alVec2f*) = 0;
	virtual void SetClearColor(const alColor&) = 0;

	/*
	Usualy drawing happens like this
	m_gs->BeginDraw();
	m_gs->ClearAll();
	DrawAll3D();
	m_gs->BeginDrawGUI();
	DrawAll2D();
	m_gui->m_context->Draw(m_dt);
	m_gs->EndDrawGUI();
	m_gs->EndDraw();
	m_gs->SwapBuffers();

	Drawing happens into 2 RTTs.
	1. RTT for window, usually for 3D drawing
	2. RTT for GUI.
	BeginDraw will set 1st render target.
	BeginDrawGUI will set 2st render target.
	EndDraw will combine this RTTs together and draw it on window.
	*/
	virtual void BeginDraw() = 0;
	virtual void ClearDepth() = 0;
	virtual void ClearColor() = 0;
	virtual void ClearAll() = 0;
	virtual void Draw() = 0;

	// Usually you need to draw many lines.
	// If you need really manymany, use mesh alMeshVertexType::Line.
	// To avoid multiple internal calls, first call BeginDrawLine3D
	virtual void BeginDrawLine3D() = 0;
	virtual void DrawLine3D(const alVec4& _p1, const alVec4& _p2, const alColor& color) = 0;

	// Same like with line 3D
	virtual void BeginDrawLine2D() = 0;
	virtual void DrawLine2D(const alVec2f& _p1, const alVec2f& _p2, const alColor& color) = 0;

	// This method will save preavious shader data and other previous data
	// Then will draw mesh
	// Then restore all saved data
	// 
	// It works in alGUIButton, for buttons with round corners
	//   (it works inside BeginDrawGUI and EndDrawGUI)
	// I am not sure if it will work in other places
	// Draw mesh with alMeshVertexType::GUI
	virtual void DrawMeshGUI(alGSMesh* mesh, const alVec2f& offset, const alColor& color) = 0;

	virtual void EndDraw() = 0;

	// it will set RTT for GUI and shader for GUI
	// it will set viewport
	// it will set Scissor Rect
	// it will call ActivateGUIShader
	virtual void BeginDrawGUI(bool clear = true) = 0;
	// it will set shader for GUI
	// call this if you use DrawLine3D or DrawLine2D and you need to draw rectangles
	// BeginDrawGUI will activate GUI shader.
	// But, you may want to draw 2D line.
	virtual void ActivateGUIShader() = 0; 

	/* Call this between 
		m_gs->BeginDrawGUI();
		
		m_gs->EndDrawGUI();

		If you need to draw lines and then again rectangles, then
		don't forget to set shaders, sing ActivateGUIShader

		m_gs->BeginDrawGUI();
		m_gui->m_context->Draw(m_dt);

		m_gs->BeginDrawLine2D();
		m_gs->DrawLine2D(...);

		m_gs->ActivateGUIShader();
		m_gs->DrawRectangle(...);

		m_gs->EndDrawGUI();
	*/
	virtual void DrawRectangle(const alVec4f& corners, const alColor& color1, const alColor& color2, alGSTexture* texture = 0, alVec4f* UVs = 0) = 0;
	virtual void DrawRectangle(const alVec4f& corners, const alColor& color, alGSTexture* texture = 0, alVec4f* UVs = 0) = 0;
	virtual void DrawText(const char32_t* text, uint32_t size, alGUIFont* font, const alVec2f& position, const alColor& color, alVec2f* endPoint = 0) = 0;

	// nothing will happen because method is empty.
	// don't know what to do, but in theory if there was Begin, 
	// then must be also be End.
	virtual void EndDrawGUI() = 0;
	
	

	// It will call final functions of GPU API
	// like SwapBuffers for OpenGL
	//      IDXGISwapChain::Present for Direct3D
	virtual void SwapBuffers() = 0;

	virtual void UpdateWindowData() = 0;
	virtual alGSTexture* CreateTexture(alGSTextureInfo*) = 0;
	
	//virtual alGSMesh* CreateMesh(alGSMeshInfo*) = 0;
	virtual alGSMesh* CreateMesh(alMesh*) = 0;

	virtual void SetRenderTarget(alGSTexture*) = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void SetScissorRect(const alVec4f& rect) = 0;
	virtual void EnableVSync() = 0;
	virtual void DisableVSync() = 0;
	virtual void EnableDepth() = 0;
	virtual void DisableDepth() = 0;
	virtual alGSShader* CreateShader(const alGSShaderCreationInfo&) = 0;
	virtual void SetShader(alGSShader*) = 0;
	virtual alGSTexture* GetWhiteTexture() = 0;
	virtual void SetMesh(alGSMesh*) = 0;
	virtual void SetRasterizationType(alGSRasterizationType) = 0;
	virtual void SetPrimitiveType(alGSPrimitiveType) = 0;
	

	virtual alGSTexture* CreateRTT(alGSTextureInfo* ti)
	{
		AL_ASSERT_ST(ti);
		ti->m_textureType = alGSTextureType::RTT;
		return this->CreateTexture(ti);
	}
	virtual alGSTexture* CreateTexture(alImage* image)
	{
		alGSTextureInfo ti(image);
		ti.UseMipMaps(false);
		ti.m_filter = alGSTextureFilter::PPP;
		ti.m_imagePtr = image;
		return this->CreateTexture(&ti);
	}

	virtual alGSTexture* CreateTexturePoint(alImage* image, bool usemipmaps = false)
	{
		alGSTextureInfo ti(image);
		ti.UseMipMaps(usemipmaps);
		ti.m_filter = alGSTextureFilter::PPP;
		ti.m_imagePtr = image;
		return this->CreateTexture(&ti);
	}
	virtual alGSTexture* CreateTextureLinear(alImage* image, bool usemipmaps = false)
	{
		alGSTextureInfo ti(image);
		ti.UseMipMaps(usemipmaps);
		ti.m_filter = alGSTextureFilter::LLL;
		ti.m_imagePtr = image;
		return this->CreateTexture(&ti);
	}

};

#endif

