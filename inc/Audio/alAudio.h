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

// buffer that 100% has same format as device
struct alAudioBuffer
{
	alAudioBufferRAW m_rawData;
};

// Object for playing the audio.
// Object has buffer - it must be same format as device
// Object has parameters for playing.
class alAudioObject
{
	friend class alAudioMixer;

	alAudioBuffer* m_buffer = 0;
	uint32_t m_position = 0;
	
	float32_t m_volume = 1.f;
public:
	alAudioObject();
	~alAudioObject();
	
	float32_t GetVolume();
	void SetVolume(float32_t);

	// Position is index in m_buffer->m_rawData.m_data[]
	// So position in bytes.
	uint32_t GetPosition();
	void SetPosition(uint32_t);
};

// All mixers have same format as device.
// Create new mixer using alAudio::GetNewMixer
class alAudioMixer
{
	friend class alAudio;

	alAudioBufferRAW m_buffer;

	float32_t m_volume = 1.f;
	alArray<alAudioObject*> m_audioObjects;
public:
	alAudioMixer();
	~alAudioMixer();

	alAudioBufferRAW* GetBuffer() { return &m_buffer; }
	float32_t GetVolume();
	void SetVolume(float32_t);

	alAudioObject* GetNewAudioObject(alAudioBuffer*);
	void DeleteAllAudioObjects();
};

class alAudio
{
	alAudioMixer* m_mainMixer = 0;
	alArray<alAudioMixer*> m_mixers;
public:
	alAudio();
	~alAudio();

	alAudioBufferInfo GetDeviceFormat();
	alAudioMixer* GetMainMixer();

	// this will create new mixer.
	// They all will be deleted when program end.
	alAudioMixer* GetNewMixer();

	alAudioBufferRAW* LoadRAW(const char*);
	alAudioBufferRAW* LoadRAW(alFileBuffer*);
};

#endif
