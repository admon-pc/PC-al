#include "al.h"
#include "System/alSystemWindow.h"
#include "GS/alGS.h"
#include "Classes/alColor.h"
#include "Input/alInput.h"

AL_LINK_LIBRARY(al);

class SystemWindowCallback;
class Editor
{
public:
	Editor();
	~Editor();

	bool Init();
	void Run();
	
	void OnRebuild();
	void OnDraw();
	void OnUpdate();

	void OnPopupCommand(uint32_t cmd);

	SystemWindowCallback* m_windowCallback = 0;
	alSystemWindow* m_mainWindow = 0;
	alGS* m_gs = 0;

	alGSTexture* m_whiteTexture = 0;
	alGUIFont* m_fontDefault = 0;

	bool m_run = true;
	uint32_t m_fps = 0;
};

class SystemWindowCallback : public alSystemWindowCallback
{
	Editor* m_app = 0;
public:
	SystemWindowCallback(Editor* dd) : m_app(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		m_app->OnRebuild();
	}

	virtual alVec2i OnGPUUpdateSize(alSystemWindow* w)
	{
		alVec2i s;
		s.x = w->m_clientSize.x / 2;
		s.y = w->m_clientSize.y / 2;
		return s;
	}

	virtual alVec2i OnMinMaxInfo(alSystemWindow* w)
	{
		return alVec2i(800, 600);
	}

	virtual void OnClose(alSystemWindow* window)
	{
		auto windowID = window->GetID();
		if (!windowID)
		{
			m_app->m_run = false;
		}
	}
	virtual void OnPopupCommand(uint32_t cmd)
	{
		m_app->OnPopupCommand(cmd);
	}
};

Editor::Editor()
{
	m_windowCallback = alCreate<SystemWindowCallback>(this);
}

Editor::~Editor()
{
	AL_DESTROY(m_gs);
	AL_DESTROY(m_windowCallback);
	AL_DESTROY(m_mainWindow);
}

bool Editor::Init()
{
	m_mainWindow = alLib::CreateSystemWindow(m_windowCallback);
	if (!m_mainWindow)
		return false;
	m_mainWindow->Show();
	m_gs = alLib::CreateGS(alVideoDriverType::Direct3D11);
	if (!m_gs->Init(m_mainWindow))
		return false;
	m_gs->SetClearColor(ColorDarkGrey);
	alLib::InitializeDefaultFont(m_gs);
	m_fontDefault = alLib::GetDefaultFont();
	OnRebuild();
	return true;
}

void Editor::Run()
{
	float32_t* dt = alLib::GetDeltaTime();
	alInput* input = alLib::GetInput();

	float64_t timer = 0.f;
	float64_t timer_limit = 1.0 / 60.0;

	float timer1Sec = 0.f;

	uint32_t fps = 0;
	while (m_run)
	{
		alLib::Update();
		timer += *dt;
		timer1Sec += *dt;
		OnUpdate();

		if (timer > timer_limit)
		{
			timer = 0.f;
			OnDraw();

			++fps;
		}

		if (timer1Sec > 1.f)
		{
			timer1Sec = 0.f;
			m_fps = fps;
			fps = 0;
		}
	}
}

void Editor::OnRebuild()
{
	if (m_gs)
		m_gs->UpdateWindowData();
}

void Editor::OnUpdate()
{
	alInput* input = alLib::GetInput();
	/*
	if (alMath::PointInRect(
		input->m_cursorCoordsForGUI.x,
		input->m_cursorCoordsForGUI.y,
		m_cellPanelRect))
	{
		if (input->m_wheelDelta &&
			input->m_kbm != alKeyboardModifier::Ctrl)
		{
			if (input->m_wheelDelta > 0.f)
				_moveUpView(1);
			if (input->m_wheelDelta < 0.f)
				_moveDownView(1);
		}
	}

	* */
}

void Editor::OnDraw()
{
	alInput* input = alLib::GetInput();
	char32_t char32Buf[100];

	m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();
	m_gs->BeginDrawGUI();
	m_gs->SetScissorRect(alVec4f(0.f, 0.f, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y));
	auto str_size = alLib::snprintf(char32Buf, 100, U"FPS:[%u], CURSOR COORDS:[%i %i]", m_fps, input->m_cursorCoords.x, input->m_cursorCoords.y);
	m_gs->DrawText(char32Buf, str_size, m_fontDefault,
		alVec2f(0, 0), ColorWhite);
	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
}

void Editor::OnPopupCommand(uint32_t cmd)
{
}

int main()
{
	alLib::InitializeLib();

	Editor* app = new Editor;
	if (app->Init())
	{
		app->Run();
	}

	delete app;

	return 1;
}
