#include "al.d3d11.h"

extern alGSD3D11* g_d3d11;

alD3D11Model::alD3D11Model()
{
}

alD3D11Model::~alD3D11Model()
{
	SAFE_RELEASE(m_vBuffer);
	SAFE_RELEASE(m_iBuffer);
}

void alD3D11Model::OnCreate()
{
	if (m_vBuffer)
		return;

	alMesh* mesh = (alMesh*)m_meshInfo.m_meshPtr;
	if (!mesh->m_vertices)
		return;

	m_vertexType = mesh->m_vertexType;

	D3D11_BUFFER_DESC	vbd, ibd;

	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));

	vbd.Usage = D3D11_USAGE_DEFAULT;
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	ibd.Usage = D3D11_USAGE_DEFAULT;
	//ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//vbd.CPUAccessFlags = 0;
	//ibd.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE

	D3D11_SUBRESOURCE_DATA	vData, iData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	HRESULT	hr = 0;


	vbd.ByteWidth = mesh->m_stride * mesh->m_vCount;
	vData.pSysMem = &mesh->m_vertices[0];

	hr = g_d3d11->GetD3D11Device()->CreateBuffer(&vbd, &vData, &m_vBuffer);
	if (FAILED(hr))
	{
		alLog::PrintError("Can't create Direct3D 11 vertex buffer [%u]\n", hr);
		return;
	}

	m_stride = mesh->m_stride;
	m_iCount = mesh->m_iCount;
	if (mesh->m_indices)
	{
		uint32_t index_sizeof = sizeof(uint16_t);
		m_indexType = DXGI_FORMAT_R16_UINT;
		if (mesh->m_indexType == alMeshIndexType::u32)
		{
			m_indexType = DXGI_FORMAT_R32_UINT;
			index_sizeof = sizeof(uint32_t);
		}
		ibd.ByteWidth = index_sizeof * mesh->m_iCount;
		iData.pSysMem = &mesh->m_indices[0];


		hr = g_d3d11->GetD3D11Device()->CreateBuffer(&ibd, &iData, &m_iBuffer);
		if (FAILED(hr))
		{
			alLog::PrintError("Can't create Direct3D 11 index buffer [%u]\n", hr);
			return;
		}
	}

	return;
}


void alD3D11Model::MapModelForWriteVerts(uint8_t** v_ptr)
{
	static D3D11_MAPPED_SUBRESOURCE mapData;
	auto hr = g_d3d11->GetD3D11DeviceContext()->Map(
		m_vBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapData
	);
	if (FAILED(hr))
	{
		alLog::PrintError("Can not lock D3D11 render model buffer. Code : %u\n", hr);
		return;
	}
	*v_ptr = (uint8_t*)mapData.pData;
	m_lockedResource = m_vBuffer;
}

void alD3D11Model::UnmapModelForWriteVerts()
{
	g_d3d11->GetD3D11DeviceContext()->Unmap(m_lockedResource, 0);
	m_lockedResource = nullptr;
}


