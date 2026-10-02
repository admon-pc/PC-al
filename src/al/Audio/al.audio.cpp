#include "al.h"
#include "Audio/alAudio.h"


#include "../al_internal.h"
extern alLibImpl* g_alLib;

alAudioBufferRAW* alLibImpl::LoadAudioWAV(alFileBuffer* fb, alAudioBufferInfo* ai)
{
	alAudioBufferRAW* audioBuffer = 0;

	fb->Seek(0, SEEK_SET);
	wav_header_t wav_header;
	fb->Read(&wav_header, sizeof(wav_header_t));
	switch (ai->m_format)
	{
	case alAudioFormat::PCM_8:
	{
		fb->Seek(36, SEEK_SET);
		char datastr[5] = { 0,0,0,0,0 };
		fb->Read(datastr, 4);
		uint32_t datasz = 0;
		fb->Read(&datasz, 4);
		audioBuffer = alCreate<alAudioBufferRAW>();
		audioBuffer->m_bufferInfo = *ai;
		audioBuffer->m_bufferInfo.m_additionalInfo.m_fileType = alAudioBufferInfo2::fileType_unknown;
		audioBuffer->m_dataSize = datasz;
		audioBuffer->m_data = (uint8_t*)alMemory::Malloc(datasz);
		fb->Read(audioBuffer->m_data, datasz);
	}break;
	case alAudioFormat::IEEE_float32:
	{
		wav_header_18_t* wav_header18 = (wav_header_18_t*)&wav_header;
		fb->Seek(38, SEEK_SET);
		fb->Seek(wav_header18->cb_size, SEEK_CUR);
		auto pos = fb->Tell();
		char datastr[5] = { 0,0,0,0,0 };
		fb->Read(datastr, 4);
		uint32_t datasz = 0;
		fb->Read(&datasz, 4);

		audioBuffer = alCreate<alAudioBufferRAW>();
		audioBuffer->m_bufferInfo = *ai;
		audioBuffer->m_bufferInfo.m_additionalInfo.m_fileType = alAudioBufferInfo2::fileType_unknown;
		audioBuffer->m_dataSize = datasz;
		audioBuffer->m_data = (uint8_t*)alMemory::Malloc(datasz);
		fb->Read(audioBuffer->m_data, datasz);
	}break;
	}
	return audioBuffer;
}

