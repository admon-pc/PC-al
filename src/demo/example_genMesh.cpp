#include "demo.h"
#include "example_genMesh.h"

#include "Geometry/alPopygonMesh.h"

#include "Image/alImage.h"

extern alDemo* g_demo;


bool DemoExample_genMesh::Init()
{
	m_gs = g_demo->m_gs;
	m_input = alLib::GetInput();

	auto img = alLib::LoadALImage("../data/demo/2.png");
	if (img)
	{
		m_texture = m_gs->CreateTexturePoint(img);
		AL_DESTROY(img);
	}

	alMat4 W;
	alPolygonMesh pmesh;
	pmesh.AddCube(1.f, W);
	pmesh.GenerateNormals(false);

	alMesh* mesh = pmesh.CreateMesh();
	if (mesh)
	{
		alGSMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_meshCube = m_gs->CreateMesh(&mi);
		AL_DESTROY(mesh);
	}

	pmesh.Clear();
	pmesh.AddSphere(32, 1.f, W);
	pmesh.GenerateNormals(true);
	mesh = pmesh.CreateMesh();
	if (mesh)
	{
		alGSMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_meshSphere = m_gs->CreateMesh(&mi);
		AL_DESTROY(mesh);
	}

	pmesh.Clear();

	alPolygonCreator pc;
	pc.SetPosition(alVec3f(0.f, 0.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(1.f, 0.f, 1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(1.f, 0.f, 0.f));
	pc.Add(false);
	pmesh.AddPolygon(&pc);
	pc.Clear();
	pc.SetPosition(alVec3f(-1.f, 0.f, 0.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(0.f, 0.f, -1.f));
	pc.Add(false);
	pc.SetPosition(alVec3f(-1.f, 0.f, -1.f));
	pc.Add(false);
	pmesh.AddPolygon(&pc);
	pc.Clear();
	pmesh.GenerateNormals(false);

	mesh = pmesh.CreateMesh();
	if (mesh)
	{
		alGSMeshInfo mi;
		mi.m_meshPtr = mesh;
		m_meshTriangles = m_gs->CreateMesh(&mi);
		AL_DESTROY(mesh);
	}

	m_GUIPanel = g_demo->m_GUI->GetNewPanel();
	m_GUIPanel->m_drawBG = false;
	m_GUIPanel->m_position.Set(0.f, 0.f);
	
	m_GUIPanel->m_size.Set(g_demo->m_mainWindow->m_clientSize.x,
		g_demo->m_mainWindow->m_clientSize.y);
	m_GUIPanel->Rebuild();

	return true;
}

void DemoExample_genMesh::Shutdown()
{
	AL_DESTROY(m_meshTriangles);
	AL_DESTROY(m_meshCube);
	AL_DESTROY(m_meshSphere);
	AL_DESTROY(m_texture);
	g_demo->m_GUI->DeleteAllPanels();
}


bool DemoExample_genMesh::Run()
{
	g_demo->m_GUI->Update(*g_demo->m_dt);


	if (m_input->IsKeyHit(alInputKey::K_ESCAPE))
	{
		Shutdown();
		return false;
	}

	alMat4 W, V, P;
	W.SetRotation(alQuaternion(0.f, alMath::DegToRad(m_angle), 0.f));
	m_angle += 20.1f * (*g_demo->m_dt);

	alVec3 cameraTarget(0, 0., 0);
	alVec3 cameraPos(0., 3., 3.0);
	alVec3 cameraDir = cameraTarget - cameraPos;
	cameraDir.Normalize2();
	float aspectRatio = (float32_t)g_demo->m_mainWindow->m_clientSize.x 
		/ (float32_t)g_demo->m_mainWindow->m_clientSize.y;

	alMath::LookAtRHMatrix(V, cameraPos, cameraTarget);
	alMath::PerspectiveRHMatrix(P, alMath::DegToRad(70.f), aspectRatio, 0.1f, 1000.f);

	g_demo->m_shaderSimple3D.m_cbVertexData.W = W;
	g_demo->m_shaderSimple3D.m_cbVertexData.WVP = P * V * W;
	g_demo->m_shaderSimple3D.m_cbVertexData.ViewDir.x = (float32_t)cameraDir.x;
	g_demo->m_shaderSimple3D.m_cbVertexData.ViewDir.y = (float32_t)cameraDir.y;
	g_demo->m_shaderSimple3D.m_cbVertexData.ViewDir.z = (float32_t)cameraDir.z;
	if(m_texture)
		g_demo->m_shaderSimple3D.m_texture1 = m_texture;
	m_gs->SetViewport(0, 0, g_demo->m_mainWindow->m_clientSize.x,
		g_demo->m_mainWindow->m_clientSize.y);
	m_gs->SetScissorRect(alVec4f(0, 0, (float32_t)g_demo->m_mainWindow->m_clientSize.x,
		(float32_t)g_demo->m_mainWindow->m_clientSize.y));

	m_gs->BeginDraw();
	m_gs->ClearAll();
	m_gs->SetShader(g_demo->m_shaderSimple3D.m_shader);
	m_gs->SetPrimitiveType(alGSPrimitiveType::Triangle);
	m_gs->SetRasterizationType(alGSRasterizationType::Solid);
	m_gs->EnableDepth();
	m_gs->SetMesh(m_meshCube);
	m_gs->Draw();

	W.SetTranslation(alVec4f(2.f,0.f,0.f,0.f));
	g_demo->m_shaderSimple3D.m_cbVertexData.W = W;
	g_demo->m_shaderSimple3D.m_cbVertexData.WVP = P * V * W;
	m_gs->SetMesh(m_meshSphere);
	m_gs->Draw();

	W.SetTranslation(alVec4f(-2.f, 0.f, 0.f, 0.f));
	g_demo->m_shaderSimple3D.m_cbVertexData.W = W;
	g_demo->m_shaderSimple3D.m_cbVertexData.WVP = P * V * W;
	m_gs->SetMesh(m_meshTriangles);
	m_gs->Draw();

	m_gs->EndDraw();

	m_gs->BeginDrawGUI();
	g_demo->m_GUI->Draw(*g_demo->m_dt);

	m_gs->SetScissorRect(alVec4f(0, 0, 
		(float32_t)g_demo->m_mainWindow->m_clientSize.x,
		(float32_t)g_demo->m_mainWindow->m_clientSize.y));
	/*m_gs->DrawText(
		U"I can draw text without creating GUI object",
		43,
		g_demo->m_guiFont,
		alVec2f(0, 40),
		ColorBlue);*/

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
	return true;
}