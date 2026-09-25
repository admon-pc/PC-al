#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

alAudioMixer::alAudioMixer()
{
}

alAudioMixer::~alAudioMixer()
{
	DeleteAllAudioObjects();
	if (m_buffer.m_data)
		alDestroy(m_buffer.m_data);
}

alAudioBufferRAW* alAudioMixer::GetBuffer()
{
	return &m_buffer; 
}

float32_t alAudioMixer::GetVolume()
{
	return m_volume;
}

void alAudioMixer::SetVolume(float32_t v)
{
	m_volume = v;
}

alAudioObject* alAudioMixer::GetNewAudioObject(alAudioBuffer* ab)
{
	alAudioObject* ao = 0;
	if (ab)
	{
		ao = alCreate<alAudioObject>();
		ao->m_buffer = ab;
		m_audioObjects.push_back(ao);
	}
	return ao;
}

void alAudioMixer::DeleteAllAudioObjects()
{
	for (size_t i = 0; i < m_audioObjects.m_size; ++i)
	{
		alDestroy(m_audioObjects.m_data[i]);
	}
	m_audioObjects.clear();
	m_audioObjects.ShrinkToFit();
}

