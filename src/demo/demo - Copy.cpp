#include "al.h"

#include "eastdc/EASprintf.h"

#include "Archive/alArchive.h"
#include "Image/alImage.h"
#include "System/alSystemWindow.h"
#include "GS/alGS.h"
#include "Input/alInput.h"
#include "Classes/alColor.h"

#include "GUI/alGUI.h"

#include "Geometry/alTriangle.h"
#include "Geometry/alPopygonMesh.h"
#include "Geometry/alMeshLoader.h"

#include <Windows.h>

#ifdef LoadImage
#undef LoadImage
#endif


AL_LINK_LIBRARY(al);

#define DEMO_WINDOWID_SECONDWINDOW 1
class Demo;
class SystemWindowCallback;

class TestTextObject : public alGUIText
{
public:
	TestTextObject(alGUIContext* ct):alGUIText(ct){}
	~TestTextObject() {}

	virtual void OnMouseEnter(alGUIElement* e){}
	virtual void OnMouseLeave(alGUIElement* e)	{	}
	virtual void OnMouseMove(alGUIElement* e)	{	}
	virtual void OnLMBHit(alGUIElement*) 	{	}
	virtual void OnRMBHit(alGUIElement*) 	{	}
	virtual void OnMMBHit(alGUIElement*) 	{	}
};
class TestButton : public alGUIButton
{
public:
	TestButton(alGUIContext* ct) :alGUIButton(ct) {}
	~TestButton() {}

	// alGUIElement
	virtual void OnMouseEnter(alGUIElement* e) {}
	virtual void OnMouseLeave(alGUIElement* e) {}
	virtual void OnMouseMove(alGUIElement* e) {	}
	virtual void OnLMBHit(alGUIElement*) {	}
	virtual void OnRMBHit(alGUIElement*) {	}
	virtual void OnMMBHit(alGUIElement*) {	}

	// alGUIButton
	virtual void OnButtonRelease(alGUIButton*) 
	{
		printf("On Button\n");
	}
};

class MeshLoaderCallback : public alMeshLoaderCallback
{
	alGS* m_gs = 0;
public:
	MeshLoaderCallback(alGS*_gs):m_gs(_gs) {}
	virtual ~MeshLoaderCallback()
	{
		for (size_t i = 0; i < m_GSMesh.m_size; ++i)
		{
			alDestroy(m_GSMesh.m_data[i]);
		}
	}

	virtual void OnMesh(alMesh* m)
	{
		alGSMeshInfo mi;
		mi.m_meshPtr = m;
		auto gsm = m_gs->CreateMesh(&mi);
		if (gsm)
		{
			m_GSMesh.push_back(gsm);
		}
		alDestroy(m);
	}


	void Load(const char* file)
	{
		alLib::LoadMesh(file, this);
	}
	alArray<alGSMesh*> m_GSMesh;
};

class Shader_Simple3D : public alGSShaderCallback
{
	Demo* m_app = 0;
public:
	Shader_Simple3D(Demo* app) : m_app(app){}
	virtual ~Shader_Simple3D() {}

	bool Create();
	alGSShaderConstantBuffer* m_constantBuffer = 0;
	alGSShader* m_shader = 0;

	struct cbVertex
	{
		alMat4 WVP;
		alMat4 W;
		alVec4f ViewDir;
	}
	m_cbVertexData;
	
	virtual void OnSetConstants() override;
};


class Demo
{
public:
	Demo();
	~Demo();

	bool Init();
	void Run();

	bool m_run = true;
	SystemWindowCallback* m_windowCallback = 0;
	alSystemWindow* m_mainWindow = 0;
	alSystemWindow* m_secondWindow = 0;

	alGS* m_gs = 0;

	Shader_Simple3D m_shaderSimple3D = Shader_Simple3D(this);

	alMat4 m_V;
	alMat4 m_P;

	alGSTexture* m_whiteTexture = 0;
	alGSTexture* m_imageTexture = 0;
	alGSTexture* m_spheremapTexture = 0;

	alGUIContext* m_GUI = 0;
	alGUIFont* m_guiFont = 0;
	alGUIPanel* m_GUIPanel = 0;

	TestTextObject* m_testText = 0;
	TestButton* m_testButton = 0;
	MeshLoaderCallback * m_loadedMesh = 0;
};

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

void Shader_Simple3D::OnSetConstants()
{
	m_constantBuffer->MapData(&m_cbVertexData, sizeof(m_cbVertexData));
	m_constantBuffer->VSSetConstantBuffers(0);
	m_constantBuffer->PSSetConstantBuffers(0);
	m_shader->SetTexture(m_app->m_imageTexture, 0);
	m_shader->SetTexture(m_app->m_spheremapTexture, 1);
}

class SystemWindowCallback : public alSystemWindowCallback
{
	Demo* m_demo = 0;
public:
	SystemWindowCallback(Demo* dd) : m_demo(dd) {}
	virtual ~SystemWindowCallback() {}

	virtual void OnSizeChanged(alSystemWindow*)
	{
		if (m_demo->m_gs)
		{
			m_demo->m_gs->UpdateWindowData();
		}
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
		auto windowID = w->GetID();
		if (windowID == DEMO_WINDOWID_SECONDWINDOW)
		{
			return alVec2i(100, 20);
		}
		return alVec2i(800, 600); 
	}

	virtual void OnClose(alSystemWindow* window)
	{
		auto windowID = window->GetID();
		if (!windowID)
		{
			m_demo->m_run = false;
		}
		else if (windowID == DEMO_WINDOWID_SECONDWINDOW)
		{
			window->Hide();
		}
	}
};


Demo::Demo()
{
	m_windowCallback = alCreate<SystemWindowCallback>(this);
}

Demo::~Demo()
{
	AL_DESTROY(m_testButton);
	AL_DESTROY(m_guiFont);
	AL_DESTROY(m_testText);
	AL_DESTROY(m_loadedMesh);
	AL_DESTROY(m_GUI);
	AL_DESTROY(m_spheremapTexture);
	AL_DESTROY(m_imageTexture);
	AL_DESTROY(m_gs);
	AL_DESTROY(m_windowCallback);
	AL_DESTROY(m_mainWindow);
}

bool Demo::Init()
{
	m_mainWindow = alLib::CreateSystemWindow(m_windowCallback);
	if (!m_mainWindow)
		return false;


	//m_secondWindow = alLib::CreateSystemWindow(m_windowCallback);
	//m_secondWindow->SetID(DEMO_WINDOWID_SECONDWINDOW);
	//m_secondWindow->Show();
	//m_secondWindow->SetTitle("Second window");
	//m_secondWindow->SetPositionAndSize(0, 0, 200, 100);

	m_mainWindow->Show();

	m_gs = alLib::CreateGS(alVideoDriverType::Direct3D11);
	if (!m_gs->Init(m_mainWindow))
		return false;
	alLib::InitializeDefaultFont(m_gs);
	//m_gs->SetClearColor(ColorWhite)
	
	m_loadedMesh = alCreate<MeshLoaderCallback>(m_gs);
	m_loadedMesh->Load("../data/models/barrel01.obj");


	m_whiteTexture = m_gs->GetWhiteTexture();
	alImage* image = alLib::LoadImage("../data/textures/models/barrel01.png");
	if (!image)
		return false;

	m_imageTexture = m_gs->CreateTextureLinear(image, false);

	image = alLib::LoadImage("../data/spheremap.jpg");
	if (!image)
		return false;
	m_spheremapTexture = m_gs->CreateTextureLinear(image, true);

	if (!m_shaderSimple3D.Create())
		return false;

	m_GUI = alLib::CreateGUIContext(m_mainWindow, m_gs);

	//auto archiveFile = alArchive::LoadZip("z.zip");
	//auto fastlz_c = archiveFile->GetFile("FastLZ-master/fastlz.c");
	//auto changelog = archiveFile->GetFile("FastLZ-master/changelog");
	//AL_DESTROY(archiveFile);
	//AL_DESTROY(changelog);
	//AL_DESTROY(fastlz_c);

	m_guiFont = alLib::CreateGUIFont();
	m_guiFont->Load("font.zip", m_gs);
		//m_GUI->LoadFont();

	m_testText = alCreate<TestTextObject>(m_GUI);
	m_testText->SetText(U"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	m_testText->SetFont(m_guiFont);
	m_testText->SetID(1);
	m_testButton = alCreate<TestButton>(m_GUI);
	m_testButton->m_position.Set(10.f, 100.f);
	m_testButton->m_size.Set(210.f,40.f);
//	m_testButton->m_disabled = true;
	m_testButton->SetFont(m_guiFont);
	m_testButton->SetText(U"button");
	m_testButton->m_lerpColors = true;
	m_testButton->m_roundCorners = true;
	m_testButton->m_roundSize = 3;
	m_testButton->m_roundIterations = 1;
	m_testButton->m_borderSize = 1;
	m_GUIPanel = m_GUI->GetNewPanel();
	m_GUIPanel->m_drawBG = false;
	m_GUIPanel->m_position.Set(0.f, 0.f);
	m_GUIPanel->m_size.Set(600.f, 600.f);
	m_GUIPanel->AddElement(m_testText);
	m_GUIPanel->AddElement(m_testButton);
	m_GUIPanel->Rebuild();
	m_GUIPanel->SetID(0);

	return true;
}

void Demo::Run()
{
	alMat4 cubeMatrix;
	alMat4 cubeMatrix_scale;
	alMat4 cubeMatrix_position;
	alMath::ScaleMatrix(cubeMatrix_scale, alVec3(1.001f, 1.001f, 1.0f));
	alMath::TranslationMatrix(cubeMatrix_position, alVec3(0.f, 0.f, 0.f));
	alPolygonMesh pmesh;
	cubeMatrix = cubeMatrix_scale * cubeMatrix_position;/*
	pmesh.AddCube(1.f, cubeMatrix);
	alMath::TranslationMatrix(cubeMatrix_position, alVec3(0.f, 0.f, 0.f));
	cubeMatrix = cubeMatrix_scale * cubeMatrix_position;
	pmesh.AddCube(1.f, cubeMatrix);*/

	pmesh.AddSphere(8, 1.5f, cubeMatrix);
	pmesh.GenerateNormals(true);

	alGSTextureCache textureCache(m_gs);
	auto textureNode = textureCache.GetTexture("../data/image_test/it.png", false);
	textureCache.Load(textureNode);

	alMesh* mesh = pmesh.CreateMesh();

	//mesh->PrintAllInfo(true);
	alGSMeshInfo minf;
	minf.m_meshPtr = mesh;

	alGSMesh* model = m_gs->CreateMesh(&minf);
	alDestroy(mesh);


	alMat4 W;


	float32_t* dt = alLib::GetDeltaTime();
	float32_t angle = 0.f;

	alVec3 cameraPos(1.5, 1., 1.0);
	alVec2f rectPos(0, 0);
	
	alInput* input = alLib::GetInput();
	while (m_run)
	{
		alLib::Update();
		m_GUI->Update(*dt);
		//m_testText->Update();

		float aspectRatio = (float32_t)m_mainWindow->m_clientSize.x / (float32_t)m_mainWindow->m_clientSize.y;
	//	Sleep(10);
		W.SetRotation(alQuaternion(0.f, alMath::DegToRad(angle), 0.f));

		if (input->IsKeyHold(alInputKey::K_UP))
		{
			//cameraPos.y -= 10.f * (*dt);
			float32_t v = -100.f * (*dt);
			m_GUIPanel->m_position.y += v;
			m_GUIPanel->Rebuild();
		}
		if (input->IsKeyHold(alInputKey::K_DOWN))
		{
			//cameraPos.y += 10.f * (*dt);
			float32_t v = 100.f * (*dt);
			m_GUIPanel->m_position.y += v;
			m_GUIPanel->Rebuild();
		}
		if (input->IsKeyHold(alInputKey::K_LEFT))
		{
			cameraPos.x -= 10.f * (*dt);
		}
		if (input->IsKeyHold(alInputKey::K_RIGHT))
		{
			cameraPos.x += 10.f * (*dt);
		}

		
		m_V.Identity();
		m_P.Identity();

		alVec3 cameraTarget(0, 0., 0);
		alVec3 cameraDir = cameraTarget - cameraPos;
		cameraDir.Normalize2();

		alMath::LookAtRHMatrix(m_V, cameraPos, cameraTarget);
		alMath::PerspectiveRHMatrix(m_P, alMath::DegToRad(70.f), aspectRatio, 0.1f, 1000.f);

		m_shaderSimple3D.m_cbVertexData.W = W;
		m_shaderSimple3D.m_cbVertexData.WVP = m_P * m_V * W;
		m_shaderSimple3D.m_cbVertexData.ViewDir.x = (float32_t)cameraDir.x;
		m_shaderSimple3D.m_cbVertexData.ViewDir.y = (float32_t)cameraDir.y;
		m_shaderSimple3D.m_cbVertexData.ViewDir.z = (float32_t)cameraDir.z;

		m_gs->SetViewport(0, 0, m_mainWindow->m_clientSize.x, m_mainWindow->m_clientSize.y);
		m_gs->SetScissorRect(alVec4f(0, 0, (float32_t)m_mainWindow->m_clientSize.x, (float32_t)m_mainWindow->m_clientSize.y));

		m_gs->BeginDraw();
		m_gs->ClearAll();

		m_gs->SetShader(m_shaderSimple3D.m_shader);
		m_gs->SetPrimitiveType(alGSPrimitiveType::Triangle);
		m_gs->SetRasterizationType(alGSRasterizationType::Solid);
		m_gs->EnableDepth();
		m_gs->SetMesh(model);
		//m_gs->Draw();
		for (size_t i = 0; i < m_loadedMesh->m_GSMesh.m_size; ++i)
		{
			m_gs->SetMesh(m_loadedMesh->m_GSMesh.m_data[i]);
			m_gs->Draw();
		}
		m_gs->EndDraw();

		alGSTexture* guiTexture = m_whiteTexture;
		if (textureNode)
			guiTexture = textureNode->m_texture;
		

		m_gs->BeginDrawGUI();
	/*	m_gs->DrawRectangle(alVec4f(rectPos.x, rectPos.y,
			rectPos.x + 212, rectPos.y + 212),
			ColorWhite, ColorWhite, guiTexture);*/
		m_GUI->Draw(*dt);


		m_testText->Draw(0.f);
		m_gs->EndDrawGUI();
		m_gs->SwapBuffers();

		angle += 20.1f * (*dt);
	}

}


int main()
{
	alLib::InitializeLib();

	alUnicodeConverter uc;
	uc.Set(u'Ж'); // 2bytes
	uc.Set(U'ࠍ');  // 3
	uc.Set(U'𐐷');
	uc.Set(U'𐌎');

	{
		
	}

	{
		alVec4_t<uint32_t> chars(0xe0, 0xa0, 0x8d, 0x0);
		uc.Set(chars);

		wchar_t w = uc.m_16[0];
	//	printf("a");
	}
	{
		alUnicodeString ustr(u8"HI ДА\n");
		ustr.Append(u"UTF16 ОПР\n");
		ustr.AppendFloat(PIPIf);
		
		ustr.Clear();
		ustr.Append(PI);
		auto pif = ustr.ToFloat();
		auto pi = ustr.ToFloat64();

		//printf("a");
	}
	{
		alUnicodeString ustr;
		ustr.ReadFromFile("1.txt");
		//printf("a");
	}
	Demo* dd = new Demo;
	if (dd->Init())
	{
		dd->Run();
	}

	return 1;
}
