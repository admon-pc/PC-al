#include "demo.h"
#include "example_gui_all.h"
#include "example_gui_all2.h"
#include "example_genMesh.h"

#include "System/alSystemWindowWin32.h"

AL_LINK_LIBRARY(al);

alDemo* g_demo = 0;

Shader_Simple3D::~Shader_Simple3D()
{
	AL_DESTROY(m_shader);
}

void Shader_Simple3D::OnSetConstants()
{
	m_constantBuffer->MapData(&m_cbVertexData, sizeof(m_cbVertexData));
	m_constantBuffer->VSSetConstantBuffers(0);
	m_constantBuffer->PSSetConstantBuffers(0);
	if(m_texture1)
		m_shader->SetTexture(m_texture1, 0);
}

bool Shader_Simple3D::Create()
{
	alGSShaderCreationInfo inf;
	inf.m_callback = this;
	inf.m_vertexType = alMeshVertexType::Triangle;
	//inf.m_vertexType = alMeshVertexType::Point;
	//inf.m_saveShaderToFile_VS = "../data/shaders/d3d11/ScreenQuad.vs";
	//inf.m_saveShaderToFile_PS = "../data/shaders/d3d11/ScreenQuad.ps";
	//inf.m_saveShaderToFile_GS = "../data/shaders/d3d11/ScreenQuad.gs";
	inf.m_shaderEntry_VS = "VSMain";
	inf.m_shaderEntry_PS = "PSMain";
	inf.m_shaderFile_VS = "../data/shaders/d3d11/Simple3D.hlsl";
	inf.m_shaderFile_PS = "../data/shaders/d3d11/Simple3D.hlsl";
	inf.m_shaderModel_VS = "vs_5_0";
	inf.m_shaderModel_PS = "ps_5_0";

	m_shader = m_app->m_gs->CreateShader(inf);
	if (!m_shader)
		return false;

	m_constantBuffer = m_shader->CreateConstantBuffer(sizeof(m_cbVertexData));

	return true;
}


alDemoDirectory* alDemoDirectory::GetDirByName(const char32_t* catName)
{
	alDemoDirectory* dir = 0;
	for (size_t i = 0; i < m_entries.m_size; ++i)
	{
		if (!alLib::StringStrcmp(catName, m_entries.m_data[i]->m_name.c_str()))
		{
			if (!m_entries.m_data[i]->m_example)
			{
				dir = m_entries.m_data[i];
				break;
			}
		}
	}

	if (!dir)
	{
		dir = new alDemoDirectory;

		dir->m_name = catName;
		dir->m_parent = this;
		m_entries.push_back(dir);

		g_demo->m_allDirectories.push_back(dir);
	}

	return dir;
}

void alDemoDirectory::AddExample(
	alDemoExample* ex)
{
	alDemoDirectory* dir = new alDemoDirectory;
	dir->m_example = ex;
	dir->m_parent = this;

	m_entries.push_back(dir);
	g_demo->m_allDirectories.push_back(dir);
}

class SystemWindowCallback : public alSystemWindowCallback
{
	alDemo* m_demo = 0;
public:
	SystemWindowCallback(alDemo* dd) : m_demo(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		m_demo->OnWindowSizeChanged();
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
		/*auto windowID = w->GetID();
		if (windowID == DEMO_WINDOWID_SECONDWINDOW)
		{
			return alVec2i(100, 20);
		}*/
		return alVec2i(800, 600);
	}

	virtual void OnClose(alSystemWindow* window)
	{
		auto windowID = window->GetID();
		if (!windowID)
		{
			m_demo->m_run = false;
		}
		/*else if (windowID == DEMO_WINDOWID_SECONDWINDOW)
		{
			window->Hide();
		}*/
	}

	virtual void OnPopupCommand(uint32_t cmd) 
	{
		g_demo->OnPopupCommand(cmd);
	}

};

alDemo::alDemo()
{
	g_demo = this;
	m_currDir = &m_mainDemoDir;
}

alDemo::~alDemo()
{
	for (size_t i = 0; i < m_allDirectories.m_size; ++i)
	{
		delete m_allDirectories.m_data[i];
	}
	for (size_t i = 0; i < m_examples.m_size; ++i)
	{
		delete m_examples.m_data[i];
	}

	AL_DESTROY(m_GUI);
	AL_DESTROY(m_guiFont);
	AL_DESTROY(m_gs);
	AL_DESTROY(m_windowCallback);
	AL_DESTROY(m_mainWindow);
}

bool alDemo::Init()
{
	m_windowCallback = alCreate<SystemWindowCallback>(this);
	m_mainWindow = alLib::CreateSystemWindow(m_windowCallback);
	if (!m_mainWindow)
		return false;
	m_mainWindow->SetTitle("Demo");
	m_mainWindow->Show();

	m_gs = alLib::CreateGS(alVideoDriverType::Direct3D11);
	if (!m_gs->Init(m_mainWindow))
		return false;

	if (!m_shaderSimple3D.Create())
		return false;
	m_shaderSimple3D.m_texture1 = m_gs->GetWhiteTexture();
	alLib::InitializeDefaultFont(m_gs);
	m_guiFont = alLib::CreateGUIFont();
	m_guiFont->Load("font.zip", m_gs);
	m_guiFontMini = alLib::GetDefaultFont();
	m_defaultFontMini = alLib::GetDefaultFont();

	m_GUI = alLib::CreateGUIContext(m_mainWindow, m_gs);
	m_dt = alLib::GetDeltaTime();
	m_input = alLib::GetInput();

	AddExample("GUI/", new DemoExample_gui_all(U"GUI 1", U"Part 1"));
	AddExample("GUI/", new DemoExample_gui_all2(U"GUI 2", U"Part 2"));
	AddExample("/", new DemoExample_genMesh(U"Mesh generator", U"..."));
	UpdatePathText();

	return true;
}

void alDemo::Run_demo()
{
	if (m_input->IsKeyHit(alInputKey::K_UP))
	{
		if (!m_currDir->m_currEntry)
			m_currDir->m_currEntry = m_currDir->m_entries.m_size - 1;
		else
			--m_currDir->m_currEntry;
	}
	if (m_input->IsKeyHit(alInputKey::K_DOWN))
	{
		++m_currDir->m_currEntry;
		if (m_currDir->m_currEntry == m_currDir->m_entries.m_size)
			m_currDir->m_currEntry = 0;
	}
	if (m_input->IsKeyHit(alInputKey::K_ENTER)
		|| m_input->IsKeyHit(alInputKey::K_RIGHT))
	{
		if (m_currDir->m_entries.m_size)
		{
			if (m_currDir->m_entries[m_currDir->m_currEntry]->m_example)
			{
				auto e = m_currDir->m_entries[m_currDir->m_currEntry]->m_example;
				if (e->Init())
				{
					m_currExample = e;
				}
			}
			else
			{
				m_currDir = m_currDir->m_entries[m_currDir->m_currEntry];
				UpdatePathText();
			}
		}
	}
	if (m_input->IsKeyHit(alInputKey::K_ESCAPE)
		|| m_input->IsKeyHit(alInputKey::K_LEFT))
	{
		if (m_currDir->m_parent)
		{
			m_currDir = m_currDir->m_parent;
		}
		UpdatePathText();
	}

	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->EndDraw();

	m_gs->BeginDrawGUI();
	float32_t posY = 0.f;

	auto font = m_guiFont;
	for (size_t i = 0; i < m_currDir->m_entries.m_size; ++i)
	{
		auto ex = m_currDir->m_entries.m_data[i]->m_example;

		if (ex)
		{
			if (ex->m_title.Size())
			{
				alColor textColor = ColorWhite;
				if (m_currDir->m_currEntry == i)
				{
					textColor = ColorYellow;
				}
				alVec2f ep;//test
				m_gs->DrawText(
					ex->m_title.c_str(),
					ex->m_title.Size(),
					font,
					alVec2f(0.f, posY),
					textColor,
					&ep);

				m_gs->DrawText(
					ex->m_description.c_str(),
					ex->m_description.Size(),
					font,
					alVec2f(300, posY),
					textColor);
				posY += font->m_maxHeight;
			}
		}
		else
		{
			if (m_currDir->m_entries.m_data[i]->m_name.Size())
			{
				alColor textColor = ColorWhite;
				if (m_currDir->m_currEntry == i)
				{
					textColor = ColorYellow;
				}

				m_directoryNameDrawString.Clear();
				m_directoryNameDrawString.Append(U"[",1);
				m_directoryNameDrawString.Append(
					m_currDir->m_entries.m_data[i]->m_name.c_str(),
					m_currDir->m_entries.m_data[i]->m_name.Size());
				m_directoryNameDrawString.Append(U"]", 1);

				m_gs->DrawText(
					m_directoryNameDrawString.c_str(),
					m_directoryNameDrawString.Size(),
					font,
					alVec2f(0.f, posY),
					textColor);
			
				posY += font->m_maxHeight;
			}
		}
	}

	m_gs->DrawText(
		m_pathText.c_str(),
		m_pathText.Size(),
		font,
		alVec2f(0.f, m_mainWindow->m_clientSize.y - font->m_maxHeight),
		ColorWhite);
	m_gs->EndDrawGUI();
	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

void alDemo::Run()
{
	while (m_run)
	{
		alLib::Update();

		if (m_currExample)
		{
			if (!m_currExample->Run())
			{
				m_currExample = 0;
			}
		}
		else
		{
			Run_demo();
		}
	}
}

void alDemo::AddExample(
	const char* path, 
	alDemoExample* example)
{
	alArray<alStringA> words;
	alLib::StringGetWords(path, &words);
	
	alDemoDirectory* dir = &m_mainDemoDir;

	alUnicodeString ustr;

	if (words.m_size)
	{
		for (size_t i = 0; i < words.m_size; ++i)
		{
			ustr.Assign(words.m_data[i].c_str());

			dir = dir->GetDirByName(ustr.c_str());
			//printf("%s\n", words.m_data[i]);
		}

		m_allExamples.push_back(example);
		dir->AddExample(example);
	}
	else
	{
		m_allExamples.push_back(example);
		dir->AddExample(example);
	}
	//m_examples.push_back(example);
}

void alDemo::UpdatePathText()
{
	m_pathText.Clear();
	m_pathText.Append(U"/");
	alUnicodeString tmpstr;
	if (m_currDir->m_parent)
	{
		auto dir = m_currDir;
		while (true)
		{
			tmpstr.Assign(dir->m_name);
			tmpstr.Flip();
			m_pathText.Append(tmpstr);
			m_pathText.Append(U"/");

			dir = dir->m_parent;
			if (!dir->m_parent)
				break;
		}
	}
	m_pathText.Flip();
}

void alDemo::OnPopupCommand(uint32_t cmd)
{
	if (cmd >= 100)
	{
		if (m_currExample)
			m_currExample->OnPopupCommand(cmd);
	}
}

void alDemo::OnWindowSizeChanged()
{
	if (m_gs)
	{
		m_gs->UpdateWindowData();
	}

	if (m_currExample)
	{
		m_currExample->OnWindowSizeChanged();
	}
}

int main(int argc, char* argv[])
{
	alLib::InitializeLib();

	alDemo* dd = new alDemo;
	if (dd->Init())
	{
		dd->Run();
	}
	return 1;
}



