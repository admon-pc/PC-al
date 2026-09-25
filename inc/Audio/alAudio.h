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

	// for 1 sample in 1 channel
	// PCM_16 is uint16_t, is 2 bytes
	uint32_t m_bytesPerSample = 2;

	// block is sample from each channel
	// `bytes per block` means m_bytesPerSample * m_channels
	uint32_t m_bytesPerBlock = m_bytesPerSample * m_channels;
	
	// Sample rate is how many samples will be transferred to device for 1 second
	// So bytes per second is m_sampleRate * m_bytesPerBlock
	uint32_t m_bytesPerSecond = m_sampleRate * m_bytesPerBlock;
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
