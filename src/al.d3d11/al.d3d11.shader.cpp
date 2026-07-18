#include "al.d3d11.h"
#include <d3dcompiler.h>

extern alGSD3D11* g_d3d11;

void alD3D11Shader_CreateInputLayout(
	const alGSShaderCreationInfo& inf,
	ID3D11InputLayout** il,
	ID3D10Blob* VsBlob)
{
	*il = 0;

	if (inf.m_vertexType != alMeshVertexType::Null)
	{
		D3D11_INPUT_ELEMENT_DESC vertexLayout[8];
		uint32_t vertexLayoutSize = 0;
		/*
		LPCSTR SemanticName;
		UINT SemanticIndex;
		DXGI_FORMAT Format;
		UINT InputSlot;
		UINT AlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION InputSlotClass;
		UINT InstanceDataStepRate;
		*/
		uint32_t abo = 0;
		int ind = 0;
		switch (inf.m_vertexType)
		{
		case alMeshVertexType::GUI:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 8;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case alMeshVertexType::Triangle:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 12;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 8;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 1;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 8;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 12;

			ind++;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 12;

			ind++;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 12;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = abo;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			abo += 16;
			break;
		case alMeshVertexType::AnimatedTriangle:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 72;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 88;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		case alMeshVertexType::Line:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;


			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case alMeshVertexType::AnimatedLine:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;


			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 28;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case alMeshVertexType::Point:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case alMeshVertexType::AnimatedPoint:
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 28;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind++;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		default:
			alLog::PrintError("Unsupportex vertex type\n");
			return;
		}
		vertexLayoutSize = ind + 1;

		auto hr = g_d3d11->GetD3D11Device()->CreateInputLayout(
			vertexLayout,
			vertexLayoutSize,
			VsBlob->GetBufferPointer(),
			VsBlob->GetBufferSize(),
			il);
		if (FAILED(hr))
		{
			alLog::PrintError("Can't create input layout. Error code [%u]\n", hr);
			return;
		}
	}
}

bool alD3D11Shader_CompileShader(
	ID3D10Blob** errorBlob, 
	ID3D10Blob** shaderBlob,
	const char* shaderFile_,
	const char* entry_,
	const char* target_)
{
	bool ready = false;
	alFileBuffer fb;

	void* inShaderData = 0;
	uint32_t inShaderDataSize = 0;
	
	if (fb.ReadFile(shaderFile_))
	{
		inShaderData = fb.Data();
		inShaderDataSize = fb.Size();
		ready = true;
	}

	if (ready)
	{
		HRESULT hr = D3DCompile(
			inShaderData,
			inShaderDataSize,
			0, 0, 0,
			entry_,
			target_,
			0,
			0,
			shaderBlob,
			errorBlob
		);
		if (FAILED(hr))
			return false;
		return true;
	}

	return false;
}

bool alD3D11Shader_ReadOrCompileShader(
	ID3D10Blob** shaderBlob,
	const char* shaderFile_,
	const char* entry_,
	const char* target_,
	const char* saveShaderToFile_)
{
	ID3D10Blob* errorBlob = nullptr;
	bool ret = false;

	if (saveShaderToFile_)
	{
		alUnicodeString ustr(saveShaderToFile_);
		alStringW strw;
		ustr.ToUTF16(strw);

		if (SUCCEEDED(D3DReadFileToBlob(strw.data(), shaderBlob)))
		{
			ret = true;
		}
	}
	if ((!*shaderBlob) && shaderFile_)
	{
		if (!alD3D11Shader_CompileShader(&errorBlob, 
			shaderBlob,
			shaderFile_,
			entry_,
			target_))
		{
			alLog::PrintError("Failed to compile shader\n");
			if (errorBlob)
			{
				char* message = (char*)errorBlob->GetBufferPointer();
				alLog::PrintError("Error message: %s", message);
				SAFE_RELEASE(errorBlob);
			}
		}
		else
		{
			ret = true;

			if (saveShaderToFile_)
			{
				alUnicodeString ustr(saveShaderToFile_);
				alStringW strw;
				ustr.ToUTF16(strw);

				D3DWriteBlobToFile(*shaderBlob, strw.data(), TRUE);
			}
		}
	}
	return ret;
}

alD3D11Shader::alD3D11Shader()
{
}

alD3D11Shader::~alD3D11Shader()
{
	for (size_t i = 0; i < m_constantBuffers.m_size; ++i)
	{
		alDestroy(m_constantBuffers.m_data[i]);
	}

	SAFE_RELEASE(m_gShader);
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_vShader);
	SAFE_RELEASE(m_vLayout);
}

bool alD3D11Shader::Create(const alGSShaderCreationInfo& inf)
{
	m_info = inf;

	ID3D10Blob* VsBlob = nullptr;
	ID3D10Blob* PsBlob = nullptr;
	ID3D10Blob* GsBlob = nullptr;

	bool ret = true;

	if (!alD3D11Shader_ReadOrCompileShader(
		&VsBlob,
		inf.m_shaderFile_VS,
		inf.m_shaderEntry_VS,
		inf.m_shaderModel_VS,
		inf.m_saveShaderToFile_VS))
	{
		alLog::PrintError("Failed to compile vertex shader\n");
		ret = false;
	}
	else
	{
		auto hr = g_d3d11->GetD3D11Device()->CreateVertexShader(
			VsBlob->GetBufferPointer(),
			VsBlob->GetBufferSize(),
			0,
			&m_vShader);
		if (FAILED(hr))
		{
			alLog::PrintError("Failed to create vertex shader. Error code [%u]\n", hr);
			ret = false;
		}
		else
		{
			alD3D11Shader_CreateInputLayout(inf, &m_vLayout, VsBlob);
			if (!m_vLayout)
			{
				ret = false;
			}
		}
	}
	
	if(!alD3D11Shader_ReadOrCompileShader(
		&PsBlob,
		inf.m_shaderFile_PS,
		inf.m_shaderEntry_PS,
		inf.m_shaderModel_PS,
		inf.m_saveShaderToFile_PS))
	{
		alLog::PrintError("Failed to compile pixel shader\n");
		ret = false;
	}
	else
	{
		auto hr = g_d3d11->GetD3D11Device()->CreatePixelShader(
			PsBlob->GetBufferPointer(),
			PsBlob->GetBufferSize(),
			0,
			&m_pShader);
		if (FAILED(hr))
		{
			alLog::PrintError("Failed to create pixel shader. Error code [%u]\n", hr);
			ret = false;
		}
	}

	if (inf.m_shaderFile_GS)
	{
		if (!alD3D11Shader_ReadOrCompileShader(
			&GsBlob,
			inf.m_shaderFile_GS,
			inf.m_shaderEntry_GS,
			inf.m_shaderModel_GS,
			inf.m_saveShaderToFile_GS))
		{
			alLog::PrintError("Failed to compile geometry shader\n");
			ret = false;
		}
		else
		{
			auto hr = g_d3d11->GetD3D11Device()->CreateGeometryShader(
				GsBlob->GetBufferPointer(),
				GsBlob->GetBufferSize(),
				0,
				&m_gShader);
			if (FAILED(hr))
			{
				alLog::PrintError("Failed to create geometry shader. Error code [%u]\n", hr);
				ret = false;
			}
		}
	}

	if (!ret)
	{
		SAFE_RELEASE(VsBlob);
		SAFE_RELEASE(PsBlob);
		SAFE_RELEASE(GsBlob);
	}

	return ret;
}

alGSShaderConstantBuffer* alD3D11Shader::CreateConstantBuffer(uint32_t size)
{
	alGSShaderConstantBuffer* newCB = 0;

	D3D11_BUFFER_DESC mbd;
	memset(&mbd, 0, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = size;
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	ID3D11Buffer* cb = 0;
	HRESULT hr = g_d3d11->GetD3D11Device()->CreateBuffer(&mbd, 0, &cb);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't create constant buffer. Error code [%u]\n", hr);
	}
	else
	{
		alD3D11GSShaderConstantBuffer* _cb = alCreate<alD3D11GSShaderConstantBuffer>();
		newCB = dynamic_cast<alGSShaderConstantBuffer*>(_cb);
		_cb->m_cb = cb;
		m_constantBuffers.push_back(_cb);
	}

	return newCB;
}

void alD3D11Shader::SetTexture(alGSTexture* texture, uint32_t slot)
{
	alD3D11Texture* t = dynamic_cast<alD3D11Texture*>(texture);
	g_d3d11->GetD3D11DeviceContext()->PSSetShaderResources(slot, 1, &t->m_textureResView);
	g_d3d11->GetD3D11DeviceContext()->PSSetSamplers(slot, 1, &t->m_samplerState);
}

// ====================================================
// 
//                        alD3D11GSShaderConstantBuffer
// 
// ====================================================

alD3D11GSShaderConstantBuffer::alD3D11GSShaderConstantBuffer()
{
}

alD3D11GSShaderConstantBuffer::~alD3D11GSShaderConstantBuffer()
{
	SAFE_RELEASE(m_cb);
}

void alD3D11GSShaderConstantBuffer::MapData(void* data, uint32_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	g_d3d11->GetD3D11DeviceContext()->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, data, d.ByteWidth);
	g_d3d11->GetD3D11DeviceContext()->Unmap(m_cb, 0);
}

void alD3D11GSShaderConstantBuffer::VSSetConstantBuffers(uint32_t slot)
{
	g_d3d11->GetD3D11DeviceContext()->VSSetConstantBuffers(slot, 1, &m_cb);
}

void alD3D11GSShaderConstantBuffer::PSSetConstantBuffers(uint32_t slot)
{
	g_d3d11->GetD3D11DeviceContext()->PSSetConstantBuffers(slot, 1, &m_cb);
}

void alD3D11GSShaderConstantBuffer::GSSetConstantBuffers(uint32_t slot)
{
	g_d3d11->GetD3D11DeviceContext()->GSSetConstantBuffers(slot, 1, &m_cb);
}

