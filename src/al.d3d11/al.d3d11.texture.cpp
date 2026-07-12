#include "al.d3d11.h"
#include "Image/alImage.h"

extern alGSD3D11* g_d3d11;


HRESULT	D3D11Texture_createSamplerState(D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressMode,
	uint32_t anisotropic_level,
	ID3D11SamplerState** samplerState,
	D3D11_COMPARISON_FUNC cmpFunc)
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = 0.0f;

	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;

	samplerDesc.ComparisonFunc = cmpFunc; //D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.MaxAnisotropy = anisotropic_level;

	return g_d3d11->GetD3D11Device()->CreateSamplerState(&samplerDesc, samplerState);
}

alD3D11Texture::alD3D11Texture()
{
}

alD3D11Texture::~alD3D11Texture()
{
	SAFE_RELEASE(m_RTV);
	SAFE_RELEASE(m_depthStencilBuffer);
	SAFE_RELEASE(m_depthStencilView);

	SAFE_RELEASE(m_samplerState);
	SAFE_RELEASE(m_textureResView);
	SAFE_RELEASE(m_texture);
//	Unload();
}

void alD3D11Texture::OnCreate()
{
	if (m_texture)
		return;

	D3D11_FILTER filter;
	alGSTextureFilter tf = m_textureInfo.m_filter;
	switch (tf)
	{
	case alGSTextureFilter::PPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case alGSTextureFilter::PPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case alGSTextureFilter::PLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case alGSTextureFilter::PLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case alGSTextureFilter::LPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case alGSTextureFilter::LPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case alGSTextureFilter::LLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case alGSTextureFilter::LLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case alGSTextureFilter::ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		break;
	case alGSTextureFilter::CMP_PPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		break;
	case alGSTextureFilter::CMP_PPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case alGSTextureFilter::CMP_PLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case alGSTextureFilter::CMP_PLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case alGSTextureFilter::CMP_LPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case alGSTextureFilter::CMP_LPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case alGSTextureFilter::CMP_LLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case alGSTextureFilter::CMP_LLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	case alGSTextureFilter::CMP_ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_ANISOTROPIC;
		break;
	default:
		break;
	}

	if (m_textureInfo.m_textureType == alGSTextureType::Texture2D)
	{
		alImage* img = (alImage*)m_textureInfo.m_imagePtr;

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_textureInfo.m_width;
		desc.Height = m_textureInfo.m_height;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		bool isGenMips = m_textureInfo.IsUseMipMaps();

		switch (img->m_format)
		{

		case alImageFormat::R8G8B8A8:
		{
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			desc.MiscFlags = 0;
			desc.MipLevels = 1;

			desc.ArraySize = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;

			if (isGenMips)
			{
				desc.MipLevels = 0;
				desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
				auto hr = g_d3d11->GetD3D11Device()->CreateTexture2D(&desc, 0, &m_texture);
				if (FAILED(hr))
				{
					alLog::PrintError("Can't create 2D texture\n");
					return;
				}
				g_d3d11->GetD3D11DeviceContext()->UpdateSubresource(m_texture, 0, NULL, img->m_data,
					img->m_pitch, 0);
			}
			else
			{
				D3D11_SUBRESOURCE_DATA initData;
				ZeroMemory(&initData, sizeof(initData));
				initData.pSysMem = img->m_data;
				initData.SysMemPitch = img->m_pitch;
				initData.SysMemSlicePitch = img->m_dataSize;
				auto hr = g_d3d11->GetD3D11Device()->CreateTexture2D(&desc, &initData, &m_texture);
				if (FAILED(hr))
				{
					alLog::PrintError("Can't create 2D texture\n");
					return;
				}
			}


			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(SRVDesc));
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			SRVDesc.Texture2D.MipLevels = 1;
			if (isGenMips)
				SRVDesc.Texture2D.MipLevels = -1;

			auto hr = g_d3d11->GetD3D11Device()->CreateShaderResourceView(m_texture,
				&SRVDesc, &m_textureResView);
			if (FAILED(hr))
			{
				alLog::PrintError("Can't create shader resource view\n");
				return;
			}
		}break;
		default:
			alLog::PrintError("Unsupported texture format\n");
			return;
		}
		if (isGenMips)
			g_d3d11->GetD3D11DeviceContext()->GenerateMips(m_textureResView);
	}
	else
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_textureInfo.m_width;
		desc.Height = m_textureInfo.m_height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = 0;
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		auto hr = g_d3d11->GetD3D11Device()->CreateTexture2D(&desc, NULL, &m_texture);
		if (FAILED(hr))
		{
			alLog::PrintError("Can't create render target texture\n");
			return;
		}
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = desc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		hr = g_d3d11->GetD3D11Device()->CreateRenderTargetView(m_texture, &renderTargetViewDesc, &m_RTV);
		if (FAILED(hr))
		{
			alLog::PrintError("Can't create render target view\n");
			return;
		}



		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = g_d3d11->GetD3D11Device()->CreateShaderResourceView(m_texture,
			&SRVDesc, &m_textureResView);
		if (FAILED(hr))
		{
			alLog::PrintError("Can't create shader resource view\n");
			return;
		}

		//================
		D3D11_TEXTURE2D_DESC	DSD;
		ZeroMemory(&DSD, sizeof(DSD));
		DSD.Width = m_textureInfo.m_width;
		DSD.Height = m_textureInfo.m_width;
		DSD.MipLevels = 1;
		DSD.ArraySize = 1;
		DSD.Format = DXGI_FORMAT_D32_FLOAT;
		DSD.SampleDesc.Count = 1;
		DSD.SampleDesc.Quality = 0;
		DSD.Usage = D3D11_USAGE_DEFAULT;
		DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		DSD.CPUAccessFlags = 0;
		DSD.MiscFlags = 0;
		if (FAILED(g_d3d11->GetD3D11Device()->CreateTexture2D(&DSD,
			0, &m_depthStencilBuffer)))
		{
			alLog::PrintError("Can't create Direct3D 11 depth stencil buffer\n");
			return;
		}
		D3D11_DEPTH_STENCIL_VIEW_DESC	depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		if (FAILED(g_d3d11->GetD3D11Device()->CreateDepthStencilView(
			m_depthStencilBuffer,
			&depthStencilViewDesc,
			&m_depthStencilView)))
		{
			alLog::PrintError("Can't create Direct3D 11 depth stencil view\n");
			return;
		}

		return;
	}

	D3D11_TEXTURE_ADDRESS_MODE tam;
	switch (m_textureInfo.m_addressMode)
	{
	case alGSTextureAddressMode::Wrap:
	default:
		tam = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case alGSTextureAddressMode::Mirror:
		tam = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	case alGSTextureAddressMode::Clamp:
		tam = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case alGSTextureAddressMode::Border:
		tam = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	case alGSTextureAddressMode::MirrorOnce:
		tam = D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		break;
	}

	D3D11_COMPARISON_FUNC cmpFunc;
	switch (m_textureInfo.m_comparisonFunc)
	{
	case alGSTextureComparisonFunc::Never:
		cmpFunc = D3D11_COMPARISON_NEVER;
		break;
	case alGSTextureComparisonFunc::Less:
		cmpFunc = D3D11_COMPARISON_LESS;
		break;
	case alGSTextureComparisonFunc::Equal:
		cmpFunc = D3D11_COMPARISON_EQUAL;
		break;
	case alGSTextureComparisonFunc::LessEqual:
		cmpFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;
	case alGSTextureComparisonFunc::Greater:
		cmpFunc = D3D11_COMPARISON_GREATER;
		break;
	case alGSTextureComparisonFunc::NotEqual:
		cmpFunc = D3D11_COMPARISON_NOT_EQUAL;
		break;
	case alGSTextureComparisonFunc::GreaterEqual:
		cmpFunc = D3D11_COMPARISON_GREATER_EQUAL;
		break;
	case alGSTextureComparisonFunc::Always:
	default:
		cmpFunc = D3D11_COMPARISON_ALWAYS;
		break;
	}


	auto hr = D3D11Texture_createSamplerState(filter, tam, m_textureInfo.m_anisotropicLevel,
		&m_samplerState, cmpFunc);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't create sampler state\n");
		return;
	}

//	m_loaded = true;
}

//void alD3D11Texture::Unload()
//{
//	SAFE_RELEASE(m_RTV);
//	SAFE_RELEASE(m_depthStencilBuffer);
//	SAFE_RELEASE(m_depthStencilView);
//
//	SAFE_RELEASE(m_samplerState);
//	SAFE_RELEASE(m_textureResView);
//	SAFE_RELEASE(m_texture);
//
//	m_loaded = false;
//}
//
