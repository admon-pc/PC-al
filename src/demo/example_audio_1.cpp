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

	//m_audioBuffer = g_demo->m_audio->LoadAudio("../data/sounds/lever1e.wav");

	/*m_audioObject = g_demo->m_audioMixer->GetNewAudioObject(m_audioBuffer);
	m_audioObject->m_loop = -1;
	m_audioObject2 = g_demo->m_audioMixer->GetNewAudioObject(m_audioBuffer);*/

	/*FILE* f = 0;
	fopen_s(&f, "wave.raw", "wb");
	if (f)
	{
		fwrite(m_audioBuffer->m_rawData->m_data, 1, m_audioBuffer->m_rawData->m_dataSize, f);
		fclose(f);
	}*/
	m_dirFiles.clear();
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator("../data/sounds/"))
		{
			if (entry.is_regular_file())
			{
				auto p = entry.path();
				auto e = p.extension();
				auto e_str = e.generic_string();
				if (strcmp(e_str.c_str(), ".wav") == 0)
				{
					alAudioBufferInfo inf;
					alAudio::GetAudioInfo(p.generic_string().c_str(), &inf);
					if (inf.m_format != alAudioFormat::Unknown
						&& (inf.m_additionalInfo.m_length <= 30.f ))
					{
						auto audioBuffer = g_demo->m_audio->LoadAudio(p.generic_string().c_str());
						if (audioBuffer)
						{
							directory_files o;
							memset(&o, 0, sizeof(o));
							alLib::sprintf(o.m_name, U"%s", p.filename().generic_u32string().c_str());
							m_dirFiles.push_back(o);
							
							_audio_data ad;
							ad.m_buffer = audioBuffer;
							ad.m_audioObject = g_demo->m_audioMixer->GetNewAudioObject(audioBuffer);
							m_audioBuffers.push_back(ad);
						}
					}
				}
			}
		}
	}
	m_combo1 = alCreate<_combo>(g_demo->m_GUI, alVec2f(0, 30), alVec2f(350, 15));
	m_combo1->SetUserData(this);
	m_combo1->SetFont(g_demo->m_guiFont);
	m_combo1->m_text.Assign(U"...");
	m_combo1->SetItems(m_dirFiles.m_data, m_dirFiles.size(),
		sizeof(directory_files), 20);

	m_GUIPanel = g_demo->m_GUI->GetNewPanel(alVec2f(0.f, 0.f),
		alVec2f(g_demo->m_mainWindow->m_clientSize.x,
			g_demo->m_mainWindow->m_clientSize.y));
	m_GUIPanel->m_drawBG = false;
	m_GUIPanel->AddElement(m_combo1);

	m_GUIPanel->Rebuild();

	return true;
}

void DemoExample_audio_1::Shutdown()
{
	if(g_demo->m_audioMixer)
		g_demo->m_audioMixer->DeleteAllAudioObjects();

	for (auto o : m_audioBuffers)
	{
		AL_DESTROY(o.m_buffer);
	}

	AL_DESTROY(m_combo1);
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

	if (m_audioObject)
	{
		auto inf = m_audioObject->GetBuffer()->m_rawData->m_bufferInfo;
		
		static char32_t buf[100];
		auto sz = alLib::sprintf(buf, U"Channels: %u", inf.m_channels);

		m_gs->DrawText(
			buf,
			sz,
			g_demo->m_guiFont,
			alVec2f(400, 60),
			ColorWhite);

		sz = alLib::sprintf(buf, U"Length: %f", inf.m_additionalInfo.m_length);

		m_gs->DrawText(
			buf,
			sz,
			g_demo->m_guiFont,
			alVec2f(400, 75),
			ColorWhite);
	}

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
	return true;
}

void DemoExample_audio_1::_combo::OnComboSelectItem(size_t i)
{
	DemoExample_audio_1* demo = (DemoExample_audio_1*)GetUserData();
	if (demo)
	{
		demo->m_audioObject = demo->m_audioBuffers.m_data[i].m_audioObject;

		uint8_t* ptr = (uint8_t*)m_items;
		m_text = (char32_t*)(&ptr[i * m_stride] + m_textOffset);
	}
}