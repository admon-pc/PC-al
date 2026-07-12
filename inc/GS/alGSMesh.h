#ifndef _AL_GSMESHH_
#define _AL_GSMESHH_

#include "Geometry/alMesh.h"

class alGSMeshInfo
{
public:
	alGSMeshInfo() {}
	~alGSMeshInfo() {}

	alMesh* m_meshPtr = nullptr;
};

class alGSMesh
{
protected:
	alGSMeshInfo m_meshInfo;
public:
	alGSMesh() {}
	virtual ~alGSMesh() {}

	alGSMeshInfo* GetMeshInfo() { return &m_meshInfo; }

	// destroy old mesh, create new.
	// m_meshInfo->m_meshPtr must exist.
	//virtual void Reload() = 0;

	// destroy GPU buffers.
	// call Reload to create it again
	//virtual void Unload() = 0;

};

#endif

