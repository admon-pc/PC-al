#pragma once

#include "Input/alInput.h"

#include "Containers/alFIFO.h"

#include "System/alCursor.h"
#include "Audio/alAudio.h"
#include "Audio/alAudioEngine.h"
#include "GUI/alGUI.h"

#ifdef AL_PLATFORM_WIN32
#include "System/alCursorWin32.h"
struct IFileSaveDialog;
struct IFileOpenDialog;
//NOMINMAX for std::min std::max
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <thread>

#ifdef LoadImage
#undef LoadImage
#endif

#ifdef DrawText
#undef DrawText
#endif

#endif

#define AL_EVENT_MAX 30
#define AL_MAX_BONES 250

class alLibGlobalData
{
public:
	alLibGlobalData();

	~alLibGlobalData();

#ifdef AL_PLATFORM_WIN32
	HANDLE m_processHeap = 0;
	HINSTANCE m_moduleHandle = 0;
#endif
};

#include "Common/alOStreamImpl.h"

#pragma pack(push, 1)  /* no padding */

typedef struct {
	/* RIFF header (12 bytes) */
	char     riff[4];          /* "RIFF"             */
	uint32_t chunk_size;       /* file size - 8      */
	char     wave[4];          /* "WAVE"             */

	/* fmt sub-chunk (24 bytes) */
	char     fmt_id[4];        /* "fmt " (trailing space) */
	uint32_t fmt_size;         /* 16 for PCM         */
	uint16_t audio_format;     /* 1 = PCM            */
	uint16_t num_channels;     /* 1 = mono, 2 = stereo */
	uint32_t sample_rate;      /* e.g. 44100 Hz      */
	uint32_t byte_rate;        /* sample_rate * num_channels * bits_per_sample / 8 */
	uint16_t block_align;      /* num_channels * bits_per_sample / 8 */
	uint16_t bits_per_sample;  /* e.g. 16            */

	/* data sub-chunk (8 bytes) */
	char     data_id[4];       /* "data"             */
	uint32_t data_size;        /* byte count of audio data */
} wav_header_t;
typedef struct {
	char     riff[4];
	uint32_t chunk_size;
	char     wave[4];
	char     fmt_id[4];
	uint32_t fmt_size;         /* 18 */
	uint16_t audio_format;     /* 1 (PCM) or 3 (IEEE float) */
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;  /* e.g. 24 or 32 */
	uint16_t cb_size;          /* 0 (no further extra fields) */
} wav_header_18_t;             /* total: 46 bytes */

#pragma pack(pop)

class alLibImpl
{
public:
	alLibImpl();
	~alLibImpl();

	alInput* m_input = 0;
	//bool m_cursorDisableAutoChange = false;
	bool m_showCursor = true;

	alSystemWindow* CreateSystemWindow(alSystemWindowCallback*);
	alEvent m_events[AL_EVENT_MAX];
	uint32_t m_events_num = 0;
	uint32_t m_events_current = 0;

	alMat4* m_matrixPtrs[(uint32_t)alMatrixType::_count];
	alMat4 m_matrixBones[AL_MAX_BONES];
	alMat4 m_defaultMatrix[(uint32_t)alMatrixType::_count];

	alArray<alImageLoader*> m_imageLoaders;
	alImageLoader* m_imageLoader_PNG = 0;
	
	alArray<alMeshLoader*> m_meshLoaders;

	alCursor* m_cursors[(uint32_t)alCursorType::_count];
	alCursor* m_cursorsDefault[(uint32_t)alCursorType::_count];
	alVec4i m_cursorClip;

	float32_t m_dt = 0.f;

	alGUIFont* m_fontMini = 0;
	alGUIColorTheme m_colorTheme;

	IFileSaveDialog* m_fileSaveDialog = 0;
	IFileOpenDialog* m_fileOpenDialog = 0;

	alAudio* m_audio = 0;
	alAudioEngine* m_audioEngine = 0;
	std::thread* m_audioThread = 0;
	std::mutex m_audio_mtx;
	std::condition_variable m_audio_cv;
	bool m_audio_cv_ready = false;
	alAudioBufferRAW* LoadAudioWAV(alFileBuffer* fb, alAudioBufferInfo* ai);
	
	alOStream_default m_ostream_default;

	alStringW m_ostream_bufferString;

};

class alAudioEngineWASAPI : public alAudioEngine
{
public:
	alAudioEngineWASAPI();
	virtual ~alAudioEngineWASAPI();

	IMMDevice* m_device = 0;
	IAudioClient* m_audioClient = 0;
	IAudioRenderClient* m_renderClient = 0;
	WAVEFORMATEX* m_mixFormat = 0;
	IMMDeviceEnumerator* m_deviceEnumerator = 0;

	uint32_t m_bufferSize = 0;
	uint32_t m_engineLatencyInMS = 50;

	bool m_run = false;

	enum RenderSampleType
	{
		SampleTypeFloat,
		SampleType16BitPCM,
	};
	RenderSampleType m_renderSampleType = SampleTypeFloat;



	virtual bool Initialize() override;
	virtual void AddCommand(const queue_data&) override;

	alFIFO<queue_data, 10> m_queue;
};




