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

class alAudioEngineWASAPI : public alAudioEngine
{
public:
	alAudioEngineWASAPI();
	virtual ~alAudioEngineWASAPI();

	IMMDevice* m_device = 0;
	IAudioClient* m_audioClient = 0;
	IAudioRenderClient* m_renderClient = 0;
	WAVEFORMATEX* m_mixFormat = 0;
	IMMDeviceEnumerator* m_deviceEnumerator = 0;

	uint32_t m_bufferSize = 0;
	uint32_t m_engineLatencyInMS = 50;

	bool m_run = false;

	enum RenderSampleType
	{
		SampleTypeFloat,
		SampleType16BitPCM,
	};
	RenderSampleType m_renderSampleType = SampleTypeFloat;


	virtual bool Initialize() override;
};


#endif


