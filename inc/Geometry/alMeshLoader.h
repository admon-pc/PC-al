#ifndef _AL_MESHLDRH_
#define _AL_MESHLDRH_

#include "Geometry/alMesh.h"

class alMeshLoaderCallback
{
public:
	alMeshLoaderCallback() {}
	virtual ~alMeshLoaderCallback() {}

	virtual void OnMesh(alMesh*) = 0;
};

class alMeshLoader
{
public:
	alMeshLoader() {}
	virtual ~alMeshLoader() {}

	virtual bool Load(const char*, alMeshLoaderCallback*) = 0;
	virtual bool Load(alFileBuffer*, alMeshLoaderCallback*) = 0;

	virtual uint32_t GetExtensionNum() = 0;
	virtual const char* GetExtension(uint32_t) = 0;
};

#endif

