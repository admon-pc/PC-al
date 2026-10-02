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
		if(g_alLib->m_audioThread)
		{
			alAudioEngine::queue_data cmd;
			cmd.m_cmd = alAudioEngine::queueCMD_stop;
			g_alLib->m_audioEngine->AddCommand(cmd);
			std::unique_lock<std::mutex> lock(g_alLib->m_audio_mtx);
			g_alLib->m_audio_cv.wait(lock, [] { return g_alLib->m_audio_cv_ready; });
			g_alLib->m_audio_cv_ready = false;
		}

		auto bi = GetDeviceFormat();
		mixer->m_buffer.m_bufferInfo = bi;
		mixer->m_buffer.m_dataSize = bi.m_bytesPerBlock * (bi.m_sampleRate / 10);
		mixer->m_buffer.m_data = (uint8_t*)alMemory::Malloc(mixer->m_buffer.m_dataSize);

		m_mixers.push_back(mixer);

		if (g_alLib->m_audioThread) 
		{
			alAudioEngine::queue_data cmd;
			cmd.m_cmd = alAudioEngine::queueCMD_resume;
			g_alLib->m_audioEngine->AddCommand(cmd);
			std::unique_lock<std::mutex> lock(g_alLib->m_audio_mtx);
			g_alLib->m_audio_cv.wait(lock, [] { return g_alLib->m_audio_cv_ready; });
			g_alLib->m_audio_cv_ready = false;
		}
	}
	return mixer;
}

size_t alAudio::GetMixerNum()
{
	return m_mixers.size();
}

alAudioMixer* alAudio::GetMixer(size_t i)
{
	if (i < m_mixers.m_size)
		return m_mixers.m_data[i];

	return 0;
}


alAudioBufferRAW* alAudio::LoadRAWAudio(const char* fn)
{
	AL_ASSERT_ST(fn);
	if (fn)
	{
		alFileBuffer fb;
		fb.ReadFile(fn);
		return LoadRAWAudio(&fb);
	}
	return 0;
}

alAudioBufferRAW* alAudio::LoadRAWAudio(alFileBuffer* fb)
{
	AL_ASSERT_ST(fb);
	if (fb)
	{
		alAudioBufferInfo info;
		GetAudioInfo(fb, &info);

		switch (info.m_additionalInfo.m_fileType)
		{
		case alAudioBufferInfo2::fileType_wav:
			return g_alLib->LoadAudioWAV(fb, &info);
		}
	}
	return 0;
}

void alAudio::GetAudioInfo(const char* fn, alAudioBufferInfo* i)
{
	AL_ASSERT_ST(fn);
	AL_ASSERT_ST(i);
	if (fn && i)
	{
		alFileBuffer fb;
		fb.ReadFile(fn, 100);
		GetAudioInfo(&fb, i);
	}
}

void alAudio::GetAudioInfo(alFileBuffer* fb, alAudioBufferInfo* info)
{
	AL_ASSERT_ST(fb);
	AL_ASSERT_ST(info);

	memset(info, 0, sizeof(alAudioBufferInfo));

	alAudioBufferInfo inf;
	memset(&inf, 0, sizeof(inf));

	if (fb)
	{
		{
			wav_header_t wav_header;
			fb->Read(&wav_header, sizeof(wav_header_t));
			if (wav_header.riff[0] == 'R'
				&& wav_header.riff[1] == 'I'
				&& wav_header.riff[2] == 'F'
				&& wav_header.riff[3] == 'F')
			{
				if (wav_header.wave[0] == 'W'
					&& wav_header.wave[1] == 'A'
					&& wav_header.wave[2] == 'V'
					&& wav_header.wave[3] == 'E')
				{
					if (wav_header.fmt_id[0] == 'f'
						&& wav_header.fmt_id[1] == 'm'
						&& wav_header.fmt_id[2] == 't'
						&& wav_header.fmt_id[3] == ' ')
					{
						inf.m_channels = wav_header.num_channels;
						inf.m_sampleRate = wav_header.sample_rate;

						inf.m_format = alAudioFormat::Unknown;

						// 2 channels; pcm8, pcm16, IEEE float
						if (wav_header.audio_format == WAVE_FORMAT_PCM)
						{
							switch (wav_header.bits_per_sample)
							{
							case 8:
								inf.m_format = alAudioFormat::PCM_8;
								inf.m_bytesPerSample = 1;
								break;
							case 16:
								inf.m_format = alAudioFormat::PCM_16;
								inf.m_bytesPerSample = 2;
								break;
							}
							inf.m_bytesPerBlock = inf.m_bytesPerSample * inf.m_channels;
							inf.m_bytesPerSecond = inf.m_sampleRate * inf.m_bytesPerBlock;
							inf.m_additionalInfo.m_length = (float)wav_header.data_size / (float)inf.m_bytesPerSecond;
						}
						else if (wav_header.audio_format == WAVE_FORMAT_IEEE_FLOAT)
						{
							wav_header_18_t* wav_header18 = (wav_header_18_t*)&wav_header;
							fb->Seek(38, SEEK_SET);
							fb->Seek(wav_header18->cb_size, SEEK_CUR);
							auto pos = fb->Tell();
							char datastr[5] = { 0,0,0,0,0 };
							fb->Read(datastr, 4);
							uint32_t datasz = 0;
							fb->Read(&datasz, 4);
							if (::strcmp(datastr, "data") == 0)
							{
								inf.m_format = alAudioFormat::IEEE_float32;
								inf.m_bytesPerSample = 4;

								inf.m_bytesPerBlock = inf.m_bytesPerSample * inf.m_channels;
								inf.m_bytesPerSecond = inf.m_sampleRate * inf.m_bytesPerBlock;
								inf.m_additionalInfo.m_length = (float)datasz / (float)inf.m_bytesPerSecond;
							}
							else
							{
								memset(&inf, 0, sizeof(inf));
							}
						}


						inf.m_additionalInfo.m_fileType = alAudioBufferInfo2::fileType_wav;
						*info = inf;
						/*alLog::Print("\tchunk_size: %u\n", wav_header.chunk_size);
						alLog::Print("\tfmt_size: %u\n", wav_header.fmt_size);
						alLog::Print("\taudio_format: %u\n", wav_header.audio_format);
						alLog::Print("\tnum_channels: %u\n", wav_header.num_channels);
						alLog::Print("\tsample_rate: %u\n", wav_header.sample_rate);
						alLog::Print("\tbyte_rate: %u\n", wav_header.byte_rate);
						alLog::Print("\tblock_align: %u\n", wav_header.block_align);
						alLog::Print("\tbits_per_sample: %u\n", wav_header.bits_per_sample);
						alLog::Print("\t\tlen: %f\n", inf.m_additionalInfo.m_length);*/

					}
				}
			}
		}
	}
}

alAudioBuffer* alAudio::LoadAudio(const char* fn)
{
	AL_ASSERT_ST(fn);
	if (fn)
	{
		alFileBuffer fb;
		fb.ReadFile(fn);

		return LoadAudio(&fb);
	}
	return 0;
}

alAudioBuffer* alAudio::LoadAudio(alFileBuffer* fb)
{
	alAudioBuffer* newBuffer = 0;
	auto raw = alAudio::LoadRAWAudio(fb);
	if (raw)
	{
		auto bi = GetDeviceFormat();
		ChangeFormat(raw, bi.m_format);
		{
			FILE* f = 0;
			fopen_s(&f, "ChangeFormat.raw", "wb");
			if (f)
			{
				fwrite(raw->m_data, 1, raw->m_dataSize, f);
				fclose(f);
			}
		}
		ChangeSampleRate(raw, bi.m_sampleRate);
		{
			FILE* f = 0;
			fopen_s(&f, "ChangeSampleRate.raw", "wb");
			if (f)
			{
				fwrite(raw->m_data, 1, raw->m_dataSize, f);
				fclose(f);
			}
		}

		switch (bi.m_channels)
		{
		case 1:
			MakeMono(raw);
			break;
		case 2:
			MakeStereo(raw);
			break;
		}

		newBuffer = alCreate<alAudioBuffer>();
		newBuffer->m_rawData = raw;
	}
	return newBuffer;
}

void alAudio::ChangeFormat(alAudioBufferRAW* raw, alAudioFormat fmt)
{
	AL_ASSERT_ST(raw);
	AL_ASSERT_ST(raw->m_data);
	AL_ASSERT_ST(raw->m_dataSize);
	AL_ASSERT_ST(fmt != alAudioFormat::Unknown);
	if (raw && (fmt != alAudioFormat::Unknown))
	{
		if (raw->m_bufferInfo.m_format == fmt)
			return;

		uint32_t numOfChannels = raw->m_bufferInfo.m_channels;
		uint32_t numOfBlocks = raw->m_dataSize / raw->m_bufferInfo.m_bytesPerBlock;
		uint32_t newBytesPerSample = 0;
		switch (fmt)
		{
		case alAudioFormat::PCM_8:
			newBytesPerSample = 1;
			break;
		case alAudioFormat::PCM_16:
			newBytesPerSample = 2;
			break;
		case alAudioFormat::IEEE_float32:
			newBytesPerSample = 4;
			break;
		default:
			return;
		}
		uint32_t newBytesPerBlock = newBytesPerSample * numOfChannels;

		uint32_t newDataSize = numOfBlocks * newBytesPerBlock;
		uint8_t* newData = (uint8_t*)alMemory::Malloc(newDataSize);
		uint8_t* oldData = raw->m_data;

		uint8_t* srcPCM8 = oldData;
		uint8_t* dstPCM8 = newData;
		int16_t* srcPCM16 = (int16_t*)oldData;
		int16_t* dstPCM16 = (int16_t*)newData;
		float32_t* srcIEEEF32 = (float32_t*)oldData;
		float32_t* dstIEEEF32 = (float32_t*)newData;

		switch (raw->m_bufferInfo.m_format)
		{
		case alAudioFormat::PCM_8:
		{
			float64_t mm = 2.0 / 255.0;
			switch (fmt)
			{
			case alAudioFormat::PCM_8: {
			}break;
			case alAudioFormat::PCM_16: {
			}break;
			case alAudioFormat::IEEE_float32:{
				for (uint32_t i = 0; i < numOfBlocks; ++i)
				{
					if (numOfChannels == 1)
					{
						dstIEEEF32[0] = (float32_t)((int)srcPCM8[0] - 127) * mm;
						++srcPCM8;
						++dstIEEEF32;
					}
					else if (numOfChannels == 2)
					{
						dstIEEEF32[0] = (float32_t)((int)srcPCM8[0] - 127) * mm;
						dstIEEEF32[1] = (float32_t)((int)srcPCM8[1] - 127) * mm;
						++srcPCM8;
						++srcPCM8;
						++dstIEEEF32;
						++dstIEEEF32;
					}
				}
			}break;
			}
		}break;
		case alAudioFormat::PCM_16:
		{
			//0,000030517578125

			float64_t mm = 0.000030517578125;
			switch (fmt)
			{
			case alAudioFormat::PCM_8: {
			}break;
			case alAudioFormat::PCM_16: {
			}break;
			case alAudioFormat::IEEE_float32: {
				for (uint32_t i = 0; i < numOfBlocks; ++i)
				{
					if (numOfChannels == 1)
					{
						dstIEEEF32[0] = (float32_t)((int)srcPCM16[0]) * mm;
						++srcPCM16;
						++dstIEEEF32;
					}
					else if (numOfChannels == 2)
					{
						dstIEEEF32[0] = (float32_t)((int)srcPCM16[0]) * mm;
						dstIEEEF32[1] = (float32_t)((int)srcPCM16[1]) * mm;
						++srcPCM16;
						++srcPCM16;
						++dstIEEEF32;
						++dstIEEEF32;
					}
				}
			}break;
			}
		}break;
		case alAudioFormat::IEEE_float32:
		{
		}break;
		}



		alMemory::Free(raw->m_data);
		raw->m_data = newData;
		raw->m_dataSize = newDataSize;
		raw->m_bufferInfo.m_format = fmt;
		raw->m_bufferInfo.m_bytesPerBlock = newBytesPerBlock;
		raw->m_bufferInfo.m_bytesPerSample = newBytesPerSample;
		raw->m_bufferInfo.m_bytesPerSecond = raw->m_bufferInfo.m_sampleRate * raw->m_bufferInfo.m_bytesPerBlock;
	}
}

void alAudio_ChangeSampleRate(alAudioBufferRAW* raw, uint32_t newSampleRate)
{
	if (newSampleRate != raw->m_bufferInfo.m_sampleRate)
	{
		uint32_t numOfBlocks = raw->m_dataSize / raw->m_bufferInfo.m_bytesPerBlock;

		float32_t multipler = (float32_t)newSampleRate / (float32_t)raw->m_bufferInfo.m_sampleRate;
		float32_t multipler2 = (float32_t)raw->m_bufferInfo.m_sampleRate / (float32_t)newSampleRate;
		uint32_t newNumOfBlocks = (uint32_t)floorf((float32_t)numOfBlocks * multipler);

		uint32_t newDataSize = newNumOfBlocks * raw->m_bufferInfo.m_bytesPerBlock;
		uint8_t* newData = (uint8_t*)alMemory::Malloc(newDataSize);
		memset(newData, 0, newDataSize);

		uint8_t* oldData = raw->m_data;

		uint8_t* srcPCM8 = oldData;
		uint8_t* dstPCM8 = newData;
		uint16_t* srcPCM16 = (uint16_t*)oldData;
		uint16_t* dstPCM16 = (uint16_t*)newData;
		float32_t* srcIEEEF32 = (float32_t*)oldData;
		float32_t* dstIEEEF32 = (float32_t*)newData;

		
		{
			uint32_t index = 0;
			
			float32_t prevL = 0.f;
			float32_t prevR = 0.f;

			uint32_t previ2 = 0;

			float val = 0.f;

			if (newSampleRate < raw->m_bufferInfo.m_sampleRate)
			{
				for (uint32_t i = 0; i < numOfBlocks; ++i)
				{
					uint32_t indexS = i * raw->m_bufferInfo.m_channels;

					switch (raw->m_bufferInfo.m_format)
					{
					case alAudioFormat::PCM_8:

						break;
					case alAudioFormat::PCM_16:
						break;
					case alAudioFormat::IEEE_float32:
					{
						float32_t* src = &srcIEEEF32[indexS];
						uint32_t i2 = index * raw->m_bufferInfo.m_channels;
						float32_t* dst = &dstIEEEF32[i2];
						dst[0] = src[0];
						if (raw->m_bufferInfo.m_channels == 2)
						{
							dst[1] = src[1];
						}
					}break;
					}

					val += multipler;
					if (val >= 1.f)
					{
						uint32_t ival = (uint32_t)floorf(val);
						index += ival;

						val -= ival;
					}
				}
			}
			else
			{
				for (uint32_t i = 0; i < newNumOfBlocks; ++i)
				{
					uint32_t indexD = i * raw->m_bufferInfo.m_channels;

					switch (raw->m_bufferInfo.m_format)
					{
					case alAudioFormat::PCM_8:

						break;
					case alAudioFormat::PCM_16:
						break;
					case alAudioFormat::IEEE_float32:
					{
						uint32_t i2 = index * raw->m_bufferInfo.m_channels;
						
						float32_t* src = &srcIEEEF32[i2];
						float32_t* dst = &dstIEEEF32[indexD];

						dst[0] = src[0];
						if (raw->m_bufferInfo.m_channels == 2)
						{
							dst[1] = src[1];
						}
					}break;
					}

					val += multipler2;
					if (val >= 1.f)
					{
						uint32_t ival = (uint32_t)floorf(val);
						index += ival;

						val -= ival;
					}
				}
			}

		}

		alMemory::Free(raw->m_data);
		raw->m_data = newData;
		raw->m_dataSize = newDataSize;
		raw->m_bufferInfo.m_sampleRate = newSampleRate;
		raw->m_bufferInfo.m_bytesPerSecond = newSampleRate * raw->m_bufferInfo.m_bytesPerBlock;
	}
}

void alAudio::ChangeSampleRate(alAudioBufferRAW* raw, uint32_t newSampleRate)
{
	AL_ASSERT_ST(raw);
	AL_ASSERT_ST((newSampleRate>=100)&&(newSampleRate <= 192000));

	if (newSampleRate > raw->m_bufferInfo.m_sampleRate)
	{
		int n = newSampleRate / raw->m_bufferInfo.m_sampleRate ;
		uint32_t sr = 0;
		for (int i = 0; i < n; ++i)
		{
			sr = raw->m_bufferInfo.m_sampleRate * 2;

			alAudio_ChangeSampleRate(raw, sr);
			if (sr > newSampleRate)
			{
				sr = newSampleRate;
				alAudio_ChangeSampleRate(raw, sr);
				break;
			}
		}
	}
	else
	{
		alAudio_ChangeSampleRate(raw, newSampleRate);
	}
}

void alAudio::MakeMono(alAudioBufferRAW* raw)
{
	AL_ASSERT_ST(raw);
}

void alAudio::MakeStereo(alAudioBufferRAW* raw)
{
	AL_ASSERT_ST(raw);
	if (raw->m_bufferInfo.m_channels == 1)
	{
		uint32_t newNumOfChannels = 2;

		uint32_t newBytesPerBlock = 0;
		switch (raw->m_bufferInfo.m_format)
		{
		case alAudioFormat::PCM_8:
			newBytesPerBlock = raw->m_bufferInfo.m_bytesPerSample * newNumOfChannels;
			break;
		case alAudioFormat::PCM_16:
			newBytesPerBlock = raw->m_bufferInfo.m_bytesPerSample * newNumOfChannels;
			break;
		case alAudioFormat::IEEE_float32:
			newBytesPerBlock = raw->m_bufferInfo.m_bytesPerSample * newNumOfChannels;
			break;
		default:
			return;
		}

		uint32_t numOfBlocks = raw->m_dataSize / raw->m_bufferInfo.m_bytesPerBlock;
		uint32_t newDataSize = numOfBlocks * newBytesPerBlock;
		uint8_t* newData = (uint8_t*)alMemory::Malloc(newDataSize);
		uint8_t* oldData = raw->m_data;

		uint8_t* srcPCM8 = oldData;
		uint8_t* dstPCM8 = newData;
		uint16_t* srcPCM16 = (uint16_t*)oldData;
		uint16_t* dstPCM16 = (uint16_t*)newData;
		float32_t* srcIEEEF32 = (float32_t*)oldData;
		float32_t* dstIEEEF32 = (float32_t*)newData;

		for (uint32_t i = 0; i < numOfBlocks; ++i)
		{
			switch (raw->m_bufferInfo.m_format)
			{
			case alAudioFormat::PCM_8:
				dstPCM8[0] = srcPCM8[0];
				dstPCM8[1] = srcPCM8[0];

				++srcPCM8;
				++dstPCM8;
				++dstPCM8;
				break;
			case alAudioFormat::PCM_16:
				dstPCM16[0] = srcPCM16[0];
				dstPCM16[1] = srcPCM16[0];

				++srcPCM16;
				++dstPCM16;
				++dstPCM16;
				break;
			case alAudioFormat::IEEE_float32:
				dstIEEEF32[0] = srcIEEEF32[0];
				dstIEEEF32[1] = srcIEEEF32[0];

				++srcIEEEF32;
				++dstIEEEF32;
				++dstIEEEF32;
				break;
			}

		}

		alMemory::Free(raw->m_data);
		raw->m_data = newData;
		raw->m_dataSize = newDataSize;
		raw->m_bufferInfo.m_channels = newNumOfChannels;
		raw->m_bufferInfo.m_bytesPerBlock = newBytesPerBlock;
		raw->m_bufferInfo.m_bytesPerSecond = raw->m_bufferInfo.m_sampleRate * raw->m_bufferInfo.m_bytesPerBlock;
	}
}

