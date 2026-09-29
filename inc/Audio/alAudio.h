#pragma once
#ifndef _AL_AUDIO_H_
#define _AL_AUDIO_H_

enum class alAudioFormat
{
	Unknown,
	PCM_8,
	PCM_16,
	IEEE_float32,
};

// Some additional information about the audio
// This struct has information that, what we can call, not important.
// Because, if you add this fields into alAudioBufferInfo,
// then every time you use alAudioBufferInfo, you will think about
// these fields, you will think 'do I relly need to initialize 
// all these things?'. Yes you can use memset().
// But it's still confusing. So, if you use alAudioBufferInfo,
// 100% you must set correct information.
// alAudioBufferInfo2 has some additional information.
// It's depends when you need to set some of these fields, 
// read comments for information.
//
struct alAudioBufferInfo2
{
	float32_t m_length = 0.f;

	enum
	{
		fileType_unknown,
		fileType_wav,
	};
	uint32_t m_fileType = 0;
};

// Important information about audio buffer.
// For not so important information (like length in milliseconds)
//  use alAudioBufferInfo2
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

	alAudioBufferInfo2 m_additionalInfo;
};



class alAudioBufferRAW
{
public:
	alAudioBufferRAW() {}
	~alAudioBufferRAW() { if (m_data) alMemory::Free(m_data); }

	alAudioBufferInfo m_bufferInfo;

	uint8_t* m_data = 0;
	uint32_t m_dataSize = 0;
};

// buffer that 100% has same format as device
class alAudioBuffer
{
public:
	alAudioBuffer() {}
	~alAudioBuffer() { AL_DESTROY(m_rawData); }

	alAudioBufferRAW* m_rawData = 0;
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
	
	float32_t GetVolume() { return m_volume; }
	void SetVolume(float32_t);

	// Position is index in m_buffer->m_rawData.m_data[]
	// So position in bytes.
	uint32_t GetPosition() { return m_position; }
	void SetPosition(uint32_t);

	alAudioBuffer* GetBuffer() { return m_buffer; }
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

	size_t GetAudioObjectNum();
	alAudioObject* GetAudioObject(size_t);
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
	size_t GetMixerNum();
	alAudioMixer* GetMixer(size_t);

	// this will create new mixer.
	// They all will be deleted when program end.
	alAudioMixer* GetNewMixer();

	// This methods will load audio and convert it
	// into device's format
	alAudioBuffer* LoadAudio(const char*);
	alAudioBuffer* LoadAudio(alFileBuffer*);

	//======================================================
	// STATIC METHODS
	// These methods you can use without audio engine initialization.
	// 
	// 
	// Load audio data without any conversion.
	// Audio Device has it's own format. For playing audio
	//     the engine use alAudioBuffer. This class 100% has same type as device.
	//     Use alAudio class for reading file into alAudioBuffer,
	//     or for converting alAudioBufferRAW to alAudioBuffer.
	// When reading, engine will determine format not by extension, but by
	//  reading internal file headers.
	static alAudioBufferRAW* LoadRAWAudio(const char*);
	static alAudioBufferRAW* LoadRAWAudio(alFileBuffer*);

	// Get information about audio file.
	// it will be zeroed by memset so if something wrong everything will be 0
	static void GetAudioInfo(const char*, alAudioBufferInfo*);
	static void GetAudioInfo(alFileBuffer*, alAudioBufferInfo*);

	static void ChangeFormat(alAudioBufferRAW*, alAudioFormat);
	// From 100 to 192000
	static void ChangeSampleRate(alAudioBufferRAW*, uint32_t);
	static void MakeMono(alAudioBufferRAW*);
	static void MakeStereo(alAudioBufferRAW*);
};

#endif
