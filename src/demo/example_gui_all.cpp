#include "demo.h"
#include "example_gui_all.h"

#include "Image/alImage.h"

extern alDemo* g_demo;

void DemoExample_gui_all_testTextObject::OnMouseEnter()
{
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_text.Assign(U"OnMouseEnter");
	}break;
	}
}

void DemoExample_gui_all_testTextObject::OnMouseLeave()
{
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_text.Assign(U"OnMouseLeave");
	}break;
	}
}
void DemoExample_gui_all_testTextObject::OnMouseMove() {
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_mouseMoved = true;
		example->m_textObject2_mouseMoved_timer = 0.f;
	}break;
	}
}
void DemoExample_gui_all_testTextObject::OnLMBHit() {
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_text.Assign(U"OnLMBHit");
	}break;
	}
}
void DemoExample_gui_all_testTextObject::OnRMBHit() {
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_text.Assign(U"OnRMBHit");
	}break;
	}
}
void DemoExample_gui_all_testTextObject::OnMMBHit() {
	auto id = GetID();
	switch (id)
	{
	case 1:
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_textObject2_text.Assign(U"OnMMBHit");
	}break;
	}
}

void DemoExample_gui_all_pictureBox::OnLMBHit()
{
	printf("...lmb");

	if (m_texture)
	{
		auto input = alLib::GetInput();
		
		float32_t clickPointX = input->m_cursorCoordsForGUI.x - m_pictureBoxBuildRect.x;
		float32_t clickPointY = input->m_cursorCoordsForGUI.y - m_pictureBoxBuildRect.y;

		m_clickedPosition.Clear();
		m_clickedPosition.Append(U"Clicked position: ");
		m_clickedPosition.Append((uint32_t)clickPointX);
		m_clickedPosition.Append(U" ; ");
		m_clickedPosition.Append((uint32_t)clickPointY);

		if (m_zoom)
		{
			m_uv.Set(0.f,0.f,1.f,1.f);
			m_zoom = false;
		}
		else
		{
			alVec4i rectangle;
			rectangle.x = (int32_t)clickPointX;
			rectangle.y = (int32_t)clickPointY;

			rectangle.x -= 16;
			rectangle.y -= 16;

			if (rectangle.x < 0)
				rectangle.x = 0;
			if (rectangle.y < 0)
				rectangle.y = 0;

			int32_t textureWidth = m_texture->GetWidth();
			int32_t textureHeight = m_texture->GetHeight();

			if (rectangle.x > textureWidth - 32)
				rectangle.x = textureWidth - 32;
			if (rectangle.y > textureHeight - 32)
				rectangle.y = textureHeight - 32;

			rectangle.z = rectangle.x + 32;
			rectangle.w = rectangle.y + 32;

			//printf("%f %f\n", clickPointX, clickPointY);
			auto uv1 = m_texture->GetUV(rectangle.x, rectangle.y);
			auto uv2 = m_texture->GetUV(rectangle.z, rectangle.w);
			
			m_uv.x = uv1.x;
			m_uv.y = uv1.y;
			m_uv.z = uv2.x;
			m_uv.w = uv2.y;

			m_zoom = true;
		}
	}
}

void DemoExample_gui_all_TestButton::OnButtonRelease()
{
	if (GetID() == 1)
	{
		DemoExample_gui_all* example = (DemoExample_gui_all*)GetUserData();
		example->m_buttonExitPressed = true;
	}
}

bool DemoExample_gui_all::Init()
{
	m_gs = g_demo->m_gs;
	m_input = alLib::GetInput();

	auto img = alLib::LoadALImage("../data/demo/1.png");
	if (img)
	{
		m_texture = m_gs->CreateTexturePoint(img);
		AL_DESTROY(img);
	}

	m_textObject1 = alCreate<DemoExample_gui_all_testTextObject>(g_demo->m_GUI);
	m_textObject1->SetText(U"This is text object");
	m_textObject1->SetFont(g_demo->m_guiFont);
	m_textObject2minifont = alCreate<DemoExample_gui_all_testTextObject>(g_demo->m_GUI);
	m_textObject2minifont->SetText(U"This is text object with small default font");
	m_textObject2minifont->SetFont(g_demo->m_guiFontMini);
	m_textObject2minifont->m_position.y = g_demo->m_guiFont->m_maxHeight;

	m_textObject2 = alCreate<DemoExample_gui_all_testTextObject>(g_demo->m_GUI);
	m_textObject2->SetText(U"Text object");
	m_textObject2->SetFont(g_demo->m_guiFont);
	m_textObject2->SetID(1);
	m_textObject2->m_position.y = 240;
	m_textObject2->SetUserData(this);

	if (m_texture)
	{
		m_pictureBox = alCreate<DemoExample_gui_all_pictureBox>(g_demo->m_GUI);
		m_pictureBox->m_position.x = 0;
		m_pictureBox->m_position.y = 300;
		m_pictureBox->m_size.x = 256;
		m_pictureBox->m_size.y = 224;
		m_pictureBox->SetTexture(m_texture);
	}

	m_button1 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button1->m_position.Set(0.f, 525);
	m_button1->m_size.Set(200.f, 30.f);
	m_button1->SetText(U"Button");
	m_button1->SetFont(g_demo->m_guiFont);

	m_button2 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button2->m_position.Set(0.f, 555);
	m_button2->m_size.Set(200.f, 30.f);
	m_button2->SetText(U"m_lerpColors = true");
	m_button2->SetFont(g_demo->m_guiFont);
	m_button2->m_lerpColors = true;

	m_button3 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button3->m_position.Set(10.f, 590);
	m_button3->m_size.Set(200.f, 30.f);
	m_button3->SetText(U"round;size:6;iterations:3");
	m_button3->SetFont(g_demo->m_guiFont);
	m_button3->m_lerpColors = true;
	m_button3->m_roundCorners = true;
	m_button3->m_roundIterations = 3;
	m_button3->m_roundSize = 6;

	m_button4 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button4->m_position.Set(10.f, 630);
	m_button4->m_size.Set(200.f, 40.f);
	m_button4->SetText(U"round;size:15;iterations:3");
	m_button4->SetFont(g_demo->m_guiFont);
	m_button4->m_lerpColors = true;
	m_button4->m_roundCorners = true;
	m_button4->m_roundIterations = 3;
	m_button4->m_roundSize = 15;

	m_button5 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button5->m_position.Set(210.f, 630);
	m_button5->m_size.Set(200.f, 40.f);
	m_button5->SetText(U"round;size:15;iterations:4");
	m_button5->SetFont(g_demo->m_guiFont);
	m_button5->m_lerpColors = true;
	m_button5->m_roundCorners = true;
	m_button5->m_roundIterations = 4;
	m_button5->m_roundSize = 15;

	m_button6 = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_button6->m_position.Set(410.f, 630);
	m_button6->m_size.Set(200.f, 40.f);
	m_button6->SetText(U"Close Demo");
	m_button6->SetFont(g_demo->m_guiFont);
	m_button6->SetID(1);
	m_button6->m_lerpColors = true;
	m_button6->m_roundCorners = true;
	m_button6->m_roundIterations = 4;
	m_button6->m_roundSize = 15;
	m_button6->m_borderSize = 2;
	m_button6->SetUserData(this);

	m_buttonToggle = alCreate<DemoExample_gui_all_TestButton>(g_demo->m_GUI);
	m_buttonToggle->m_position.Set(210.f, 525);
	m_buttonToggle->m_size.Set(200.f, 30.f);
	m_buttonToggle->SetText(U"Toggle Button");
	m_buttonToggle->SetFont(g_demo->m_guiFont);
	m_buttonToggle->m_toggleButton = true;

	img = alLib::LoadALImage("../data/demo/3.png");
	if (img)
	{
		m_textureAtlasTexture = m_gs->CreateTexturePoint(img);
		AL_DESTROY(img);
	}
	m_textureAtlas = alCreate<alGUITextureAtlas>(m_textureAtlasTexture);
	uint32_t iconIndex_iconBase = m_textureAtlas->AddUV(alVec2u(0, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck1 = m_textureAtlas->AddUV(alVec2u(14, 0), alVec2u(14, 14));
	uint32_t iconIndex_iconCheck2 = m_textureAtlas->AddUV(alVec2u(28, 0), alVec2u(14, 14));

	m_buttonIcon1 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIcon1->m_position.Set(260.f, 350);
	m_buttonIcon1->m_size.Set(14.f, 14.f);
	m_buttonIcon1->m_lerpColors = true;

	m_buttonIcon2 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIcon2->m_position.Set(260.f, 365);
	m_buttonIcon2->m_size.Set(14.f, 14.f);
	m_buttonIcon2->m_lerpColors = true;
	m_buttonIcon2->m_toggleButton = true;

	m_buttonIcon3 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIcon3->m_position.Set(260.f, 380);
	m_buttonIcon3->m_size.Set(14.f, 14.f);
	m_buttonIcon3->m_lerpColors = true;
	m_buttonIcon3->m_toggleButton = true;
	m_buttonIcon3->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIcon3->m_iconIndexPress = iconIndex_iconCheck2;

	m_buttonIcon3textR = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIcon3textR->m_position.Set(260.f, 395.f);
	m_buttonIcon3textR->m_size.Set(14.f, 14.f);
	m_buttonIcon3textR->m_lerpColors = true;
	m_buttonIcon3textR->m_toggleButton = true;
	m_buttonIcon3textR->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIcon3textR->m_iconIndexPress = iconIndex_iconCheck2;
	m_buttonIcon3textR->m_useText = true;
	m_buttonIcon3textR->SetText(U"Checkbox");
	m_buttonIcon3textR->SetFont(g_demo->m_guiFontMini);

	m_buttonIconRadio1_1 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio1_1->m_position.Set(260.f, 410.f);
	m_buttonIconRadio1_1->m_size.Set(14.f, 14.f);
	m_buttonIconRadio1_1->m_lerpColors = true;
	m_buttonIconRadio1_1->m_toggleButton = true;
	m_buttonIconRadio1_1->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio1_1->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio1_1->m_radioButton = true;
	m_buttonIconRadio1_1->m_useText = true;
	m_buttonIconRadio1_1->SetText(U"Group 1");
	m_buttonIconRadio1_1->SetFont(g_demo->m_guiFontMini);
	m_buttonIconRadio1_1->RadioCheck();

	m_buttonIconRadio2_1 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio2_1->m_position.Set(260.f, 425.f);
	m_buttonIconRadio2_1->m_size.Set(14.f, 14.f);
	m_buttonIconRadio2_1->m_lerpColors = true;
	m_buttonIconRadio2_1->m_toggleButton = true;
	m_buttonIconRadio2_1->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio2_1->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio2_1->m_radioButton = true;
	m_buttonIconRadio2_1->m_useText = true;
	m_buttonIconRadio2_1->SetText(U"Group 1");
	m_buttonIconRadio2_1->SetFont(g_demo->m_guiFontMini);

	m_buttonIconRadio3_1 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio3_1->m_position.Set(260.f, 440.f);
	m_buttonIconRadio3_1->m_size.Set(14.f, 14.f);
	m_buttonIconRadio3_1->m_lerpColors = true;
	m_buttonIconRadio3_1->m_toggleButton = true;
	m_buttonIconRadio3_1->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio3_1->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio3_1->m_radioButton = true;
	m_buttonIconRadio3_1->m_useText = true;
	m_buttonIconRadio3_1->SetText(U"Group 1");
	m_buttonIconRadio3_1->SetFont(g_demo->m_guiFontMini);

	m_buttonIconRadio1_2 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio1_2->m_position.Set(360.f, 410.f);
	m_buttonIconRadio1_2->m_size.Set(14.f, 14.f);
	m_buttonIconRadio1_2->m_lerpColors = true;
	m_buttonIconRadio1_2->m_toggleButton = true;
	m_buttonIconRadio1_2->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio1_2->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio1_2->m_radioButton = true;
	m_buttonIconRadio1_2->m_useText = true;
	m_buttonIconRadio1_2->SetText(U"Group 2");
	// group 1 is 0 so group 2 is 1
	m_buttonIconRadio1_2->m_radioGroup = 1;
	m_buttonIconRadio1_2->SetFont(g_demo->m_guiFontMini);
	m_buttonIconRadio1_2->RadioCheck();

	m_buttonIconRadio2_2 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio2_2->m_position.Set(360.f, 425.f);
	m_buttonIconRadio2_2->m_size.Set(14.f, 14.f);
	m_buttonIconRadio2_2->m_lerpColors = true;
	m_buttonIconRadio2_2->m_toggleButton = true;
	m_buttonIconRadio2_2->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio2_2->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio2_2->m_radioButton = true;
	m_buttonIconRadio2_2->m_useText = true;
	m_buttonIconRadio2_2->SetText(U"Group 2");
	m_buttonIconRadio2_2->m_radioGroup = 1;
	m_buttonIconRadio2_2->SetFont(g_demo->m_guiFontMini);

	m_buttonIconRadio3_2 = alCreate<DemoExample_gui_all_TestButtonIcon>(g_demo->m_GUI, m_textureAtlas, iconIndex_iconBase);
	m_buttonIconRadio3_2->m_position.Set(360.f, 440.f);
	m_buttonIconRadio3_2->m_size.Set(14.f, 14.f);
	m_buttonIconRadio3_2->m_lerpColors = true;
	m_buttonIconRadio3_2->m_toggleButton = true;
	m_buttonIconRadio3_2->m_iconIndexMouseHover = iconIndex_iconCheck1;
	m_buttonIconRadio3_2->m_iconIndexPress = iconIndex_iconCheck1;
	m_buttonIconRadio3_2->m_radioButton = true;
	m_buttonIconRadio3_2->m_useText = true;
	m_buttonIconRadio3_2->SetText(U"Group 2");
	m_buttonIconRadio3_2->m_radioGroup = 1;
	m_buttonIconRadio3_2->SetFont(g_demo->m_guiFontMini);

	m_GUIPanel = g_demo->m_GUI->GetNewPanel();
	m_GUIPanel->AddElement(m_buttonIconRadio1_1);
	m_GUIPanel->AddElement(m_buttonIconRadio2_1);
	m_GUIPanel->AddElement(m_buttonIconRadio3_1);
	m_GUIPanel->AddElement(m_buttonIconRadio1_2);
	m_GUIPanel->AddElement(m_buttonIconRadio2_2);
	m_GUIPanel->AddElement(m_buttonIconRadio3_2);
	m_GUIPanel->AddElement(m_buttonIcon3textR);
	m_GUIPanel->AddElement(m_buttonIcon3);
	m_GUIPanel->AddElement(m_buttonIcon2);
	m_GUIPanel->AddElement(m_buttonIcon1);
	m_GUIPanel->AddElement(m_buttonToggle);
	m_GUIPanel->AddElement(m_button6);
	m_GUIPanel->AddElement(m_button5);
	m_GUIPanel->AddElement(m_button4);
	m_GUIPanel->AddElement(m_button3);
	m_GUIPanel->AddElement(m_button2);
	m_GUIPanel->AddElement(m_button1);
	m_GUIPanel->AddElement(m_textObject1);
	m_GUIPanel->AddElement(m_textObject2minifont);
	m_GUIPanel->AddElement(m_textObject2);
	m_GUIPanel->AddElement(m_pictureBox);
	m_GUIPanel->m_drawBG = false;
	m_GUIPanel->m_position.Set(0.f, 0.f);
	
	m_GUIPanel->m_size.Set(g_demo->m_mainWindow->m_clientSize.x,
		g_demo->m_mainWindow->m_clientSize.y);
	m_GUIPanel->Rebuild();

	

	return true;
}

void DemoExample_gui_all::Shutdown()
{
	m_buttonExitPressed = false;
	m_textObject2_mouseMoved = false;
	m_textObject2_mouseMoved_timer = 0.f;

	g_demo->m_GUI->m_radioGroups[0].m_activeButton = 0;
	g_demo->m_GUI->m_radioGroups[1].m_activeButton = 0;

	AL_DESTROY(m_buttonIconRadio1_1);
	AL_DESTROY(m_buttonIconRadio2_1);
	AL_DESTROY(m_buttonIconRadio3_1);
	AL_DESTROY(m_buttonIconRadio1_2);
	AL_DESTROY(m_buttonIconRadio2_2);
	AL_DESTROY(m_buttonIconRadio3_2);
	AL_DESTROY(m_buttonIcon3textR);
	AL_DESTROY(m_buttonIcon3);
	AL_DESTROY(m_buttonIcon2);
	AL_DESTROY(m_buttonIcon1);
	AL_DESTROY(m_buttonToggle);
	AL_DESTROY(m_button6);
	AL_DESTROY(m_button5);
	AL_DESTROY(m_button4);
	AL_DESTROY(m_button3);
	AL_DESTROY(m_button2);
	AL_DESTROY(m_button1);
	AL_DESTROY(m_texture);
	AL_DESTROY(m_textureAtlas);
	AL_DESTROY(m_textObject1);
	AL_DESTROY(m_textObject2);
	AL_DESTROY(m_textObject2minifont);
	AL_DESTROY(m_pictureBox);

	g_demo->m_GUI->DeleteAllPanels();
}


bool DemoExample_gui_all::Run()
{
	if (m_textObject2_mouseMoved)
	{
		m_textObject2_mouseMoved_timer += *g_demo->m_dt;
		if (m_textObject2_mouseMoved_timer > 0.125f)
		{
			m_textObject2_mouseMoved = false;
			m_textObject2_mouseMoved_timer = 0.f;
		}
	}

	g_demo->m_GUI->Update(*g_demo->m_dt);


	if (m_input->IsKeyHit(alInputKey::K_ESCAPE)
		|| m_buttonExitPressed)
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
		U"I can draw text without creating GUI object",
		43,
		g_demo->m_guiFont,
		alVec2f(0, 40),
		ColorBlue);
	m_gs->DrawText(
		U"GS has a method DrawRectangle() and drawing text is just",
		56,
		g_demo->m_guiFont,
		alVec2f(0, 55),
		ColorDimGrey);
	m_gs->DrawText(
		U"drawing a rectangle with texture",
		32,
		g_demo->m_guiFont,
		alVec2f(0, 70),
		ColorGainsboro);
	m_gs->DrawRectangle(alVec4f(0.f, 95, 150, 150), ColorFireBrick, ColorLavender);
	m_gs->DrawText(
		U"Here is just a rectangle",
		24,
		g_demo->m_guiFont,
		alVec2f(150, 95),
		ColorGhostWhite);

	m_gs->DrawText(
		U"You can set two colors",
		22,
		g_demo->m_guiFont,
		alVec2f(150, 110),
		ColorGainsboro);

	if (m_texture)
	{
		alVec4f uv;
		auto xy = m_texture->GetUV(115, 99);
		auto zw = m_texture->GetUV(123, 113);
		uv.x = xy.x;
		uv.y = xy.y;
		uv.z = zw.x;
		uv.w = zw.y;
		m_gs->DrawRectangle(alVec4f(0.f, 155, 150, 210),
			ColorWhite,
			ColorWhite,
			m_texture, &uv);
		m_gs->DrawText(
			U"Here rectangle with texture",
			27,
			g_demo->m_guiFont,
			alVec2f(150, 155),
			ColorGainsboro);
	}

	m_gs->DrawText(
		U"Drawing rectangle directly is easy, but you can't interact with it",
		66,
		g_demo->m_guiFont,
		alVec2f(0, 210),
		ColorTan);
	m_gs->DrawText(
		U"You need to create GUI object for clicking",
		42,
		g_demo->m_guiFont,
		alVec2f(0, 225),
		ColorTeal);

	if (m_textObject2_text.Size())
	{
		alVec2f ep;
		m_gs->DrawText(
			m_textObject2_text.c_str(),
			m_textObject2_text.Size(),
			g_demo->m_guiFont,
			alVec2f(150, 240),
			ColorThistle,
			&ep);

		if (m_textObject2_mouseMoved)
		{
			m_gs->DrawText(
				U"OnMouseMove",
				11,
				g_demo->m_guiFont,
				ep + alVec2f(1, 0),
				ColorTomato);
		}
	}

	if (m_texture)
	{
		m_gs->DrawText(
			U"Picture Box. Click on it.",
			25,
			g_demo->m_guiFont,
			alVec2f(256, 300),
			ColorTurquoise);
		m_gs->DrawText(
			m_pictureBox->m_clickedPosition.c_str(),
			m_pictureBox->m_clickedPosition.Size(),
			g_demo->m_guiFont,
			alVec2f(256, 320),
			ColorViolet);
		
	}

	m_gs->DrawText(
		U"This is ButtonIcon. This is button but it use texture in TextureAtlas",
		69,
		g_demo->m_guiFont,
		alVec2f(280, 330),
		ColorWhite);

	m_gs->DrawText(
		U"ButtonIcon can be modified to work like checkbox or radiobutton",
		63,
		g_demo->m_guiFont,
		alVec2f(300, 355),
		ColorWhite);

	m_gs->EndDrawGUI();
	m_gs->SwapBuffers();
	return true;
}