#ifndef _ALD3D11_H_
#define _ALD3D11_H_

#include "al.h"
#include "GS/alGS.h"
#include "Classes/alColor.h"

#include <d3d11.h>

#include "al.d3d11.texture.h"
#include "al.d3d11.mesh.h"
#include "al.d3d11.shader.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)if(x){x->Release();x=0;}
#endif

#ifdef DrawText
#undef DrawText
#endif

struct alGSD3D11MainTarget
{
	ID3D11RenderTargetView* m_targetView = 0;
	ID3D11Texture2D* m_depthStencilBuffer = 0;
	ID3D11DepthStencilView* m_depthStencilView = 0;
};

struct alGSD3D11WindowData
{
	IDXGISwapChain* m_swapChain = 0;

	// render to texture
	alD3D11Texture* m_rtt = 0;
	alVec2i m_rttSize;

	// render to quad
	alGSD3D11MainTarget m_mainTarget;
};

class alD3D11Shader_GUIRectangle : public alGSShaderCallback
{
public:
	alD3D11Shader_GUIRectangle() {}
	virtual ~alD3D11Shader_GUIRectangle()
	{
		AL_DESTROY(m_shader);
	}

	alD3D11Shader* m_shader = 0;

	struct cbVertex
	{
		alMatrix_t<float32_t> m_ProjMtx;
		alVec4f m_Corners;
		alColor m_Color1;
		alColor m_Color2;
		alVec4f m_UVs;
	}m_cbVertex_impl;
	//ID3D11Buffer* m_cbVertex;

	alD3D11GSShaderConstantBuffer* m_cb = 0;
	//virtual void OnSetConstants();

	virtual void OnSetShader() 
	{
		m_cb->VSSetConstantBuffers(0);
		m_cb->GSSetConstantBuffers(0);
	}
};

class alD3D11Shader_GUIMesh : public alGSShaderCallback
{
public:
	alD3D11Shader_GUIMesh() {}
	virtual ~alD3D11Shader_GUIMesh()
	{
		AL_DESTROY(m_shader);
	}

	alD3D11Shader* m_shader = 0;

	struct cbVertex
	{
		alMatrix_t<float32_t> m_ProjMtx;
		alVec2f m_Offset;
		alVec2f m_Padding;
	}m_cbVertex_impl;

	struct cbPixel {
		alColor m_Color;
	}m_cbPixel_impl;

	alD3D11GSShaderConstantBuffer* m_cbVertex = 0;
	alD3D11GSShaderConstantBuffer* m_cbPixel = 0;
	//virtual void OnSetConstants();

	virtual void OnSetShader()
	{
		m_cbVertex->VSSetConstantBuffers(0);
		m_cbPixel->PSSetConstantBuffers(0);
	}
};

class alD3D11Shader_screenQuad : public alGSShaderCallback
{
public:
	alD3D11Shader_screenQuad() {}
	virtual ~alD3D11Shader_screenQuad() 
	{
		AL_DESTROY(m_shader);
	}
	
	alD3D11Shader* m_shader = 0;

	/*struct cbVertex 
	{
		alMat4 m_ProjMtx;
		alVec4f m_Corners;
		alColor m_Color1;
		alColor m_Color2;
		alVec4f m_UVs;
	}m_cbVertex_impl;*/
	//ID3D11Buffer* m_cbVertex;

	alD3D11GSShaderConstantBuffer* m_cb = 0;
};

class alGSD3D11 : public alGS
{
	ID3D11Device* m_d3d11Device = 0;
	ID3D11DeviceContext* m_d3d11DevCon = 0;
	ID3D11DepthStencilState* m_depthStencilStateEnabled = 0;
	ID3D11DepthStencilState* m_depthStencilStateDisabled = 0;
	ID3D11RasterizerState* m_RasterizerSolid = 0;
	ID3D11RasterizerState* m_RasterizerSolidNoBackFaceCulling = 0;
	ID3D11RasterizerState* m_RasterizerWireframeNoBackFaceCulling = 0;
	ID3D11RasterizerState* m_RasterizerWireframe = 0;
	ID3D11BlendState* m_blendStateAlphaEnabled = 0;
	ID3D11BlendState* m_blendStateAlphaDisabled = 0;

	alMatrix_t<float32_t> m_GUIProjMtx;

	// for rtt from window data
	alD3D11Texture* m_currRTT = 0;
	alD3D11Texture* m_GUIRTT = 0;

	bool m_vsync = false;

	alColor m_clearColor;

	alArray<alGSD3D11WindowData*> m_allWindowsData;

	alD3D11Model* m_mainTargetSurface = 0;
	alD3D11Shader_screenQuad* m_shaderScreenQuad = 0;
	alD3D11Texture* m_whiteTexture = 0;

	alD3D11Shader_GUIRectangle* m_shaderGUIRectangle = 0;
	alD3D11Shader_GUIMesh* m_shaderGUIMesh = 0;

	alD3D11Shader* m_currShader = 0;
	alD3D11Model* m_currMesh = 0;
public:
	alGSD3D11();
	virtual ~alGSD3D11();

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
	virtual void DrawRectangle(const alVec4f& corners,
		const alColor& color, 
		alGSTexture* texture = 0, alVec4f* UVs = 0)override;
	virtual void DrawText(const char32_t* text, uint32_t size, 
		alGUIFont* font, const alVec2f& position, 
		const alColor& color, alVec2f* endPoint)override;

	ID3D11Device* GetD3D11Device() { return m_d3d11Device; }
	ID3D11DeviceContext* GetD3D11DeviceContext() { return m_d3d11DevCon; }
	void UpdateGUIProjection();
};

#endif
