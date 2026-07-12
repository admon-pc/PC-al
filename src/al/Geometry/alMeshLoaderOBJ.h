#ifndef _AL_MESHLDROBJH_
#define _AL_MESHLDROBJH_

class alMeshLoaderOBJ : public alMeshLoader
{
public:
	alMeshLoaderOBJ();
	virtual ~alMeshLoaderOBJ();

	virtual bool Load(const char*, alMeshLoaderCallback*) override;
	virtual bool Load(alFileBuffer*, alMeshLoaderCallback*) override;

	virtual uint32_t GetExtensionNum() override;
	virtual const char* GetExtension(uint32_t) override;
};

#endif


