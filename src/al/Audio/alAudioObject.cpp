#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

alAudioObject::alAudioObject()
{
}

alAudioObject::~alAudioObject()
{
}

void alAudioObject::SetVolume(float32_t v)
{
	if (v > 1.f)
		m_volume = 1.f;
	else if (v < 0.f)
		m_volume = 0.f;
	else
		m_volume = v;
}

void alAudioObject::SetPosition(uint32_t p)
{
	m_position = p;
	if (m_position >= m_buffer->m_rawData->m_dataSize)
		m_position = m_buffer->m_rawData->m_dataSize - m_buffer->m_rawData->m_bufferInfo.m_bytesPerBlock;
}


