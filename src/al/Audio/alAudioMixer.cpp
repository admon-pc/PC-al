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
	AL_ASSERT_ST(ab);
	alAudioObject* ao = 0;
	if (ab)
	{
		ao = alCreate<alAudioObject>();
		ao->m_buffer = ab;
		ao->m_numOfBlocks = ab->m_rawData->m_dataSize / ab->m_rawData->m_bufferInfo.m_bytesPerBlock;
		m_audioObjects.push_back(ao);
	}
	return ao;
}

void alAudioMixer::DeleteAllAudioObjects()
{
	if (m_audioObjects.m_size)
	{

		{
			alAudioEngine::queue_data cmd;
			cmd.m_cmd = alAudioEngine::queueCMD_stop;
			g_alLib->m_audioEngine->AddCommand(cmd);
			std::unique_lock<std::mutex> lock(g_alLib->m_audio_mtx);
			g_alLib->m_audio_cv.wait(lock, [] { return g_alLib->m_audio_cv_ready; });
			g_alLib->m_audio_cv_ready = false;
		}

		for (size_t i = 0; i < m_audioObjects.m_size; ++i)
		{
			alDestroy(m_audioObjects.m_data[i]);
		}
		m_audioObjects.clear();
		m_audioObjects.ShrinkToFit();


		{
			alAudioEngine::queue_data cmd;
			cmd.m_cmd = alAudioEngine::queueCMD_resume;
			g_alLib->m_audioEngine->AddCommand(cmd);
			std::unique_lock<std::mutex> lock(g_alLib->m_audio_mtx);
			g_alLib->m_audio_cv.wait(lock, [] { return g_alLib->m_audio_cv_ready; });
			g_alLib->m_audio_cv_ready = false;
		}
	}
}

size_t alAudioMixer::GetAudioObjectNum()
{
	return m_audioObjects.size();
}

alAudioObject* alAudioMixer::GetAudioObject(size_t i)
{
	if (i < m_audioObjects.size())
	{
		return m_audioObjects.m_data[i];
	}

	return 0;
}

