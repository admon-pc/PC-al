#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"

#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;
extern alLibImpl* g_alLib;

class alAudioObjectCallbackDefault : public alAudioObjectCallback
{
public:
	alAudioObjectCallbackDefault() {}
	virtual ~alAudioObjectCallbackDefault() {}

	virtual void OnEnd() override {}
}
g_alAudioObjectCallbackDefault;

alAudioObject::alAudioObject()
{
	SetCallback(&g_alAudioObjectCallbackDefault);
}

alAudioObject::~alAudioObject()
{
}

void alAudioObject::SetCallback(alAudioObjectCallback* cb)
{
	if (cb)
		m_cb = cb;
	else
		m_cb = &g_alAudioObjectCallbackDefault;
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
	{
		m_position = m_buffer->m_rawData->m_dataSize - m_buffer->m_rawData->m_bufferInfo.m_bytesPerBlock;
	}
	else
	{
		if (m_position)
		{
			auto v = m_position / m_buffer->m_rawData->m_bufferInfo.m_bytesPerBlock;
			m_position = v * m_buffer->m_rawData->m_bufferInfo.m_bytesPerBlock;
		}
		else
		{
			m_position = p;
		}
	}
}

void alAudioObject::Play()
{
	m_playbackState = playbackState_play;
}

void alAudioObject::Pause()
{
	m_playbackState = playbackState_pause;
}

void alAudioObject::Reset()
{
	m_playbackState = playbackState_pause;
	SetPosition(0);
}

bool alAudioObject::IsPlaying()
{
	return (m_playbackState == playbackState_play);
}
