#pragma once
#ifndef _AL_AUDIO_H_
#define _AL_AUDIO_H_

enum class alAudioFormat
{
	Unknown,
	PCM_8,
	PCM_16,
	PCM_32_float,
};

struct alAudioBufferInfo
{
	uint32_t m_sampleRate = 11000;
	uint32_t m_channels = 1;
	alAudioFormat m_format = alAudioFormat::PCM_16;
};

struct alAudioBufferRAW
{
	alAudioBufferInfo m_bufferInfo;

	uint8_t* m_data = 0;
	uint32_t m_dataSize = 0;
};


class alAudio
{
public:
	alAudio();
	~alAudio();

	alAudioBufferInfo GetDeviceFormat();
};

#endif
