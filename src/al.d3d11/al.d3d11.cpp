#include "al.d3d11.h"
#include "System/alSystemWindow.h"
#include "System/alSystemWindowWin32.h"
#include "Image/alImage.h"
#include "GUI/alGUI.h"

#ifdef DrawText
#undef DrawText
#endif

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

alGSD3D11* g_d3d11 = 0;

alGS* alCreateGS_d3d11()
{
	return alCreate<alGSD3D11>();
}

alGSD3D11::alGSD3D11()
{
	g_d3d11 = this;
}

alGSD3D11::~alGSD3D11()
{
	AL_DESTROY(m_shaderScreenQuad);
	AL_DESTROY(m_shaderGUIRectangle);
	AL_DESTROY(m_shaderGUIMesh);
	AL_DESTROY(m_GUIRTT);

	for (size_t i = 0; i < m_allWindowsData.m_size; ++i)
	{
		SAFE_RELEASE(m_allWindowsData.m_data[i]->m_mainTarget.m_depthStencilBuffer);
		SAFE_RELEASE(m_allWindowsData.m_data[i]->m_mainTarget.m_depthStencilView);
		SAFE_RELEASE(m_allWindowsData.m_data[i]->m_mainTarget.m_targetView);
		AL_DESTROY(m_allWindowsData.m_data[i]->m_rtt);
		SAFE_RELEASE(m_allWindowsData.m_data[i]->m_swapChain);
	}
	/*SAFE_RELEASE(m_mainTarget.m_depthStencilView);
	SAFE_RELEASE(m_mainTarget.m_depthStencilBuffer);
	SAFE_RELEASE(m_mainTarget.m_targetView);*/

	AL_DESTROY(m_whiteTexture)

	SAFE_RELEASE(m_blendStateAlphaDisabled);
	SAFE_RELEASE(m_blendStateAlphaEnabled);
	SAFE_RELEASE(m_RasterizerWireframe);
	SAFE_RELEASE(m_RasterizerWireframeNoBackFaceCulling);
	SAFE_RELEASE(m_RasterizerSolidNoBackFaceCulling);
	SAFE_RELEASE(m_RasterizerSolid);
	SAFE_RELEASE(m_depthStencilStateDisabled);
	SAFE_RELEASE(m_depthStencilStateEnabled);
	SAFE_RELEASE(m_d3d11DevCon);
	SAFE_RELEASE(m_d3d11Device);
}

const char* alGSD3D11::GetVideoDriverName()
{
	return "D3D11";
}

bool alGSD3D11::Init(alSystemWindow* sw)
{
	m_activeWindow = sw;
	alGSD3D11WindowData windowGPUData;

	DXGI_MODE_DESC	bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Width = (UINT)m_activeWindow->m_clientSize.x;
	bufferDesc.Height = (UINT)m_activeWindow->m_clientSize.y;
	//if (m_params.m_vSync)
	bufferDesc.RefreshRate.Numerator = 60;
	//else bufferDesc.RefreshRate.Numerator = 0;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_d3d11Device,
		&featureLevel,
		&m_d3d11DevCon);

	if (FAILED(hr))
	{
		alLog::PrintError("Can't create Direct3D 11 Device : code %u\n", hr);
		return false;
	}

	IDXGIDevice* dxgiDevice = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	IDXGIFactory1* dxgiFactory = nullptr;
	hr = m_d3d11Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't QueryInterface : IID_IDXGIDevice, code %u\n", hr);
		return false;
	}

	hr = dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't get DXGI adapter, code %u\n", hr);
		dxgiDevice->Release();
		return false;
	}

	hr = dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't get DXGI factory, code %u\n", hr);
		dxgiDevice->Release();
		dxgiAdapter->Release();
		return false;
	}

	alSystemWindowOSDataWin32* windowData = (alSystemWindowOSDataWin32*)sw->GetOSData();
	DXGI_SWAP_CHAIN_DESC	swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowData->m_hwnd;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true/*m_params.m_fullScreen ? false : true*/;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	hr = dxgiFactory->CreateSwapChain(m_d3d11Device, &swapChainDesc, &windowGPUData.m_swapChain);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't create Swap Chain : code %u\n", hr);
		dxgiDevice->Release();
		dxgiAdapter->Release();
		dxgiFactory->Release();
		return false;
	}

	alGSD3D11WindowData* pwindowGPUData = new alGSD3D11WindowData;
	*pwindowGPUData = windowGPUData;
	sw->m_GPUData = (void*)pwindowGPUData;
	m_allWindowsData.push_back(pwindowGPUData);


	dxgiFactory->MakeWindowAssociation(windowData->m_hwnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStateEnabled)))
	{
		alLog::PrintError("Can't create Direct3D 11 depth stencil state\n");
	}

	m_d3d11DevCon->OMSetDepthStencilState(this->m_depthStencilStateEnabled, 0);

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled)))
	{
		alLog::PrintError("Can't create Direct3D 11 depth stencil state\n");
	}

	D3D11_RASTERIZER_DESC	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolid)))
	{
		alLog::PrintError("Can not create rasterizer state\n");
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling);
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframeNoBackFaceCulling);
	rasterDesc.CullMode = D3D11_CULL_BACK;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframe);

	m_d3d11DevCon->RSSetState(m_RasterizerSolid);


	D3D11_BLEND_DESC  bd;
	memset(&bd, 0, sizeof(bd));
	bd.AlphaToCoverageEnable = 0;
	bd.RenderTarget[0].BlendEnable = TRUE;

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled)))
	{
		alLog::PrintError("Can't create Direct3D 11 blend state\n");
	}
	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);
	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0);

	D3D11_RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = (LONG)sw->m_clientSize.x;
	sr.bottom = (LONG)sw->m_clientSize.y;
	m_d3d11DevCon->RSSetScissorRects(1, &sr);

	D3D11_VIEWPORT viewport;
	viewport.Width = (float32_t)sw->m_clientSize.x;
	viewport.Height = (float32_t)sw->m_clientSize.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3d11DevCon->RSSetViewports(1, &viewport);
	
	//UpdateGUIProjection();
	/*{
		miGPUTextureInfo inf;
		miImage img;
		img.create(1, 1);
		img.fill(ColorWhite);
		inf.m_imagePtr = &img;
		inf.m_width = img.m_width;
		inf.m_height = img.m_height;
		m_whiteTexture = miCreate<miD3D11Texture>();
		*m_whiteTexture->GetTextureInfo() = inf;
		m_whiteTexture->Reload();
	}*/

	UpdateWindowData();

	{
		alImage image;
		image.Create(1, 1);
		image.Fill(ColorWhite);

		alGSTextureInfo ti;
		ti.m_imagePtr = &image;
		ti.m_width = image.m_width;
		ti.m_height = image.m_height;
		m_whiteTexture = dynamic_cast<alD3D11Texture*>(this->CreateTexture(&ti));
	}
	{
		auto mesh = alCreate<alMesh>();
		mesh->m_iCount = 6;
		mesh->m_vCount = 4;
		mesh->m_stride = sizeof(alVertexGUI);
		mesh->m_vertexType = alMeshVertexType::GUI;
		mesh->m_vertices = (uint8_t*)alMemory::Malloc(mesh->m_vCount * mesh->m_stride);
		mesh->m_indices = (uint8_t*)alMemory::Malloc(mesh->m_iCount * sizeof(uint16_t));
		uint16_t* inds = (uint16_t*)mesh->m_indices;

		alVertexGUI* vertex = (alVertexGUI*)mesh->m_vertices;
		vertex->Position.Set(-1.f, 1.f);
		vertex->UV.Set(0.f, 0.f);
		vertex++;
		vertex->Position.Set(-1.f, -1.f);
		vertex->UV.Set(0.f, 1.f);
		vertex++;
		vertex->Position.Set(1.f, -1.f);
		vertex->UV.Set(1.f, 1.f);
		vertex++;
		vertex->Position.Set(1.f, 1.f);
		vertex->UV.Set(1.f, 0.f);
		vertex++;

		inds[0] = 0;
		inds[1] = 1;
		inds[2] = 2;
		inds[3] = 0;
		inds[4] = 2;
		inds[5] = 3;

		m_mainTargetSurface = alCreate<alD3D11Model>();
		m_mainTargetSurface->GetMeshInfo()->m_meshPtr = mesh;
		m_mainTargetSurface->OnCreate();
		alDestroy(mesh);
	}
	{
		m_shaderScreenQuad = alCreate<alD3D11Shader_screenQuad>();
		alGSShaderCreationInfo inf;
		inf.m_callback = m_shaderScreenQuad;
		inf.m_vertexType = alMeshVertexType::GUI;
		inf.m_shaderEntry_VS = "VSMain";
		inf.m_shaderEntry_PS = "PSMain";
		inf.m_shaderEntry_GS = "GSMain";
		inf.m_shaderFile_VS = "../data/shaders/d3d11/ScreenQuad.hlsl";
		inf.m_shaderFile_PS = "../data/shaders/d3d11/ScreenQuad.hlsl";
		inf.m_shaderModel_VS = "vs_4_0";
		inf.m_shaderModel_PS = "ps_4_0";
		inf.m_shaderModel_GS = "gs_4_0";
		m_shaderScreenQuad->m_shader = dynamic_cast<alD3D11Shader*>(CreateShader(inf));
		if (!m_shaderScreenQuad->m_shader)
			return false;
	}
	{
		m_shaderGUIRectangle = alCreate<alD3D11Shader_GUIRectangle>();
		alGSShaderCreationInfo inf;
		inf.m_callback = m_shaderGUIRectangle;
		inf.m_vertexType = alMeshVertexType::Point;
		//inf.m_saveShaderToFile_VS = "../data/shaders/d3d11/GUIRectangle.vs";
		//inf.m_saveShaderToFile_PS = "../data/shaders/d3d11/GUIRectangle.ps";
		//inf.m_saveShaderToFile_GS = "../data/shaders/d3d11/GUIRectangle.gs";
		inf.m_shaderEntry_VS = "VSMain";
		inf.m_shaderEntry_PS = "PSMain";
		inf.m_shaderEntry_GS = "GSMain";
		inf.m_shaderFile_VS = "../data/shaders/d3d11/GUIRectangle.hlsl";
		inf.m_shaderFile_PS = "../data/shaders/d3d11/GUIRectangle.hlsl";
		inf.m_shaderFile_GS = "../data/shaders/d3d11/GUIRectangle.hlsl";
		inf.m_shaderModel_VS = "vs_4_0";
		inf.m_shaderModel_PS = "ps_4_0";
		inf.m_shaderModel_GS = "gs_4_0";
		m_shaderGUIRectangle->m_shader = dynamic_cast<alD3D11Shader*>(CreateShader(inf));
		if (!m_shaderGUIRectangle->m_shader)
			return false;
		m_shaderGUIRectangle->m_cb
			= dynamic_cast<alD3D11GSShaderConstantBuffer*>
			(m_shaderGUIRectangle->m_shader
				->CreateConstantBuffer(
					sizeof(m_shaderGUIRectangle->m_cbVertex_impl)));
	}
	{
		m_shaderGUIMesh = alCreate<alD3D11Shader_GUIMesh>();
		alGSShaderCreationInfo inf;
		inf.m_callback = m_shaderGUIMesh;
		inf.m_vertexType = alMeshVertexType::GUI;
		inf.m_shaderEntry_VS = "VSMain";
		inf.m_shaderEntry_PS = "PSMain";
		inf.m_shaderFile_VS = "../data/shaders/d3d11/GUIMesh.hlsl";
		inf.m_shaderFile_PS = "../data/shaders/d3d11/GUIMesh.hlsl";
		inf.m_shaderModel_VS = "vs_4_0";
		inf.m_shaderModel_PS = "ps_4_0";
		m_shaderGUIMesh->m_shader = dynamic_cast<alD3D11Shader*>(CreateShader(inf));
		if (!m_shaderGUIMesh->m_shader)
			return false;
		m_shaderGUIMesh->m_cbVertex
			= dynamic_cast<alD3D11GSShaderConstantBuffer*>
			(m_shaderScreenQuad->m_shader
				->CreateConstantBuffer(
					sizeof(m_shaderGUIMesh->m_cbVertex_impl)));
		m_shaderGUIMesh->m_cbPixel
			= dynamic_cast<alD3D11GSShaderConstantBuffer*>
			(m_shaderGUIMesh->m_shader
				->CreateConstantBuffer(
					sizeof(m_shaderGUIMesh->m_cbPixel_impl)));
	}

	SetClearColor(ColorMediumSeaGreen);

	return true;
}

bool alGSD3D11::InitWindow(alSystemWindow* sw)
{
	return true;
}

void alGSD3D11::GetDepthRange(alVec2f* d)
{
	AL_ASSERT_ST(d);
	d->x = 0.f;
	d->y = 1.f;
}

void alGSD3D11::SetClearColor(const alColor& c)
{
	m_clearColor = c;
}

void alGSD3D11::BeginDraw()
{
	alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	m_currRTT = dynamic_cast<alD3D11Texture*>(data->m_rtt);

	//alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	// nothing to do
	m_d3d11DevCon->OMSetRenderTargets(1, &m_currRTT->m_RTV, m_currRTT->m_depthStencilView);
	SetViewport(0, 0, m_currRTT->GetGSTextureInfo()->m_width, m_currRTT->GetGSTextureInfo()->m_height);
}

void alGSD3D11::ClearDepth()
{
	m_d3d11DevCon->ClearDepthStencilView(m_currRTT->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void alGSD3D11::ClearColor()
{
	m_d3d11DevCon->ClearRenderTargetView(m_currRTT->m_RTV, m_clearColor.Data());
}

void alGSD3D11::ClearAll()
{
	alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	m_d3d11DevCon->ClearDepthStencilView(m_currRTT->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_d3d11DevCon->ClearRenderTargetView(m_currRTT->m_RTV, m_clearColor.Data());
}

void alGSD3D11::Draw()
{
	AL_ASSERT_ST(m_currMesh);
	AL_ASSERT_ST(m_currShader);
	m_currShader->m_info.m_callback->OnSetConstants();

	uint32_t offset = 0;
	m_d3d11DevCon->IASetVertexBuffers(0, 1, &m_currMesh->m_vBuffer, &m_currMesh->m_stride, &offset);
	switch (m_currMesh->m_vertexType)
	{
	default:
		m_d3d11DevCon->IASetIndexBuffer(m_currMesh->m_iBuffer, m_currMesh->m_indexType, 0);
		m_d3d11DevCon->DrawIndexed(m_currMesh->m_iCount, 0, 0);
		break;
	case alMeshVertexType::AnimatedPoint:
	case alMeshVertexType::Point:
		m_d3d11DevCon->Draw(m_currMesh->m_iCount, 0);
		break;
	}
}

void alGSD3D11::DrawMeshGUI(alGSMesh* mesh, const alVec2f& offset, const alColor& color)
{
	auto oldShader = m_currShader;
	AL_ASSERT_ST(mesh);
	SetShader(m_shaderGUIMesh->m_shader);
	m_shaderGUIMesh->m_cbVertex_impl.m_Offset = offset;
	m_shaderGUIMesh->m_cbVertex->MapData(&m_shaderGUIMesh->m_cbVertex_impl, sizeof(m_shaderGUIMesh->m_cbVertex_impl));

	m_shaderGUIMesh->m_cbPixel_impl.m_Color = color;
	m_shaderGUIMesh->m_cbPixel->MapData(&m_shaderGUIMesh->m_cbPixel_impl, sizeof(m_shaderGUIMesh->m_cbPixel_impl));

	SetMesh(mesh);

	uint32_t _offset = 0u;
	D3D11_PRIMITIVE_TOPOLOGY oldTopology;
	m_d3d11DevCon->IAGetPrimitiveTopology(&oldTopology);

	m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DevCon->IASetVertexBuffers(0, 1, &m_currMesh->m_vBuffer, &m_currMesh->m_stride, &_offset);
	m_d3d11DevCon->IASetIndexBuffer(m_currMesh->m_iBuffer, m_currMesh->m_indexType, 0);
	m_d3d11DevCon->DrawIndexed(m_currMesh->m_iCount, 0, 0);
	
	SetShader(oldShader);
	m_d3d11DevCon->IASetPrimitiveTopology(oldTopology);
}

void alGSD3D11::EndDraw()
{
	alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	
	m_d3d11DevCon->OMSetRenderTargets(1, &data->m_mainTarget.m_targetView, data->m_mainTarget.m_depthStencilView);
	m_d3d11DevCon->ClearDepthStencilView(data->m_mainTarget.m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_d3d11DevCon->ClearRenderTargetView(data->m_mainTarget.m_targetView, m_clearColor.Data());

	SetViewport(0, 0, m_activeWindow->m_clientSize.x, m_activeWindow->m_clientSize.y);
	SetScissorRect(alVec4f(0, 0, (float32_t)m_activeWindow->m_clientSize.x, (float32_t)m_activeWindow->m_clientSize.y));
	
	m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DisableDepth();
	m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

	SetShader(m_shaderScreenQuad->m_shader);

	m_currRTT = dynamic_cast<alD3D11Texture*>(data->m_rtt);
	m_shaderScreenQuad->m_shader->SetTexture(m_currRTT, 0);
	m_shaderScreenQuad->m_shader->SetTexture(m_GUIRTT, 1);

	uint32_t offset = 0;
	m_d3d11DevCon->IASetVertexBuffers(0, 1, &m_mainTargetSurface->m_vBuffer, &m_mainTargetSurface->m_stride, &offset);
	m_d3d11DevCon->IASetIndexBuffer(m_mainTargetSurface->m_iBuffer, m_mainTargetSurface->m_indexType, 0);
	m_d3d11DevCon->DrawIndexed(m_mainTargetSurface->m_iCount, 0, 0);
}

void alGSD3D11::SwapBuffers()
{
	alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	m_vsync ? data->m_swapChain->Present(1, 0)
		: data->m_swapChain->Present(0, 0);
}

// Function can be devided on 2 parts
// 1. Resize RTT
// 2. Resize swapchain buffer
void alGSD3D11::UpdateWindowData()
{
	alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
	data->m_rttSize = m_activeWindow->GetWindowCallback()->OnGPUUpdateSize(m_activeWindow);

	AL_DESTROY(data->m_rtt);
	{
		alGSTextureInfo ti;
		ti.m_width = data->m_rttSize.x;
		ti.m_height = data->m_rttSize.y;
		ti.m_textureType = alGSTextureType::RTT;
		data->m_rtt = (alD3D11Texture*)this->CreateTexture(&ti);
	}
	
	m_d3d11DevCon->OMSetRenderTargets(0, 0, 0);

	SAFE_RELEASE(data->m_mainTarget.m_depthStencilBuffer);
	SAFE_RELEASE(data->m_mainTarget.m_depthStencilView);
	SAFE_RELEASE(data->m_mainTarget.m_targetView);

	AL_DESTROY(m_GUIRTT);
	{
		alGSTextureInfo ti;
		ti.m_width = m_activeWindow->m_clientSize.x;
		ti.m_height = m_activeWindow->m_clientSize.y;
		ti.m_textureType = alGSTextureType::RTT;
		m_GUIRTT = (alD3D11Texture*)this->CreateTexture(&ti);
	}
	

	data->m_swapChain->ResizeBuffers(0, 
		(UINT)m_activeWindow->m_clientSize.x, (UINT)m_activeWindow->m_clientSize.y,
		DXGI_FORMAT_UNKNOWN, 0);

	ID3D11Texture2D* BackBuffer = 0;
	if (FAILED(data->m_swapChain->GetBuffer(
		0,
		IID_ID3D11Texture2D,
		(void**)&BackBuffer)))
	{
		alLog::PrintError("Can't create Direct3D 11 back buffer\n");
		return;
	}
	if (FAILED(this->m_d3d11Device->CreateRenderTargetView(
		BackBuffer, 0, &data->m_mainTarget.m_targetView)))
	{
		alLog::PrintError("Can't create Direct3D 11 render target\n");
		if (BackBuffer) 
			BackBuffer->Release();
		return;
	}

	if (BackBuffer) 
		BackBuffer->Release();


	D3D11_TEXTURE2D_DESC	DSD;
	ZeroMemory(&DSD, sizeof(DSD));
	DSD.Width = m_activeWindow->m_clientSize.x;
	DSD.Height = m_activeWindow->m_clientSize.y;
	DSD.MipLevels = 1;
	DSD.ArraySize = 1;
	DSD.Format = DXGI_FORMAT_D32_FLOAT;
	DSD.SampleDesc.Count = 1;
	DSD.SampleDesc.Quality = 0;
	DSD.Usage = D3D11_USAGE_DEFAULT;
	DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSD.CPUAccessFlags = 0;
	DSD.MiscFlags = 0;
	if (FAILED(m_d3d11Device->CreateTexture2D(&DSD,
		0, &data->m_mainTarget.m_depthStencilBuffer)))
	{
		alLog::PrintError("Can't create Direct3D 11 depth stencil buffer\n");
		return;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC	depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_d3d11Device->CreateDepthStencilView(
		data->m_mainTarget.m_depthStencilBuffer,
		&depthStencilViewDesc, 
		&data->m_mainTarget.m_depthStencilView)))
	{
		alLog::PrintError("Can't create Direct3D 11 depth stencil view\n");
		return;
	}
	m_d3d11DevCon->OMSetRenderTargets(1, 
		&data->m_mainTarget.m_targetView,
		data->m_mainTarget.m_depthStencilView);

	UpdateGUIProjection();

}

alGSTexture* alGSD3D11::CreateTexture(alGSTextureInfo* ti)
{
	auto t = alCreate<alD3D11Texture>();
	*t->GetGSTextureInfo() = *ti;

	if (ti->m_imagePtr)
	{
		// Not need to set this values by yourself anymore
		t->GetGSTextureInfo()->m_height = ti->m_imagePtr->m_height;
		t->GetGSTextureInfo()->m_width = ti->m_imagePtr->m_width;
	}

	t->OnCreate();
	//t->Reload();
	return t;
}

alGSMesh* alGSD3D11::CreateMesh(alGSMeshInfo* mi)
{
	auto m = alCreate<alD3D11Model>();
	*m->GetMeshInfo() = *mi;
	m->OnCreate();
	return m;
}

void alGSD3D11::SetRenderTarget(alGSTexture* t)
{
	if (t)
	{
		m_currRTT = dynamic_cast<alD3D11Texture*>(t);
	}
	else
	{
		alGSD3D11WindowData* data = (alGSD3D11WindowData*)m_activeWindow->m_GPUData;
		m_currRTT = data->m_rtt;
	}
	m_d3d11DevCon->OMSetRenderTargets(1, &m_currRTT->m_RTV, m_currRTT->m_depthStencilView);
}

void alGSD3D11::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	m_d3d11DevCon->RSSetViewports(1, &viewport);
}

void alGSD3D11::SetScissorRect(const alVec4f& rect)
{
	D3D11_RECT r;
	r.left = (LONG)rect.x;
	r.top = (LONG)rect.y;
	r.right = (LONG)rect.z;
	r.bottom = (LONG)rect.w;
	m_d3d11DevCon->RSSetScissorRects(1, &r);
}

void alGSD3D11::EnableVSync()
{
	m_vsync = true;
}

void alGSD3D11::DisableVSync()
{
	m_vsync = false;
}

void alGSD3D11::EnableDepth()
{
	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateEnabled, 0);
}

void alGSD3D11::DisableDepth()
{
	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);
}

alGSShader* alGSD3D11::CreateShader(const alGSShaderCreationInfo& inf)
{
	alD3D11Shader* s = alCreate<alD3D11Shader>();
	if (!s->Create(inf))
		AL_DESTROY(s);
	return s;
}

void alGSD3D11::UpdateGUIProjection()
{
	//alMat4* m = alLib::GetMatrix(alMatrixType::GUIProjection);
	float32_t L = 0;
	float32_t R = (float32_t)m_activeWindow->m_clientSize.x;
	float32_t T = 0;
	float32_t B = (float32_t)m_activeWindow->m_clientSize.y;

	m_GUIProjMtx.m_data[0] = alVec4f(2.0f / (R - L), 0.0f, 0.0f, 0.0f);
	m_GUIProjMtx.m_data[1] = alVec4f(0.0, 2.0f / (T - B), 0.0f, 0.0f);
	m_GUIProjMtx.m_data[2] = alVec4f(0.0, 0.0, 0.5f, 0.0f);
	m_GUIProjMtx.m_data[3] = alVec4f((R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);
}

void alGSD3D11::SetShader(alGSShader* s)
{
	alD3D11Shader* shader = dynamic_cast<alD3D11Shader*>(s);
		m_currShader = shader;
		m_d3d11DevCon->IASetInputLayout(shader->m_vLayout);
		m_d3d11DevCon->VSSetShader(shader->m_vShader, 0, 0);
		m_d3d11DevCon->GSSetShader(shader->m_gShader, 0, 0);
		m_d3d11DevCon->PSSetShader(shader->m_pShader, 0, 0);

		m_currShader->m_info.m_callback->OnSetShader();
}

alGSTexture* alGSD3D11::GetWhiteTexture()
{
	return m_whiteTexture;
}

void alGSD3D11::SetMesh(alGSMesh* gsmesh)
{
	m_currMesh = dynamic_cast<alD3D11Model*>(gsmesh);
}

void alGSD3D11::SetRasterizationType(alGSRasterizationType rt)
{
	switch (rt)
	{
	case alGSRasterizationType::Wireframe:
		m_d3d11DevCon->RSSetState(m_RasterizerWireframe);
		break;
	case alGSRasterizationType::WireframeNoCull:
		m_d3d11DevCon->RSSetState(m_RasterizerWireframeNoBackFaceCulling);
		break;
	default:
	case alGSRasterizationType::Solid:
		m_d3d11DevCon->RSSetState(m_RasterizerSolid);
		break;
	case alGSRasterizationType::SolidNoCull:
		m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);
		break;
	}
}

void alGSD3D11::SetPrimitiveType(alGSPrimitiveType t)
{
	switch (t)
	{
	case alGSPrimitiveType::Triangle:
	default:
		m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case alGSPrimitiveType::Line:
		m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		break;
	case alGSPrimitiveType::Point:
		m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	}
}

void alGSD3D11::DrawRectangle(const alVec4f& corners,
	const alColor& color1, const alColor& color2,
	alGSTexture* texture, alVec4f* UVs)
{
	//alMat4* m = alLib::GetMatrix(alMatrixType::GUIProjection);
	/*if (m_currShader != m_shaderGUIRectangle->m_shader)
	{
		m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
		m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);
		m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);
		SetShader(m_shaderGUIRectangle->m_shader);
	}*/

	m_shaderGUIRectangle->m_cbVertex_impl.m_Corners = corners;
	m_shaderGUIRectangle->m_cbVertex_impl.m_Color1 = color1;
	m_shaderGUIRectangle->m_cbVertex_impl.m_Color2 = color2;
	m_shaderGUIRectangle->m_cbVertex_impl.m_UVs.Set(0.f, 0.f, 1.f, 1.f);
	if (UVs)
		m_shaderGUIRectangle->m_cbVertex_impl.m_UVs = *UVs;

	//SetShader(m_shaderGUIRectangle->m_shader);

	//Do not delete these comments
	// this is an example how to set constant buffers
	//m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_shaderGUIRectangle->m_cbVertex);
	//m_d3d11DevCon->GSSetConstantBuffers(0, 1, &m_shaderGUIRectangle->m_cbVertex);
	
	m_shaderGUIRectangle->m_cb->MapData(&m_shaderGUIRectangle->m_cbVertex_impl, sizeof(m_shaderGUIRectangle->m_cbVertex_impl));
	//m_shaderGUIRectangle->OnSetConstants();
	if (!texture)
		texture = m_whiteTexture;
	m_d3d11DevCon->PSSetShaderResources(0, 1, &((alD3D11Texture*)texture)->m_textureResView);
	m_d3d11DevCon->PSSetSamplers(0, 1, &((alD3D11Texture*)texture)->m_samplerState);
	//m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);
	m_d3d11DevCon->Draw(1, 0);
}

void alGSD3D11::DrawRectangle(const alVec4f& corners,
	const alColor& color,
	alGSTexture* texture, alVec4f* UVs)
{
	DrawRectangle(corners, color,color,texture,UVs);
}

void alGSD3D11::BeginDrawGUI(bool clear)
{
	m_shaderGUIRectangle->m_cbVertex_impl.m_ProjMtx = m_GUIProjMtx;
	m_shaderGUIMesh->m_cbVertex_impl.m_ProjMtx = m_GUIProjMtx;
	const float clearColor[4] = { 0.f,0.f,0.f,0.f };
	SetRenderTarget(m_GUIRTT);
	if(clear)
		m_d3d11DevCon->ClearRenderTargetView(m_GUIRTT->m_RTV, clearColor);

	SetViewport(0, 0, m_activeWindow->m_clientSize.x, m_activeWindow->m_clientSize.y);
	SetScissorRect(alVec4f(0, 0, (float32_t)m_activeWindow->m_clientSize.x, (float32_t)m_activeWindow->m_clientSize.y));
	m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	m_d3d11DevCon->OMSetDepthStencilState(m_depthStencilStateDisabled, 0);
	m_d3d11DevCon->RSSetState(m_RasterizerSolidNoBackFaceCulling);

	SetShader(m_shaderGUIRectangle->m_shader);
}

void alGSD3D11::EndDrawGUI()
{
}

void alGSD3D11::DrawText(const char32_t* text, 
	uint32_t size, 
	alGUIFont* font, 
	const alVec2f& position, 
	const alColor& color, 
	alVec2f* endPoint)
{
	alVec2f _position = position;
	alVec2f _ep;

	alVec2f uv1, uv2;
	for (uint32_t i = 0; i < size; ++i)
	{
		char32_t character = text[i];
		auto glyph = font->GetGlyph(character);
		if (!glyph)
		{
			glyph = font->GetUnknownGlyph();
			if (!glyph)
				continue;
		}

		alVec4f corners;

		corners.x = _position.x;
		corners.y = _position.y;

		corners.z = corners.x + glyph->width;
		corners.w = corners.y + glyph->height;

		_ep.x = corners.z;
		_ep.y = corners.y;

		m_shaderGUIRectangle->m_cbVertex_impl.m_Corners = corners;
		m_shaderGUIRectangle->m_cbVertex_impl.m_Color1 = color;
		m_shaderGUIRectangle->m_cbVertex_impl.m_Color2 = color;

		auto texture = font->GetTexture(glyph->textureID);
		if (texture)
		{
			alVec4f _UVs(glyph->lt.x, glyph->lt.y, glyph->rb.x, glyph->rb.y);
			m_shaderGUIRectangle->m_cbVertex_impl.m_UVs = _UVs;

			m_d3d11DevCon->PSSetShaderResources(0, 1, &((alD3D11Texture*)texture)->m_textureResView);
			m_d3d11DevCon->PSSetSamplers(0, 1, &((alD3D11Texture*)texture)->m_samplerState);

			m_shaderGUIRectangle->m_cb->MapData(&m_shaderGUIRectangle->m_cbVertex_impl, sizeof(m_shaderGUIRectangle->m_cbVertex_impl));
			m_d3d11DevCon->Draw(1, 0);

			//_position.x += glyph->underhang;
			_position.x += glyph->width - glyph->overhang + font->m_spacing;
			if (character == L' ')
				_position.x += font->m_spaceWidth;
			if (character == L'\t')
				_position.x += font->m_tabWidth;
		}
	}

	if (endPoint)
	{
		*endPoint = _ep;
	}
}
