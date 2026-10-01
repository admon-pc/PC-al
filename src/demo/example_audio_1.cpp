#include "demo.h"
#include "example_audio_1.h"

#include "Audio/alAudio.h"

#include <filesystem>

#define DEMOGUIALL2_ID_CUT 101
#define DEMOGUIALL2_ID_COPY 102
#define DEMOGUIALL2_ID_PASTE 103
#define DEMOGUIALL2_ID_DELETE 104
#define DEMOGUIALL2_ID_SELECTALL 105

#define DEMOGUIALL2_CHECKBOXID_USEHSCROLL 1
#define DEMOGUIALL2_CHECKBOXID_USEVSCROLL 2
#define DEMOGUIALL2_BUTTON_DEACTIVATE 1



extern alDemo* g_demo;

bool DemoExample_audio_1::Init()
{
	m_gs = g_demo->m_gs;
	m_input = alLib::GetInput();

	//const char* files[] =
	//{
	//	{"../data/sounds/001_pcm8bit_1ch.wav"},
	//	{"../data/sounds/001_pcm8bit_2ch.wav"},
	//	{"../data/sounds/001_pcm8bit_4ch.wav"},
	//	{"../data/sounds/001_pcm16bit_2ch.wav"},
	//	{"../data/sounds/001_pcm24bit_2ch.wav"},
	//	{"../data/sounds/001_pcm32bit_2ch.wav"},
	//	{"../data/sounds/001_IEEEf32bit_2ch.wav"},
	//	//{"../data/sounds/001_IEEEf64bit_2ch.wav"},
	//};

	//alAudioBufferInfo inf;
	//for (int i = 0; i < 7; ++i)
	//{
	//	alLog::Print("FILE %s\n", files[i]);
	//	alAudio::GetAudioInfo(files[i], &inf);
	//}

	if(!g_demo->m_audio)
		g_demo->m_audio = alLib::InitializeAudio();
	if(!g_demo->m_audioMixer)
		g_demo->m_audioMixer = g_demo->m_audio->GetNewMixer();

	m_audioBuffer = g_demo->m_audio->LoadAudio("../data/sounds/lever1b.wav");

	m_audioObject = g_demo->m_audioMixer->GetNewAudioObject(m_audioBuffer);
	m_audioObject2 = g_demo->m_audioMixer->GetNewAudioObject(m_audioBuffer);

	//FILE* f = 0;
	//fopen_s(&f, "wave.raw", "wb");
	//if (f)
	//{
	//	fwrite(m_audioBuffer->m_rawData->m_data, 1, m_audioBuffer->m_rawData->m_dataSize, f);
	//	fclose(f);
	//}

	return true;
}

void DemoExample_audio_1::Shutdown()
{
	if(g_demo->m_audioMixer)
		g_demo->m_audioMixer->DeleteAllAudioObjects();
	AL_DESTROY(m_audioBuffer);

	g_demo->m_GUI->DeleteAllPanels();
	alLib::GetCursor(alCursorType::Arrow)->Activate();

	//g_demo->m_audio->StopAll();
}


bool DemoExample_audio_1::Run()
{
	g_demo->m_GUI->Update(*g_demo->m_dt);

//	Sleep(100);

	if (m_input->IsKeyHit(alInputKey::K_ESCAPE))
	{
		Shutdown();
		return false;
	}

	if (m_input->IsKeyHit(alInputKey::K_1))
	{
		if (m_audioObject)
			m_audioObject->Play();
	}
	if (m_input->IsKeyHit(alInputKey::K_Q))
	{
		if (m_audioObject2)
			m_audioObject2->Play();
	}
	if (m_input->IsKeyHit(alInputKey::K_2))
	{
		if (m_audioObject)
			m_audioObject->Reset();
	}
	if (m_input->IsKeyHit(alInputKey::K_3))
	{
		if (m_audioObject)
			m_audioObject->Pause();
	}


	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();

	m_gs->BeginDrawGUI();
	g_demo->m_GUI->Draw(*g_demo->m_dt);

	m_gs->SetScissorRect(alVec4f(0, 0, 
		(float32_t)g_demo->m_mainWindow->m_clientSize.x,
		(float32_t)g_demo->m_mainWindow->m_clientSize.y));
	
	m_gs->DrawText(
		U"Use keyboard keys 1,2,3,4",
		25,
		g_demo->m_guiFont,
		alVec2f(0, 0),
		ColorWhite);

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
	return true;
}