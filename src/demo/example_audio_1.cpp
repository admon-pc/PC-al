#include "demo.h"
#include "example_audio_1.h"

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
	m_audio = alLib::InitializeAudio();

	return true;
}

void DemoExample_audio_1::Shutdown()
{
	g_demo->m_GUI->DeleteAllPanels();
	alLib::GetCursor(alCursorType::Arrow)->Activate();
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