#ifndef _ALD3D11M_H_
#define _ALD3D11M_H_

#include "GS/alGSMesh.h"

class alD3D11Model : public alGSMesh
{
public:
	alD3D11Model();
	virtual ~alD3D11Model();

	virtual void MapModelForWriteVerts(uint8_t** v_ptr);
	virtual void UnmapModelForWriteVerts();
	
	void OnCreate();

	ID3D11Buffer* m_lockedResource = 0;
	ID3D11Buffer* m_vBuffer = 0;
	ID3D11Buffer* m_iBuffer = 0;
	uint32_t m_stride = 0;
	uint32_t m_iCount = 0;

	DXGI_FORMAT m_indexType = DXGI_FORMAT_R16_UINT;

	alMeshVertexType m_vertexType = alMeshVertexType::Triangle;
};

#endif
