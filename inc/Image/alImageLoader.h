#ifndef _ALIMGLDR_H_
#define _ALIMGLDR_H_

class alImageLoader
{
public:
	alImageLoader() {}
	virtual ~alImageLoader() {}

	virtual alImage* Load(const char*) = 0;
	virtual alImage* Load(alFileBuffer*) = 0;
	
	virtual bool GetInfo(const char* p, alImage* img) = 0;
	virtual bool GetInfo(alFileBuffer*, alImage* img) = 0;

	virtual uint32_t GetExtensionNum() = 0;
	virtual const char* GetExtension(uint32_t) = 0;
	
	virtual bool Save(alImage*, const char* fileName) = 0;
};

#endif

