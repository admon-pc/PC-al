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

	virtual void BeginDraw() = 0;
	virtual void ClearDepth() = 0;
	virtual void ClearColor() = 0;
	virtual void ClearAll() = 0;
	virtual void Draw() = 0;

	// Draw mesh with alMeshVertexType::GUI
	virtual void DrawMeshGUI(alGSMesh* mesh, const alVec2f& offset, const alColor& color) = 0;

	virtual void EndDraw() = 0;
	virtual void BeginDrawGUI() = 0;
	virtual void EndDrawGUI() = 0;
	virtual void SwapBuffers() = 0;
	virtual void UpdateWindowData() = 0;
	virtual alGSTexture* CreateTexture(alGSTextureInfo*) = 0;
	virtual alGSMesh* CreateMesh(alGSMeshInfo*) = 0;
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
	virtual void DrawRectangle(const alVec4f& corners, const alColor& color1, const alColor& color2, alGSTexture* texture = 0, alVec4f* UVs = 0) = 0;
	virtual void DrawRectangle(const alVec4f& corners, const alColor& color, alGSTexture* texture = 0, alVec4f* UVs = 0) = 0;
	virtual void DrawText(const char32_t* text, uint32_t size, alGUIFont* font, const alVec2f& position, const alColor& color, alVec2f* endPoint = 0) = 0;

	virtual alGSTexture* CreateRTT(alGSTextureInfo* ti)
	{
		AL_ASSERT_ST(ti);
		ti->m_textureType = alGSTextureType::RTT;
		return this->CreateTexture(ti);
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

