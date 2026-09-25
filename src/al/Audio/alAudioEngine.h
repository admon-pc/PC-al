#pragma once
#ifndef _AL_AUDIOENG_H_
#define _AL_AUDIOENG_H_

#include <MMDeviceAPI.h>
#include <AudioClient.h>
#include <AudioPolicy.h>

class alAudioEngine
{
protected:
	alAudioBufferInfo m_audioDeviceInfo;
public:
	alAudioEngine() {}
	virtual ~alAudioEngine() {}

	virtual bool Initialize() = 0;
	
	alAudioBufferInfo GetDeviceInfo() { return m_audioDeviceInfo; }
};


#endif


