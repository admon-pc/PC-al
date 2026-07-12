
#pragma once 
#ifndef _AL_IMGLDRPNG_H__
#define _AL_IMGLDRPNG_H__

#include "Image/alImageLoader.h"

class alImageLoaderPNG : public alImageLoader
{
public:
	alImageLoaderPNG();
	virtual ~alImageLoaderPNG();

	virtual alImage* Load(const char*) override;
	virtual alImage* Load(alFileBuffer*) override;
	virtual bool GetInfo(const char* p, alImage* img) override;
	virtual bool GetInfo(alFileBuffer*, alImage* img) override;

	virtual uint32_t GetExtensionNum() override;
	virtual const char* GetExtension(uint32_t) override;

	virtual bool Save(alImage*, const char* fileName) override;
};

#endif
