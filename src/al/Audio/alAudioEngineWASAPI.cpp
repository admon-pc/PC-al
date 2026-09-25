#include "al.h"
#include "Audio/alAudio.h"

#include "alAudioEngine.h"
#include <functiondiscoverykeys.h>
#include "../al_internal.h"
extern alLibGlobalData g_alLibGlobalData;



alAudioEngineWASAPI::alAudioEngineWASAPI()
{
}

alAudioEngineWASAPI::~alAudioEngineWASAPI()
{
	if(m_audioClient)
		m_audioClient->Stop();
	AL_SAFERELEASE(m_audioClient);

   // AL_SAFERELEASE(m_deviceCollection);
	AL_SAFERELEASE(m_deviceEnumerator);
	AL_SAFERELEASE(m_device);
}

bool alAudioEngineWASAPI::Initialize()
{
	HRESULT hr = S_OK;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
		NULL, 
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_deviceEnumerator));
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to instantiate device enumerator: %x\n", hr);
		return false;
	}

  /*  hr = m_deviceEnumerator->EnumAudioEndpoints(
		eRender,
		DEVICE_STATE_ACTIVE, 
		&m_deviceCollection);
	if (FAILED(hr))
	{
		printf("Unable to retrieve device collection: %x\n", hr);
		return false;
	}

	UINT deviceCount = 0;
	hr = m_deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to get device collection length: %x\n", hr);
		return false;
	}*/

	ERole deviceRole = eMultimedia;
	hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, deviceRole, &m_device);
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to get default device for role %d : %x\n", deviceRole, hr);
		return false;
	}
	
	{
		IPropertyStore* propertyStore = 0;
		hr = m_device->OpenPropertyStore(STGM_READ, &propertyStore);
		if (FAILED(hr))
		{
			alLog::PrintError("Unable to open device property store : %x\n",  hr);
			return false;
		}

		PROPVARIANT friendlyName;
		PropVariantInit(&friendlyName);
		hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &friendlyName);
		AL_SAFERELEASE(propertyStore);

		if (FAILED(hr))
		{
			alLog::PrintError("Unable to retrieve friendly name for device : %x\n", hr);
			PropVariantClear(&friendlyName);
			return false;
		}

		wchar_t deviceName[128];
		swprintf(deviceName, 128, L"%s", friendlyName.vt != VT_LPWSTR ? L"Unknown" : friendlyName.pwszVal);

		alStringA stra;
		alUnicodeConverter::wchar_to_char(deviceName, wcslen(deviceName), &stra);

		alLog::PrintInfo("Audio Device : name [%s]\n", stra.c_str());

		PropVariantClear(&friendlyName);
	}

	hr = m_device->Activate(
		__uuidof(IAudioClient), 
		CLSCTX_INPROC_SERVER, 
		NULL, 
		reinterpret_cast<void**>(&m_audioClient));
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to activate audio client: %x.\n", hr);
		return false;
	}

	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator), 
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&m_deviceEnumerator));
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to instantiate device enumerator: %x\n", hr);
		return false;
	}

	hr = m_audioClient->GetMixFormat(&m_mixFormat);
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to get mix format on audio client: %x.\n", hr);
		return false;
	}

	//m_frameSize = m_mixFormat->nBlockAlign;
	
	m_audioDeviceInfo.m_channels = m_mixFormat->nChannels;
	m_audioDeviceInfo.m_sampleRate = m_mixFormat->nSamplesPerSec;
	m_audioDeviceInfo.m_format = alAudioFormat::PCM_16;

	if (m_mixFormat->wFormatTag == WAVE_FORMAT_PCM ||
		m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
		reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
	{
		if (m_mixFormat->wBitsPerSample == 16)
		{
			m_renderSampleType = SampleType16BitPCM;
		}
		else
		{
			alLog::PrintError("Unknown PCM integer sample type\n");
			return false;
		}
	}
	else if (m_mixFormat->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
		(m_mixFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
			reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_mixFormat)->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
	{
		m_renderSampleType = SampleTypeFloat;
		m_audioDeviceInfo.m_format = alAudioFormat::PCM_32_float;
	}
	else
	{
		alLog::PrintError("unrecognized device format.\n");
		return false;
	}

	alLog::PrintInfo("Audio Device : channels [%u]\n", m_audioDeviceInfo.m_channels);
	alLog::PrintInfo("Audio Device : sample rate [%u]\n", m_audioDeviceInfo.m_sampleRate);
	switch (m_audioDeviceInfo.m_format)
	{
	case alAudioFormat::PCM_16:
		alLog::PrintInfo("Audio Device : format [%s]\n", "PCM_16");
		m_audioDeviceInfo.m_bytesPerSample = 2;
		break;
	case alAudioFormat::PCM_32_float:
		alLog::PrintInfo("Audio Device : format [%s]\n", "PCM_32_float");
		m_audioDeviceInfo.m_bytesPerSample = 4;
		break;
	}
	m_audioDeviceInfo.m_bytesPerBlock = m_audioDeviceInfo.m_bytesPerSample * m_audioDeviceInfo.m_channels;
	alLog::PrintInfo("Audio Device : bytes per sample [%u]\n", m_audioDeviceInfo.m_bytesPerSample);
	alLog::PrintInfo("Audio Device : bytes per block [%u]\n", m_audioDeviceInfo.m_bytesPerBlock);
	
	m_audioDeviceInfo.m_bytesPerSecond = m_audioDeviceInfo.m_sampleRate * m_audioDeviceInfo.m_bytesPerBlock;
	alLog::PrintInfo("Audio Device : bytes per second [%u]\n", m_audioDeviceInfo.m_bytesPerSecond);

	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_NOPERSIST,
		m_engineLatencyInMS * 10000,
		0,
		m_mixFormat,
		NULL);

	if (FAILED(hr))
	{
		alLog::PrintError("Unable to initialize audio client: %x.\n", hr);
		return false;
	}

	hr = m_audioClient->GetBufferSize(&m_bufferSize);
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to get audio client buffer: %x. \n", hr);
		return false;
	}

	printf("m_bufferSize: %u\n", m_bufferSize);

	hr = m_audioClient->GetService(IID_PPV_ARGS(&m_renderClient));
	if (FAILED(hr))
	{
		alLog::PrintError("Unable to get new render client: %x.\n", hr);
		return false;
	}

	{
		BYTE* pData;
		hr = m_renderClient->GetBuffer(m_bufferSize, &pData);
		if (FAILED(hr))
		{
			alLog::PrintError("Failed to get buffer: %x.\n", hr);
			return false;
		}
		hr = m_renderClient->ReleaseBuffer(m_bufferSize, AUDCLNT_BUFFERFLAGS_SILENT);
		if (FAILED(hr))
		{
			alLog::PrintError("Failed to release buffer: %x.\n", hr);
			return false;
		}
	}
	
	m_audioClient->Start();

	AL_SAFERELEASE(m_deviceEnumerator);
	return true;
}

void alAudioThreadFunction_WASAPI(alAudioEngineWASAPI* engine)
{
	engine->m_run = true;
	auto di = engine->GetDeviceInfo();

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("Unable to initialize COM in render thread: %x\n", hr);
		return;
	}

	Sleep(100);

	uint8_t* wave = (uint8_t*)malloc(di.m_bytesPerSecond);
	if (wave)
	{
		auto wave_ptr = wave;

		uint16_t* pcm16_ch1 = (uint16_t*)wave_ptr;
		uint16_t* pcm16_ch2 = pcm16_ch1 + 1;

		float32_t* pcm32_ch1 = (float32_t*)wave_ptr;
		float32_t* pcm32_ch2 = pcm32_ch1 + 1;


		float64_t angle_step = (240.0 * PIPI) / (float64_t)di.m_sampleRate;
		float64_t angle = 0.f;
		for (uint32_t i = 0; i < di.m_sampleRate; ++i)
		{
			float64_t sn = sin(angle);
			
			/*
			float32_t t = (float32_t)i / (float32_t)di.m_sampleRate;
			float sn = sinf(PIPIf*240.f*t);
			*/
			switch (di.m_format)
			{
			case alAudioFormat::PCM_16:
				break;
			case alAudioFormat::PCM_32_float:
				*pcm32_ch1 = sn;
				if (di.m_channels == 2)
				{
					*pcm32_ch2 = sn;
				}

				wave_ptr += di.m_bytesPerBlock;
				pcm32_ch1 = (float32_t*)wave_ptr;
				pcm32_ch2 = pcm32_ch1 + 1;
				break;
			}

			angle += angle_step;
		}

		/*FILE* f = 0;
		fopen_s(&f, "wave.raw", "wb");
		if (f)
		{
			fwrite(wave, 1, di.m_bytesPerSecond, f);
			fclose(f);
		}*/
	}
	//engine->m_audioClient->Start();

	UINT32 data_position = 0;
	while (engine->m_run)
	{
		Sleep(10);

		if (wave)
		{

			BYTE* pData = 0;
			UINT32 padding = 0;
			UINT32 framesAvailable = 0;
		
			hr = engine->m_audioClient->GetCurrentPadding(&padding);
			if (SUCCEEDED(hr))
			{
				framesAvailable = engine->m_bufferSize - padding;
				//while ((_RenderBufferQueue->_BufferLength <= (framesAvailable * _FrameSize)))
				{
				//	printf("engine->m_bufferSize: %u\n", engine->m_bufferSize);
				//	printf("framesAvailable: %u\n", framesAvailable);
					hr = engine->m_renderClient->GetBuffer(framesAvailable, &pData);
					if (SUCCEEDED(hr))
					{
						//
						//  Copy data from the render buffer to the output buffer and bump our render pointer.
						uint32_t copy_sz = framesAvailable * di.m_bytesPerBlock;
						/*if ((data_position + copy_sz) > di.m_bytesPerSecond)
						{
							copy_sz = (data_position + copy_sz) - di.m_bytesPerSecond;
							CopyMemory(pData, &wave[data_position], copy_sz);
						}
						else*/
						{
							uint32_t copy_available = di.m_bytesPerSecond - data_position;
							if (copy_sz > copy_available)
							{
								//copy_sz = copy_available;
								printf("tik\n");
								CopyMemory(pData, &wave[data_position], copy_available);

								data_position = 0;
								// if repeat
								CopyMemory(&pData[copy_available], &wave[data_position], copy_sz - copy_available);
								data_position = copy_sz - copy_available;
								data_position -= copy_sz;
							}
							else
							{
								CopyMemory(pData, &wave[data_position], copy_sz);
							}
						}

						data_position += copy_sz;
						if (data_position >= di.m_bytesPerSecond)
							data_position = 0;

						hr = engine->m_renderClient->ReleaseBuffer(framesAvailable, 0);
						if (!SUCCEEDED(hr))
						{
							printf("Unable to release buffer: %x\n", hr);
						}
					}
					else
					{
						printf("Unable to get buffer: %x\n", hr);
						switch (hr)
						{
						case AUDCLNT_E_BUFFER_ERROR:
							printf("AUDCLNT_E_BUFFER_ERROR\n");
							break;
						case AUDCLNT_E_BUFFER_TOO_LARGE:
							printf("AUDCLNT_E_BUFFER_TOO_LARGE\n");
							break;
						case AUDCLNT_E_BUFFER_SIZE_ERROR:
							printf("AUDCLNT_E_BUFFER_SIZE_ERROR\n");
							break;
						case AUDCLNT_E_OUT_OF_ORDER:
							printf("AUDCLNT_E_OUT_OF_ORDER\n");
							break;
						case AUDCLNT_E_DEVICE_INVALIDATED:
							printf("AUDCLNT_E_DEVICE_INVALIDATED\n");
							break;
						case AUDCLNT_E_RESOURCES_INVALIDATED:
							printf("AUDCLNT_E_RESOURCES_INVALIDATED\n");
							break;
						case AUDCLNT_E_BUFFER_OPERATION_PENDING:
							printf("AUDCLNT_E_BUFFER_OPERATION_PENDING\n");
							break;
						case AUDCLNT_E_SERVICE_NOT_RUNNING:
							printf("AUDCLNT_E_SERVICE_NOT_RUNNING\n");
							break;
						case E_POINTER:
							printf("E_POINTER\n");
							break;
						}
					}

					hr = engine->m_audioClient->GetCurrentPadding(&padding);
					if (SUCCEEDED(hr))
					{
						framesAvailable = engine->m_bufferSize - padding;
					}
					else
					{
						printf("Unable to get current padding: %x\n", hr);
					}
				}
			}
		}


		while (!engine->m_queue.empty())
		{
			auto cmd = engine->m_queue.get_pop();
			
			switch (cmd.m_cmd)
			{
			case alAudioEngineWASAPI::queueCMD_quit:
				engine->m_run = false;
				//printf("Received\n");
				break;
			}
		}
	}

	if (wave)
	{
		free(wave);
	}

	CoUninitialize();
}
