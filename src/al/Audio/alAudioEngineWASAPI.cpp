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

        alLog::PrintInfo("Audio Device : %s\n", stra.c_str());

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

    hr = m_audioClient->GetService(IID_PPV_ARGS(&m_renderClient));
    if (FAILED(hr))
    {
        alLog::PrintError("Unable to get new render client: %x.\n", hr);
        return false;
    }

    AL_SAFERELEASE(m_deviceEnumerator);
    return true;
}

void alAudioThreadFunction_WASAPI(alAudioEngineWASAPI* engine)
{
}
