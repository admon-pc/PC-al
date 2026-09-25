#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

alAudio::alAudio()
{
	m_mainMixer = GetNewMixer();
	m_mixers.clear();
}

alAudio::~alAudio()
{
	AL_DESTROY(m_mainMixer);
}

alAudioBufferInfo alAudio::GetDeviceFormat()
{
	return g_alLib->m_audioEngine->GetDeviceInfo();
}

alAudioMixer* alAudio::GetMainMixer()
{
	return m_mainMixer;
}

alAudioMixer* alAudio::GetNewMixer()
{
	alAudioMixer* mixer = alCreate<alAudioMixer>();
	if (mixer)
	{
		auto bi = GetDeviceFormat();
		mixer->m_buffer.m_bufferInfo = bi;
		mixer->m_buffer.m_dataSize = bi.m_bytesPerSecond / 10;
		mixer->m_buffer.m_data = (uint8_t*)alMemory::Calloc(mixer->m_buffer.m_dataSize);

		m_mixers.push_back(mixer);
	}
	return mixer;
}
